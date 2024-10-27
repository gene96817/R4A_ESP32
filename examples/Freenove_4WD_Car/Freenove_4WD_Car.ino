/**********************************************************************
  Freenove_4WD_Car.ino

  Sample sketch to control the Freenove 4WD car
  https://store.freenove.com/products/fnk0053
  https://www.amazon.com/gp/product/B08X6PTQFM/ref=ox_sc_act_title_1
**********************************************************************/

//****************************************
// Constants
//****************************************

//#define USE_NTRIP
#define USE_OV2640
//#define USE_ZED_F9P

#define DEBUG_BOOT              0
#define DEBUG_LOOP_CORE_0       0
#define DEBUG_LOOP_CORE_1       0

// Servo default starting position
#define SERVO_PAN_START     90  // Degrees

#define SERVO_TILT_START    15  // Degrees

//****************************************
// GNSS - Global Navigation Satellite System
//****************************************

#define GNSS_POINTS_PER_SECOND      1
#define GNSS_POINTS_TO_AVERAGE      (10 * GNSS_POINTS_PER_SECOND)   // 10 Seconds

//****************************************
// Includes
//****************************************

#include <R4A_Freenove_4WD_Car.h>   // Freenove 4WD Car configuration

#define DOWNLOAD_AREA       "/nvm/"

#include "Parameters.h"

//****************************************
// OV2640 camera
//****************************************

#ifdef  USE_OV2640
class OV2640 : public R4A_OV2640
{
  public:

    // Constructor
    // Inputs:
    //   i2cBus: R4A_I2C_BUS object address used to access the OV2640 camera
    //   i2cAddress: I2C address of the OV2640 camera
    //   pins: R4A_OV2640_PINS object containing the ESP32 GPIO pin numbers
    //   clockHz: OV2640 clock frequency input
    OV2640(R4A_I2C_BUS * i2cBus,
           int i2cAddress,
           const R4A_OV2640_PINS * pins,
           uint32_t clockHz)
        : R4A_OV2640(i2cBus, i2cAddress, pins, clockHz)
    {
    }

    // Process the frame buffer
    // Inputs:
    //   frameBuffer: Buffer containing the raw image data
    //   display: Address of Print object for output
    // Outputs:
    //   Returns true if the processing was successful and false upon error
    bool processFrameBuffer(camera_fb_t * frameBuffer,
                            Print * display);

    // Process the web server's frame buffer
    // Inputs:
    //   frameBuffer: Buffer containing the raw image data
    // Outputs:
    //   Returns true if the processing was successful and false upon error
    virtual bool processWebServerFrameBuffer(camera_fb_t * frameBuffer);
};

#endif  // USE_OV2640

//****************************************
// I2C bus configuration
//****************************************

// I2C addresses
#define ZEDF9P_I2C_ADDRESS      0x42

const R4A_I2C_DEVICE_DESCRIPTION i2cBusDeviceTable[] =
{
    {OV2640_I2C_ADDRESS,   "OV2640 Camera"},
    {PCA9685_I2C_ADDRESS,  "PCA9685 16-Channel LED controller, motors & servos"},
    {PCF8574_I2C_ADDRESS,  "PCF8574 8-Bit I/O Expander, line tracking"},
    {VK16K33_I2C_ADDRESS,  "VT16K33 16x8 LED controller, LED matrix"},
#ifdef  USE_ZED_F9P
    {ZEDF9P_I2C_ADDRESS,   "u-blox ZED F9P GNSS receiver"}
#endif  // USE_ZED_F9P
};
const int i2cBusDeviceTableEntries = sizeof(i2cBusDeviceTable) / sizeof(i2cBusDeviceTable[0]);

R4A_ESP32_I2C_BUS i2cBus(0, i2cBusDeviceTable, i2cBusDeviceTableEntries);
    R4A_PCA9685 pca9685(&i2cBus, PCA9685_I2C_ADDRESS, 50, 25 * 1000 * 1000);
        R4A_PCA9685_SERVO servoPan(&pca9685, 0, 0, 180);
        R4A_PCA9685_SERVO servoTilt(&pca9685, 1, 2, 150);
        R4A_PCA9685_MOTOR motorBackLeft(&pca9685, 8, 9);
        R4A_PCA9685_MOTOR motorBackRight(&pca9685, 11, 10);
        R4A_PCA9685_MOTOR motorFrontRight(&pca9685, 13, 12);
        R4A_PCA9685_MOTOR motorFrontLeft(&pca9685, 14, 15);
    R4A_PCF8574 pcf8574(&i2cBus, PCF8574_I2C_ADDRESS);
#ifdef  USE_OV2640
    OV2640 ov2640(&i2cBus, OV2640_I2C_ADDRESS, &r4aOV2640Pins, 20 * 1000 * 1000);
#endif  // USE_OV2640
#ifdef  USE_ZED_F9P
    R4A_ZED_F9P zedf9p(&i2cBus, ZEDF9P_I2C_ADDRESS);
#endif  // USE_ZED_F9P

bool ov2640Present;
bool vk16k33Present;
bool zedf9pPresent;

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
// NTRIP Client
//****************************************

#ifdef  USE_NTRIP
// Define the back-off intervals between connection attempts in milliseconds
const uint32_t r4aNtripClientBbackoffIntervalMsec[] =
{
    0,
    15 * R4A_MILLISECONDS_IN_A_SECOND,
    30 * R4A_MILLISECONDS_IN_A_SECOND,
     1 * R4A_MILLISECONDS_IN_A_MINUTE,
     2 * R4A_MILLISECONDS_IN_A_MINUTE,
};

const int r4aNtripClientBbackoffCount = sizeof(r4aNtripClientBbackoffIntervalMsec) / sizeof(r4aNtripClientBbackoffIntervalMsec[0]);

class NTRIP_CLIENT : public R4A_NTRIP_CLIENT
{
  private:

    // Get the active serial port
    Print * getSerial()
    {
        return &Serial;
    }

    // Get the I2C bus transaction size
    uint8_t i2cTransactionSize()
    {
#ifdef  USE_ZED_F9P
        if (zedf9pPresent)
            return zedf9p._i2cTransactionSize;
#endif  // USE_ZED_F9P
        return 32;
    }

    // Push data to the GNSS
    int pushRawData(uint8_t * buffer, int bytesToPush, Print * display)
    {
#ifdef  USE_ZED_F9P
        if (zedf9pPresent)
            return zedf9p.pushRawData(buffer, bytesToPush, display);
#endif  // USE_ZED_F9P
        return bytesToPush;
    }

  public:

    // Constructor
    NTRIP_CLIENT() : R4A_NTRIP_CLIENT()
    {
    }
};

NTRIP_CLIENT ntrip;
#endif  // USE_NTRIP

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

#define ROBOT_START_DELAY_SEC               5
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

//****************************************
// SPI support - WS2812 LEDs
//****************************************

R4A_SPI * r4aSpi = new R4A_ESP32_SPI();

//****************************************
// Web server
//****************************************

class WEB_SERVER : public R4A_WEB_SERVER
{
  public:

    // Constructor
    // Inputs:
    //   port: Port number for the web server
    WEB_SERVER(uint16_t port = 80) : R4A_WEB_SERVER(port)
    {
    }

    // Update the configuration
    // Inputs:
    //   config: Address of the HTTP config object
    void configUpdate(httpd_config_t * config);

    // Register the error handlers
    // Outputs:
    //   Returns true if the all of the error handlers were installed and
    //   false upon failure
    bool registerErrorHandlers();

    // Register the URI handlers
    // Outputs:
    //   Returns true if the all of the URI handlers were installed and
    //   false upon failure
    bool registerUriHandlers();
};

WEB_SERVER webServer(80);

//****************************************
// WiFi support
//****************************************

R4A_TELNET_SERVER telnet(menuTable, menuTableEntries);

const R4A_SSID_PASSWORD r4aWifiSsidPassword[]
{
    {wifiSSID, wifiPassword},
    {wifiSSID2, wifiPassword2},
    {wifiSSID3, wifiPassword3},
    {wifiSSID4, wifiPassword4},
};
const int r4aWifiSsidPasswordEntries = sizeof(r4aWifiSsidPassword)
                                     / sizeof(r4aWifiSsidPassword[0]);

R4A_WIFI wifi(nullptr, nullptr);

//*********************************************************************
// Entry point for the application
void setup()
{
    BaseType_t status;

    // Initialize the USB serial port
    Serial.begin(115200);
    Serial.println();
    Serial.printf("Freenove 4WD Car\r\n");

    // Display the core number
    if (DEBUG_BOOT)
    {
        Serial.printf("setup() running on core %d\r\n", xPortGetCoreID());
        Serial.flush();
    }

    // Get the parameters
    if (DEBUG_BOOT)
        callingRoutine("r4aEsp32NvmGetParameters");
    r4aEsp32NvmGetParameters(&parameterFilePath);

    // Enable web server debugging
    r4aWebServerDebug = webServerDebug ? &Serial : nullptr;

    // Set the ADC reference voltage
    if (DEBUG_BOOT)
        callingRoutine("r4aEsp32VoltageSetReference");
    r4aEsp32VoltageSetReference(ADC_REFERENCE_VOLTAGE);

    // Turn off the buzzer
    if (DEBUG_BOOT)
    {
        Serial.printf("Turning off the buzzer\r\n");
        Serial.flush();
    }
    pinMode(BLUE_LED_BUZZER_PIN, OUTPUT);
    digitalWrite(BLUE_LED_BUZZER_PIN, ESP32_WROVER_BLUE_LED_OFF);

    // Turn off ESP32 Wrover blue LED when battery power is applied
    if (DEBUG_BOOT)
    {
        Serial.printf("Setting the blue LED\r\n");
        Serial.flush();
    }
    float batteryVoltage = READ_BATTERY_VOLTAGE(nullptr);
    int blueLED = (batteryVoltage > 2.)
                ? ESP32_WROVER_BLUE_LED_ON : ESP32_WROVER_BLUE_LED_OFF;
    digitalWrite(BLUE_LED_BUZZER_PIN, blueLED);

    // Delay to allow the hardware initialize
    delay(1000);

    // Start the core 0 task
    if (DEBUG_BOOT)
        callingRoutine("xTaskCreatePinnedToCore");
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
    {
        Serial.printf("Core 0 task started\r\n");
        Serial.flush();
    }

    // Start WiFi if enabled
    if (DEBUG_BOOT)
    {
        Serial.printf("Calling wifiBegin\r\n");
        Serial.flush();
    }
    if (wifiDebug)
        wifi._debug = &Serial;
    wifi.begin(mdnsHostName);

#ifdef  USE_NTRIP
    // Validate the NTRIP tables
    if (DEBUG_BOOT)
        callingRoutine("ntrip.validateTables");
    ntrip.validateTables();
#endif  // USE_NTRIP

    // Initialize the NTP client
    if (DEBUG_BOOT)
        callingRoutine("r4aNtpSetup");
    r4aNtpSetup(-10 * R4A_SECONDS_IN_AN_HOUR, true);

    // Initialize the SPI controller for the WD2812 LEDs
    if (!r4aLEDSetup(2, BATTERY_WS2812_PIN, 4 * 1000 * 1000, car.numberOfLEDs))
        r4aReportFatalError("Failed to allocate the SPI device for the WS2812 LEDs!");

    // Turn off all of the 3 color LEDs
    if (DEBUG_BOOT)
        callingRoutine("car.ledsOff");
    car.ledsOff();

    // Reduce the LED intensity
    if (DEBUG_BOOT)
        callingRoutine("r4aLEDSetIntensity");
    r4aLEDSetIntensity(1);

    // Set the initial LED values
    if (DEBUG_BOOT)
        callingRoutine("r4aLEDUpdate");
    r4aLEDUpdate(true);

    //****************************************
    // Synchronize with core 0
    //****************************************

    // Wait for the other core to finish initialization
    if (DEBUG_BOOT)
    {
        Serial.printf("Waiting for setupCore0 to complete\r\n");
        Serial.flush();
    }
    while (!core0Initialized)
        delayMicroseconds(1);
    if (DEBUG_BOOT)
        callingRoutine("r4aNtpSetup");

    //****************************************
    // Core 1 completed initialization
    //****************************************

    // Finished with the initialization
    if (DEBUG_BOOT)
    {
        Serial.printf("setup complete\r\n");
        Serial.flush();
    }
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
            callingRoutine("READ_BATTERY_VOLTAGE");
        float batteryVoltage = READ_BATTERY_VOLTAGE(nullptr);
        int blueLED = (batteryVoltage > 2.)
                    ? ESP32_WROVER_BLUE_LED_ON : ESP32_WROVER_BLUE_LED_OFF;
        digitalWrite(BLUE_LED_BUZZER_PIN, blueLED);
    }

    // Update the location
#ifdef  USE_ZED_F9P
    if (zedf9pPresent)
    {
        if (DEBUG_LOOP_CORE_1)
            callingRoutine("zedf9p.update");
        zedf9p.update(currentMsec, nullptr);
    }
#endif  // USE_ZED_F9P

    // Update the LEDs
    if (DEBUG_LOOP_CORE_1)
        callingRoutine("car.ledsUpdate");
    car.ledsUpdate(currentMsec);

    // Determine if WiFi station mode is configured
    if (r4aWifiSsidPasswordEntries)
    {
        // Determine if WiFi is connected
        wifiConnected = wifi._connected;

        // Check for NTP updates
        if (DEBUG_LOOP_CORE_1)
            callingRoutine("r4aNtpUpdate");
        r4aNtpUpdate(wifiConnected);

#ifdef  USE_NTRIP
        // Update the NTRIP client state
        if (DEBUG_LOOP_CORE_1)
            callingRoutine("ntrip.update\r\n");
        ntrip.update(wifiConnected);
#endif  // USE_NTRIP

        // Notify the telnet server of WiFi changes
        if (DEBUG_LOOP_CORE_1)
            callingRoutine("telnet.update");
        telnet.update(wifiConnected);
        if (previousConnected != wifiConnected)
        {
            previousConnected = wifiConnected;
            if (wifiConnected)
                Serial.printf("Telnet: %s:%d\r\n", WiFi.localIP().toString().c_str(),
                              telnet.port());
        }

        // Update the web server
        if (DEBUG_LOOP_CORE_1)
            callingRoutine("webServer.update");
        webServer.update(wifiConnected && webServerEnable);
    }

    // Process the next image
#ifdef  USE_OV2640
    if (ov2640Present && ov2640Enable)
        ov2640.update();
#endif  // USE_OV2640

    // Process serial commands
    if (DEBUG_LOOP_CORE_1)
        callingRoutine("r4aSerialMenu");
    r4aSerialMenu(&serialMenu);
}

//*********************************************************************
// Setup for core 0
void setupCore0(void *parameter)
{
    // Display the core number
    if (DEBUG_BOOT)
    {
        Serial.printf("setupCore0() running on core %d\r\n", xPortGetCoreID());
        Serial.flush();
    }

    // Allow I2C devices time to power up
    delay(100);

    // Setup and enumerate the I2C devices
    if(DEBUG_BOOT)
        callingRoutine("i2cBus.begin");
    i2cBus.begin(I2C_SDA,
                 I2C_SCL,
                 R4A_I2C_FAST_MODE_HZ);

    // Determine which devices are present
    if(DEBUG_BOOT)
        callingRoutine("i2cBus.isDevicePresent");
    ov2640Present = i2cBus.isDevicePresent(OV2640_I2C_ADDRESS);
    vk16k33Present = i2cBus.isDevicePresent(VK16K33_I2C_ADDRESS);
    zedf9pPresent = i2cBus.isDevicePresent(ZEDF9P_I2C_ADDRESS);

    // Initialize the PCA9685
    if(DEBUG_BOOT)
        callingRoutine("pca9685.begin");
    if (pca9685.begin())
    {
        // Initialize the Pan/Tilt servos
        if(DEBUG_BOOT)
            callingRoutine("servoPan.positionSet");
        servoPan.positionSet(servoPanStartDegrees);
        if(DEBUG_BOOT)
            callingRoutine("servoTilt.positionSet");
        servoTilt.positionSet(servoTiltStartDegrees);
    }

    // Initialize the PCF8574
    if(DEBUG_BOOT)
        callingRoutine("pcf8574.write");
    pcf8574.write(0xff);

    // Initialize the camera
#ifdef USE_OV2640
    if (ov2640Present)
    {
        if(DEBUG_BOOT)
            callingRoutine("ov2640.setup");
        Serial.printf("Initializing the OV2640 camera\r\n");
        ov2640.setup(PIXFORMAT_RGB565);
    }
#endif  // USE_OV2640

    // Initialize the GPS receiver
#ifdef  USE_ZED_F9P
    if (zedf9pPresent)
    {
        if(DEBUG_BOOT)
            callingRoutine("zedf9p.begin");
        Serial.printf("Initializing the ZED-F9P GNSS receiver\r\n");
        zedf9p.begin();
    }
#endif  // USE_ZED_F9P

    //****************************************
    // Core 0 completed initialization
    //****************************************

    if (DEBUG_BOOT)
    {
        Serial.printf("setupCore0 complete\r\n");
        Serial.flush();
    }

    // Finished with the initialization
    core0Initialized = true;

    //****************************************
    // Synchronize with core 1
    //****************************************

    if (DEBUG_BOOT)
    {
        Serial.printf("Waiting for setup to complete\r\n");
        Serial.flush();
    }

    // Wait for the other core to finish initialization
    while (!core1Initialized)
        delayMicroseconds(1);

    if(DEBUG_BOOT)
        callingRoutine("loopCore0");

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
    static uint32_t lastGnssI2cPollMsec;

    // Get the time since boot
    currentMsec = millis();

#ifdef  USE_ZED_F9P
    // Update the location
    if (zedf9pPresent)
    {
        if ((currentMsec - lastGnssI2cPollMsec) >= r4aZedF9pPollMsec)
        {
            lastGnssI2cPollMsec = currentMsec;
            if (DEBUG_LOOP_CORE_0)
                callingRoutine("zedf9p.i2cPoll");
            zedf9p.i2cPoll();
        }
    }
#endif  // USE_ZED_F9P

#ifdef  USE_NTRIP
    // Send navigation data to the GNSS radio
    if (r4aNtripClientEnable)
    {
        if (DEBUG_LOOP_CORE_0)
            callingRoutine("ntrip.rbRemoveData");
        ntrip.rbRemoveData(r4aNtripClientDebugRtcm ? &Serial : nullptr);
    }
#endif  // USE_NTRIP

    // Perform the robot challenge
    if (DEBUG_LOOP_CORE_0)
        callingRoutine("robot.update");
    robot.update(currentMsec);
}

//*********************************************************************
// Display the name of the next routine that will be executed
void callingRoutine(const char * name)
{
    Serial.printf("Calling %s on core %d\r\n", name, xPortGetCoreID());
    Serial.flush();
}
