/**********************************************************************
  Freenove_4WD_Car.ino

  Sample sketch to control the Freenove 4WD car
  https://store.freenove.com/products/fnk0053
  https://www.amazon.com/gp/product/B08X6PTQFM/ref=ox_sc_act_title_1
**********************************************************************/

#define DEBUG_BOOT              0
#define DEBUG_LOOP_CORE_0       0
#define DEBUG_LOOP_CORE_1       0

//****************************************
// Includes
//****************************************

#include <R4A_Freenove_4WD_Car.h>   // Freenove 4WD Car configuration

#include "Parameters.h"

//****************************************
// I2C bus configuration
//****************************************

USE_I2C_BUS_TABLE;

//****************************************
// Battery macros
//****************************************

#define ADC_REFERENCE_VOLTAGE   3.48    // Volts

#define DISPLAY_BATTERY_VOLTAGE(display)  \
    r4aEsp32VoltageDisplay(BATTERY_WS2812_PIN, 0, BATTERY_VOLTAGE_MULTIPLIER, display)

#define READ_BATTERY_VOLTAGE(adcValue)  \
    r4aEsp32VoltageGet(BATTERY_WS2812_PIN, 0, BATTERY_VOLTAGE_MULTIPLIER, adcValue)

//****************************************
// CPU core management
//****************************************

volatile bool core0Initialized;
volatile bool core1Initialized;

//****************************************
// Light Sensor
//****************************************

int16_t lsAdcReference;
int16_t lsAdcValue;

//****************************************
// Line sensors
//****************************************

uint8_t lineSensors;        // Last value of the line sensors

//****************************************
// Menus
//****************************************

extern const R4A_MENU_TABLE menuTable[];
extern const int menuTableEntries;

//****************************************
// Motor support
//****************************************

USE_MOTOR_TABLE;

bool robotMotorSetSpeeds(int16_t left, int16_t right, Print * display = nullptr);

//****************************************
// Robot
//****************************************

#define ROBOT_MINIMUM_VOLTAGE       8.0 // Don't start robot if below this voltage

R4A_Freenove_4WD_Car car;

//****************************************
// Robot operation
//****************************************

#define ROBOT_LIGHT_TRACKING_DURATION_SEC   (3 * R4A_SECONDS_IN_A_MINUTE)
#define ROBOT_LINE_FOLLOW_DURATION_SEC      (3 * R4A_SECONDS_IN_A_MINUTE)

#define ROBOT_START_DELAY_SEC               10
#define ROBOT_START_DELAY_MILLISECONDS      (ROBOT_START_DELAY_SEC * R4A_MILLISECONDS_IN_A_SECOND)

#define ROBOT_STOP_TO_IDLE_SEC              30

bool ignoreBatteryCheck;
void robotIdle(uint32_t currentMsec);
void robotDisplayTime(uint32_t milliseconds);

R4A_ROBOT robot(0,                          // CPU core
                ROBOT_START_DELAY_SEC,      // Challenge start delay
                ROBOT_STOP_TO_IDLE_SEC,     // Delay after running the challenge
                robotIdle,                  // Idle routine
                robotDisplayTime);          // Time display routine

//****************************************
// Serial menu support
//****************************************

R4A_MENU serialMenu(menuTable, menuTableEntries);

//****************************************
// Servos
//****************************************

USE_SERVO_TABLE;

// Servo limits and default starting position
#define SERVO_PAN_START     90  // Degrees

#define SERVO_TILT_START    15  // Degrees

//****************************************
// SPI support - WS2812 LEDs
//****************************************

R4A_SPI * r4aSpi = new R4A_ESP32_SPI();

//****************************************
// WiFi menu support
//****************************************

uint8_t wifiApCount;
WiFiMulti wifiMulti;
R4A_TELNET_SERVER telnet(menuTable, menuTableEntries);

//*********************************************************************
// Entry point for the application
void setup()
{
    BaseType_t status;

    // Initialize the USB serial port
    Serial.begin(115200);
    Serial.println();
    Serial.println("Freenove 4WD Car");

    // Display the core number
    if (DEBUG_BOOT)
        Serial.printf("setup() running on core %d\r\n", xPortGetCoreID());

    // Get the parameters
    if (DEBUG_BOOT)
        Serial.println("Calling r4aEsp32NvmGetParameters");
    r4aEsp32NvmGetParameters(&parameterFilePath);

    // Set the ADC reference voltage
    if (DEBUG_BOOT)
        Serial.println("Calling r4aEsp32VoltageSetReference");
    r4aEsp32VoltageSetReference(ADC_REFERENCE_VOLTAGE);

    // Turn off the buzzer
    if (DEBUG_BOOT)
        Serial.println("Turning off the buzzer");
    pinMode(BLUE_LED_BUZZER_PIN, OUTPUT);
    digitalWrite(BLUE_LED_BUZZER_PIN, ESP32_WROVER_BLUE_LED_OFF);

    // Turn off ESP32 Wrover blue LED when battery power is applied
    if (DEBUG_BOOT)
        Serial.println("Setting the blue LED");
    float batteryVoltage = READ_BATTERY_VOLTAGE(nullptr);
    int blueLED = (batteryVoltage > 2.)
                ? ESP32_WROVER_BLUE_LED_ON : ESP32_WROVER_BLUE_LED_OFF;
    digitalWrite(BLUE_LED_BUZZER_PIN, blueLED);

    // Start the core 0 task
    if (DEBUG_BOOT)
        Serial.println("Calling xTaskCreatePinnedToCore");
    status = xTaskCreatePinnedToCore(
                  setupCore0,   // Function to implement the task
                  "Core 0",     // Name of the task
                  10000,        // Stack size in words
                  NULL,         // Task input parameter
                  0,            // Priority of the task
                  nullptr,      // Task handle.
                  0);           // Core where the task should run
    if (status != pdPASS)
        r4aReportFatalError("Failed to create the core 0 task!");
    if (DEBUG_BOOT)
        Serial.println("Core 0 task started");

    // Delay to allow the hardware initialize
    delay(250);

    // Set the WiFi access point credentials
    wifiApCount = 0;
    if (wifiSSID && strlen(wifiSSID))
    {
        if (DEBUG_BOOT)
            Serial.printf("Calling wifiMulti.addAP(%s, password)\r\n", wifiSSID);
        if (wifiMulti.addAP(wifiSSID, wifiPassword))
            wifiApCount += 1;
    }
    if (wifiSSID2 && strlen(wifiSSID2))
    {
        if (DEBUG_BOOT)
            Serial.printf("Calling wifiMulti.addAP(%s, password2)\r\n", wifiSSID2);
        if (wifiMulti.addAP(wifiSSID2, wifiPassword2))
            wifiApCount += 1;
    }
    if (wifiSSID3 && strlen(wifiSSID3))
    {
        if (DEBUG_BOOT)
            Serial.printf("Calling wifiMulti.addAP(%s, password3)\r\n", wifiSSID3);
        if (wifiMulti.addAP(wifiSSID3, wifiPassword3))
            wifiApCount += 1;
    }
    if (wifiSSID2 && strlen(wifiSSID4))
    {
        if (DEBUG_BOOT)
            Serial.printf("Calling wifiMulti.addAP(%s, password4)\r\n", wifiSSID4);
        if (wifiMulti.addAP(wifiSSID4, wifiPassword4))
            wifiApCount += 1;
    }

    // Start the WiFi network
    if (wifiApCount)
    {
        Serial.printf("Waiting for WiFi to start on core %d \r\n", xPortGetCoreID());
        wifiMulti.run();
    }

    // Initialize the NTP client
    if (DEBUG_BOOT)
        Serial.println("Calling r4aNtpSetup");
    r4aNtpSetup(-10 * R4A_SECONDS_IN_AN_HOUR, true);

    // Initialize the SPI controller for the WD2812 LEDs
    if (!r4aLEDSetup(2, BATTERY_WS2812_PIN, 4 * 1000 * 1000, car.numberOfLEDs))
        r4aReportFatalError("Failed to allocate the SPI device for the WS2812 LEDs!");

    // Turn off all of the 3 color LEDs
    if (DEBUG_BOOT)
        Serial.println("Calling car.ledsOff");
    car.ledsOff();

    // Reduce the LED intensity
    if (DEBUG_BOOT)
        Serial.println("Calling r4aLEDSetIntensity");
    r4aLEDSetIntensity(1);

    // Set the initial LED values
    if (DEBUG_BOOT)
        Serial.println("Calling r4aLEDUpdate");
    r4aLEDUpdate(true);

    //****************************************
    // Synchronize with core 0
    //****************************************

    // Wait for the other core to finish initialization
    if (DEBUG_BOOT)
        Serial.println("Waiting for setupCore0 to complete");
    while (!core0Initialized)
        delayMicroseconds(1);
    if (DEBUG_BOOT)
        Serial.println("Calling r4aNtpSetup");

    //****************************************
    // Core 1 completed initialization
    //****************************************

    // Finished with the initialization
    if (DEBUG_BOOT)
        Serial.println("setup complete");
    core1Initialized = true;

    //****************************************
    // Execute loop forever
    //****************************************
}

//*********************************************************************
// Idle loop for core 1 of the application
void loop()
{
    uint32_t currentMsec;
    static uint32_t lastBatteryCheckMsec;
    static bool previousConnected;
    bool wifiConnected;

    // Turn on the ESP32 WROVER blue LED when the battery power is OFF
    currentMsec = millis();
    if ((currentMsec - lastBatteryCheckMsec) >= 100)
    {
        lastBatteryCheckMsec = currentMsec;
        if (DEBUG_LOOP_CORE_1)
            Serial.println("Calling READ_BATTERY_VOLTAGE");
        float batteryVoltage = READ_BATTERY_VOLTAGE(nullptr);
        int blueLED = (batteryVoltage > 2.)
                    ? ESP32_WROVER_BLUE_LED_ON : ESP32_WROVER_BLUE_LED_OFF;
        digitalWrite(BLUE_LED_BUZZER_PIN, blueLED);
    }

    // Update the LEDs
    if (DEBUG_LOOP_CORE_1)
        Serial.println("Calling car.ledsUpdate");
    car.ledsUpdate(currentMsec);

    // Determine if WiFi is enabled
    if (wifiApCount)
    {
        // Determine if WiFi is connected
        wifiConnected = (WiFi.status() == WL_CONNECTED);

        // Check for NTP updates
        if (DEBUG_LOOP_CORE_1)
            Serial.println("Calling r4aNtpUpdate");
        r4aNtpUpdate(wifiConnected);

        // Notify the telnet server of WiFi changes
        if (DEBUG_LOOP_CORE_1)
            Serial.println("Calling telnet.update");
        telnet.update(wifiConnected);
        if (previousConnected != wifiConnected)
        {
            previousConnected = wifiConnected;
            if (wifiConnected)
                Serial.printf("Telnet: %s:%d\r\n", WiFi.localIP().toString().c_str(),
                              telnet.port());
        }
    }

    // Process serial commands
    if (DEBUG_LOOP_CORE_1)
        Serial.println("Calling r4aSerialMenu");
    r4aSerialMenu(&serialMenu);
}

//*********************************************************************
// Setup for core 0
void setupCore0(void *parameter)
{
    // Display the core number
    if (DEBUG_BOOT)
        Serial.printf("setupCore0() running on core %d\r\n", xPortGetCoreID());

    // Allow I2C devices time to power up
    delay(100);

    // Initialize I2C
    if(DEBUG_BOOT)
        Serial.printf("Calling i2cBusSetup on core %d \r\n", xPortGetCoreID());

    // Setup and enumerate the I2C devices
    i2cBus.begin(I2C_SDA,
                 I2C_SCL,
                 R4A_I2C_FAST_MODE_HZ);

    // Initialize the PCA9685
    if(DEBUG_BOOT)
        Serial.printf("PCA9685 operating on core %d \r\n", xPortGetCoreID());
    if (pca9685.begin())
    {
        // Initialize the Pan/Tilt servos
        if(DEBUG_BOOT)
            Serial.printf("Calling servoSetup on core %d \r\n", xPortGetCoreID());
        servoPan.positionSet(SERVO_PAN_START);
        servoTilt.positionSet(SERVO_TILT_START);
    }

    // Initialize the PCF8574
    if(DEBUG_BOOT)
        Serial.printf("PCF8574 operating on core %d \r\n", xPortGetCoreID());
    pcf8574.write(0xff);

    //****************************************
    // Core 0 completed initialization
    //****************************************

    if (DEBUG_BOOT)
        Serial.println("setupCore0 complete");

    // Finished with the initialization
    core0Initialized = true;

    //****************************************
    // Synchronize with core 1
    //****************************************

    if (DEBUG_BOOT)
        Serial.println("Waiting for setup to complete");

    // Wait for the other core to finish initialization
    while (!core1Initialized)
        delayMicroseconds(1);

    if(DEBUG_BOOT)
        Serial.printf("Calling loopCore0 on core %d \r\n", xPortGetCoreID());

    //****************************************
    // Execute loopCore0 forever
    //****************************************

    while (1)
        loopCore0();
}

//*********************************************************************
// Idle loop for core 0 the application
void loopCore0()
{
    uint32_t currentMsec;

    // Get the time since boot
    currentMsec = millis();

    // Perform the robot challenge
    if (DEBUG_LOOP_CORE_0)
        Serial.println("Calling robot.update");
    robot.update(currentMsec);
}
