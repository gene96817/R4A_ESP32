/**********************************************************************
  Basic_Light_Tracking.ino

  Light tracking support
**********************************************************************/

//****************************************
// Constants
//****************************************

#define BLT_SPEED_FAST      1500
#define BLT_SPEED_MEDIUM    1200
#define BLT_SPEED_SLOW      1000

#define BLT_TURN_THRESHOLD  100

//*********************************************************************
// Implement the basic light tracking class
class BLT : public R4A_ROBOT_CHALLENGE
{
  public:

    // Constructor
    BLT(const char * name)
        : R4A_ROBOT_CHALLENGE(name, ROBOT_LIGHT_TRACKING_DURATION_SEC)
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
        // Read the voltage from the photo-resistor voltage divider
        r4aEsp32VoltageGet(LIGHT_SENSOR_PIN,
                           0,
                           1,
                           &lsAdcValue);

        // Is the light on the left side?
        if (lsAdcValue <= (lsAdcReference - BLT_TURN_THRESHOLD))
            blfMotorSetSpeeds(BLT_SPEED_MEDIUM, BLT_SPEED_FAST);

        // Is the light on the right side?
        else if (lsAdcValue >= (lsAdcReference + BLT_TURN_THRESHOLD))
            blfMotorSetSpeeds(BLT_SPEED_FAST, BLT_SPEED_MEDIUM);

        // Go straight
        else
            blfMotorSetSpeeds(BLT_SPEED_SLOW, BLT_SPEED_SLOW);
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
// Start the light tracking
void bltStart(const struct _R4A_MENU_ENTRY * menuEntry,
              const char * command,
              Print * display)
{
    static BLT basicLightTracking("Basic Light Tracking");
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
        robot.init(&basicLightTracking,
                   ROBOT_LIGHT_TRACKING_DURATION_SEC,
                   display);
}
