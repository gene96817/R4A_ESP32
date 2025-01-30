/**********************************************************************
  03_Telnet_Output.ino

  Perform basic line following with telnet output
**********************************************************************/

#include <R4A_Freenove_4WD_Car.h>   // Freenove 4WD Car configuration
#include "secrets.h"

//****************************************
// Constants
//****************************************

#define BLF_DEBUG_LINE_SENSORS      0

#define BLF_SPEED_LV4   4000
#define BLF_SPEED_LV3   3000
#define BLF_SPEED_LV1   1500

//****************************************
// I2C bus configuration
//****************************************

const R4A_I2C_DEVICE_DESCRIPTION i2cBusDeviceTable[] =
{
    {PCA9685_I2C_ADDRESS,  "PCA9685 16-Channel LED controller, motors & servos"},
    {PCF8574_I2C_ADDRESS,  "PCF8574 8-Bit I/O Expander, line tracking"},
    {VK16K33_I2C_ADDRESS,  "VT16K33 16x8 LED controller, LED matrix"},
};

R4A_I2C_BUS i2cBus =
{
    &Wire,              // _i2cBus
    i2cBusDeviceTable,  // _deviceTable
    sizeof(i2cBusDeviceTable) / sizeof(i2cBusDeviceTable[0]), // _deviceTableEntries
    0,                  // _lock
    {0,},               // _present
    r4aEsp32I2cBusWriteWithLock, // _writeWithLock
    r4aEsp32I2cBusRead, // _read
    false,              // _enumerated
};

R4A_I2C_BUS * r4aI2cBus; // I2C bus for menu system

R4A_PCA9685 pca9685(&i2cBus, PCA9685_I2C_ADDRESS, 50, 25 * 1000 * 1000);
    R4A_PCA9685_MOTOR motorBackLeft(&pca9685, 8, 9);
    R4A_PCA9685_MOTOR motorBackRight(&pca9685, 11, 10);
    R4A_PCA9685_MOTOR motorFrontRight(&pca9685, 13, 12);
    R4A_PCA9685_MOTOR motorFrontLeft(&pca9685, 14, 15);
R4A_PCF8574 pcf8574(&i2cBus, PCF8574_I2C_ADDRESS);

//****************************************
// Forward routine declarations
//****************************************

void blfChallenge(R4A_ROBOT_CHALLENGE * object);
void blfDisplayTime(uint32_t deltaMsec);
void blfIdle(uint32_t mSecToStart);
void blfInit(struct _R4A_ROBOT_CHALLENGE * object);
void blfStop(R4A_ROBOT_CHALLENGE * object);
void blfStart(struct _R4A_ROBOT_CHALLENGE * object);
bool robotMotorSetSpeeds(int16_t left, int16_t right, Print * display = nullptr);

//****************************************
// Locals
//****************************************

uint32_t lastDisplayMsec;
uint32_t startMsec;

USE_MOTOR_TABLE;

#define ROBOT_LINE_FOLLOW_DURATION_SEC      (3 * R4A_SECONDS_IN_A_MINUTE)
#define ROBOT_START_DELAY_SEC               5
#define ROBOT_STOP_TO_IDLE_SEC              30

R4A_ROBOT robot;

R4A_ROBOT_CHALLENGE basicLineFollowing =
{
    blfChallenge,
    blfInit,
    blfStart,
    blfStop,

    "Basic Line Following", // _name
    ROBOT_STOP_TO_IDLE_SEC  // Seconds between stop and idle
};

//****************************************
// WiFi support
//****************************************

#define TELNET_PORT         23

bool telnetFailed;
R4A_TELNET_SERVER telnet(4,         // Max clients
                         nullptr,   // ProcessInput
                         nullptr,   // Create
                         nullptr);  // Delete

WiFiMulti wifiMulti;

//*********************************************************************
// The robotRunning routine calls this routine to actually perform
// the challenge.  This routine typically reads a sensor and may
// optionally adjust the motors based upon the sensor reading.  The
// routine then must return.  The robot layer will call this routine
// multiple times during the robot operation.
void blfChallenge(R4A_ROBOT_CHALLENGE * object)
{
    uint32_t currentMsec;
    uint8_t lineSensors;

    // Display the time
    currentMsec = millis();
    if ((currentMsec - lastDisplayMsec) >= 100)
    {
        lastDisplayMsec = currentMsec;
        blfDisplayTime(currentMsec - startMsec);
    }

    // Read the line sensors
    pcf8574.read(&lineSensors);
    lineSensors &= 7;
    log_d("%d %d %d",
          lineSensors & 1,
          (lineSensors & 2) ? 1 : 0,
          (lineSensors & 4) ? 1 : 0);

    // Update the robot direction
    switch (lineSensors)
    {
    //     RcL
    case 0b000:
    case 0b111:
    default:
        // No line or stop circle detected
        r4aPca9685MotorBrakeAll();
        r4aRobotStop(&robot, millis(), &telnet);
        break;

    //     RcL
    case 0b010:
    case 0b101:
        // Robot over center of line
        robotMotorSetSpeeds(BLF_SPEED_LV1,  BLF_SPEED_LV1); // Move Forward
        break;

    //     RcL
    case 0b001:
    case 0b011:
        // Robot over left sensor, need to turn left
        robotMotorSetSpeeds(-BLF_SPEED_LV3, BLF_SPEED_LV4); // Turn left
        break;

    //     RcL
    case 0b100:
    case 0b110:
        // Robot over right sensor, need to turn right
        robotMotorSetSpeeds(BLF_SPEED_LV4, -BLF_SPEED_LV3); // Turn right
        break;
    }
}

//*********************************************************************
// Entry point for the application
void setup()
{
    BaseType_t status;

    // Initialize the USB serial port
    Serial.begin(115200);
    Serial.println();
    Serial.printf("%s\r\n", __FILE__);

    // Delay to allow the hardware initialize
    delay(1000);

    // Initialize the I2C bus for line sensors and motor control
    log_v("Calling i2cBus.begin");
    r4aEsp32I2cBusBegin(&i2cBus,
                        I2C_SDA,
                        I2C_SCL,
                        R4A_I2C_FAST_MODE_HZ);
    r4aI2cBus = &i2cBus;

    // Initialize the PCA9685 for motor control
    log_v("Calling pca9685.begin");
    pca9685.begin();

    // Scan WiFi for possible remote APs
    Serial.println("Scanning WiFi ");
    wifiMulti.addAP(wifiSSID1, wifiPassword1);
    wifiMulti.addAP(wifiSSID2, wifiPassword2);

    // Connect to a remote AP if possible
    Serial.println("Connecting Wifi ");
    for (int loops = 10; loops > 0; loops--)
    {
        if (wifiMulti.run() == WL_CONNECTED)
            break;
        else
        {
            Serial.println(loops);
            delay(1000);
        }
    }
    if (wifiMulti.run() != WL_CONNECTED)
    {
        Serial.println("WiFi connect failed");
        delay(1000);
        ESP.restart();
    }

    // Wait for WiFi to connect
    while (WiFi.STA.connected() == false)
        delay(100);

    // Start the telnet server
    telnet.begin(WiFi.STA.localIP(), TELNET_PORT);

    // Wait for a telnet client
    Serial.printf("Waiting for telnet client connection to %s:%d\r\n",
                  WiFi.STA.localIP().toString().c_str(), TELNET_PORT);
    telnetFailed = false;
    while (telnet.hasClient() == false)
    {
        delay(100);
        telnet.update(WiFi.STA.connected());
    }
    Serial.printf("Telnet client connected\r\n");
    telnet.printf("%s\r\n", __FILE__);

    // Initialize the robot
    telnet.printf("Calling r4aRobotInit to initialize the robot\r\n");
    Serial.printf("Start initial delay\r\n");
    r4aRobotInit(&robot,
                 xPortGetCoreID(),       // CPU core
                 ROBOT_START_DELAY_SEC,  // Challenge start delay
                 ROBOT_STOP_TO_IDLE_SEC, // Delay after running the challenge
                 blfIdle,                // Idle routine
                 blfDisplayTime);        // Time display routine

    // Start the robot challenge
    telnet.printf("Calling r4aRobotStart to select the robot challenge\r\n");
    r4aRobotStart(&robot,
                  &basicLineFollowing,
                  ROBOT_LINE_FOLLOW_DURATION_SEC,
                  &telnet);

    // Execute loop forever
}

//*********************************************************************
// Idle loop for core 1 of the application
void loop()
{
    static bool previousConnected = true;
    bool wifiConnected;

    // Perform the robot challenge
    r4aRobotUpdate(&robot, millis());

    // Determine if WiFi is connected
    wifiConnected = WiFi.STA.connected();

    // Notify the telnet server of WiFi changes
    if (previousConnected != wifiConnected)
    {
        previousConnected = wifiConnected;
        if (wifiConnected)
        {
            telnet.begin(WiFi.STA.localIP(), TELNET_PORT);
            Serial.printf("Telnet: %s:%d\r\n", WiFi.localIP().toString().c_str(),
                          telnet.port());
        }
        else
        {
            telnet.end();
        }
    }

    // Update the telnet clients
    telnet.update(wifiConnected);

    // Stop the robot when the telnet clients disconnect
    if ((telnet.hasClient() == false) && r4aRobotIsActive(&robot))
        r4aRobotStop(&robot, millis());
}

//*********************************************************************
// Display the delta time
// Called by the init routine to display the countdown time
// Called by the initial delay routine to display the countdown time
// Called by the stop routine to display the actual challenge duration
// Inputs:
//   deltaMsec: Milliseconds to display
void blfDisplayTime(uint32_t deltaMsec)
{
    uint32_t minutes;
    uint32_t seconds;
    uint32_t milliseconds;

    milliseconds = deltaMsec;
    seconds = milliseconds / R4A_MILLISECONDS_IN_A_SECOND;
    milliseconds -= seconds * R4A_MILLISECONDS_IN_A_SECOND;
    minutes = seconds / R4A_SECONDS_IN_A_MINUTE;
    seconds -= minutes * R4A_SECONDS_IN_A_MINUTE;
    if (telnet.hasClient())
        telnet.printf("%d:%02d.%d\r\n", minutes, seconds, milliseconds / 100);
    else if (!telnetFailed)
    {
        telnetFailed = true;
        Serial.printf("Telnet clients disconnected!\r\n");
    }
}

//*********************************************************************
// Called by the update routine when the robot is not running a challenge
// The initial delay routine calls this routine just before calling
// the challenge routine for the first time.
// Inputs:
//   deltaMsec: Milliseconds to display
void blfIdle(uint32_t mSecToStart)
{
    static bool displayed;

    if (!displayed)
    {
        displayed = true;
        Serial.printf("Idle\r\n");
        if (telnet.hasClient())
            telnet.printf("blfIdle\r\n");
    }

    // Check for telnet connection
    if (telnetFailed == false)
    {
        if (telnet.hasClient() == false)
        {
            telnetFailed = true;
            Serial.printf("Telnet clients disconnected!\r\n");
        }
    }
}

//*********************************************************************
// Initialize the robot challenge
// The robotStart calls this routine before switching to the initial
// delay state.
// Inputs:
//   object: Address of a R4A_ROBOT_CHALLENGE data structure
void blfInit(struct _R4A_ROBOT_CHALLENGE * object)
{
    if (telnet.hasClient())
        telnet.printf("blfInit\r\n");
    else if (!telnetFailed)
    {
        telnetFailed = true;
        Serial.printf("Telnet clients disconnected!\r\n");
    }
}

//*********************************************************************
// Start the robot challenge
// The initial delay routine calls this routine just before calling
// the challenge routine for the first time.
// Inputs:
//   object: Address of a R4A_ROBOT_CHALLENGE data structure
void blfStart(struct _R4A_ROBOT_CHALLENGE * object)
{
    Serial.printf("Start\r\n");
    if (telnet.hasClient())
        telnet.printf("blfStart\r\n");
    else if (!telnetFailed)
    {
        telnetFailed = true;
        Serial.printf("Telnet clients disconnected!\r\n");
    }
    startMsec = millis();
    lastDisplayMsec = 0;
}

//*********************************************************************
// The robot.stop routine calls this routine to stop the motors and
// perform any other actions.
void blfStop(R4A_ROBOT_CHALLENGE * object)
{
    if (telnet.hasClient())
        telnet.printf("blfStop\r\n");
    else if (!telnetFailed)
    {
        telnetFailed = true;
        Serial.printf("Telnet clients disconnected!\r\n");
    }

    // Apply the brakes
    r4aPca9685MotorBrakeAll();
}

//*********************************************************************
// Set the speeds of the motors
// Start bit, I2C device address, ACK, register address, ACK, 8 data bytes
// with ACKs and a stop bit, all at 400 KHz
// 770 uSec = (1+8+1+8+1+((8+1)×32)+1)÷(400×1000)
// Returns true if successful, false otherwise
bool robotMotorSetSpeeds(int16_t left, int16_t right, Print * display)
{
    // Update motor speeds
    return motorFrontLeft.speed(left, display)
           && motorBackLeft.speed(left, display)
           && motorFrontRight.speed(right, display)
           && motorBackRight.speed(right, display)
           && pca9685.writeBufferedRegisters(display);
}
