/**********************************************************************
  Basic_Line_Following.ino

  Perform basic line following, modified code from 04.2_Track_Car example
**********************************************************************/

//****************************************
// Constants
//****************************************

#define BLF_DEBUG_MOTORS        0
#define BLF_DEBUG_STATES        0

#define BLF_SPEED_LV4   4000
#define BLF_SPEED_LV3   3000
#define BLF_SPEED_LV2   2500
#define BLF_SPEED_LV1   1500

//****************************************
// Locals
//****************************************

int blfTimeBefore = 0;  //Record each non-blocking time
int blfTimeCount = 0;   //Record the number of non-blocking times
int blfTimeFlag = 0;    //Record the blink time

//*********************************************************************
// Set the speeds of the motors
// Start bit, I2C device address, ACK, register address, ACK, 8 data bytes
// with ACKs and a stop bit, all at 400 KHz
// 770 uSec = (1+8+1+8+1+((8+1)×32)+1)÷(400×1000)
// Returns true if successful, false otherwise
bool blfMotorSetSpeeds(int16_t left, int16_t right, Print * display)
{
    // Update motor speeds
    return motorFrontLeft.speed(left, display)
           && motorBackLeft.speed(left, display)
           && motorFrontRight.speed(right, display)
           && motorBackRight.speed(right, display)
           && pca9685.writeBufferedRegisters(display);
}

//****************************************
// Constants
//****************************************

#define BLT_SPEED_FAST      1500
#define BLT_SPEED_MEDIUM    1200
#define BLT_SPEED_SLOW      1000

#define BLT_TURN_THRESHOLD  100

//*********************************************************************
// Implement the basic line following class
class BLF : public R4A_ROBOT_CHALLENGE
{
  public:

    // Constructor
    BLF(const char * name)
        : R4A_ROBOT_CHALLENGE(name, ROBOT_LINE_FOLLOW_DURATION_SEC)
    {
    }

    //****************************************
    // The robotStart routine calls this routine to verify the battery
    // level.
    // Outputs:
    //   Returns true if the battery level is high enough to run the robot
    //   challenge and returns false if the battery needs to be changed.
    bool batteryLevel()
    {
        return robotCheckBatteryLevel();
    }

    //****************************************
    // The robotRunning routine calls this routine to actually perform
    // the challenge.  This routine typically reads a sensor and may
    // optionally adjust the motors based upon the sensor reading.  The
    // routine then must return.  The robot layer will call this routine
    // multiple times during the robot operation.
    void challenge()
    {
        // Read the line sensors
        pcf8574.read(&lineSensors);
        lineSensors &= 7;
        if (BLF_DEBUG_STATES)
            Serial.printf("%d %d %d\r\n",
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
            break;

        //     RcL
        case 0b010:
        case 0b101:
            // Robot over center of line
            blfMotorSetSpeeds(BLF_SPEED_LV1,  BLF_SPEED_LV1); // Move Forward
            break;

        //     RcL
        case 0b001:
        case 0b011:
            // Robot over left sensor, need to turn left
            blfMotorSetSpeeds(-BLF_SPEED_LV3, BLF_SPEED_LV4); // Turn left
            break;

        //     RcL
        case 0b100:
        case 0b110:
            // Robot over right sensor, need to turn right
            blfMotorSetSpeeds(BLF_SPEED_LV4, -BLF_SPEED_LV3); // Turn right
            break;
        }
    }

    //****************************************
    // The robotStart calls this routine before switching to the initial
    // delay state.
    void init()
    {
        challengeInit();
    }

    //****************************************
    // The initial delay routine calls this routine just before calling
    // the challenge routine for the first time.
    void start()
    {
        challengeStart();

        // Set the reference voltage from the photo-resistor voltage divider
        r4aEsp32VoltageGet(LIGHT_SENSOR_PIN,
                           0,
                           1,
                           &lsAdcReference);
    }

    //****************************************
    // The robotStop routine calls this routine to stop the motors and
    // perform any other actions.
    void stop()
    {
        challengeStop();
    }
};

//*********************************************************************
// Start the line following
void blfStart(const struct _R4A_MENU_ENTRY * menuEntry,
              const char * command,
              Print * display)
{
    static BLF basicLineFollowing("Basic Line Following");
    float voltage;

    // Only start the robot if the battery is on
    if (!robotCheckBatteryLevel())
    {
        voltage = READ_BATTERY_VOLTAGE(nullptr);
        display->printf("Battery: %f4.2V\r\n", voltage);
        display->println("WARNING: Battery voltage to run the robot!");
    }
    else
        // Start the robot challenge if the robot is not active
        robot.init(&basicLineFollowing,
                   ROBOT_LINE_FOLLOW_DURATION_SEC,
                   display);
}
