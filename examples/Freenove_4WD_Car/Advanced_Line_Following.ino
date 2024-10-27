/**********************************************************************
  Advanced_Line_Following.ino

  Advanced line following support
**********************************************************************/

//*********************************************************************
// Implement the advanced line following class
class ALF : public R4A_ROBOT_CHALLENGE
{
  public:

    // Constructor
    ALF(const char * name)
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
    void challenge();

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
    // The robot.stop routine calls this routine to stop the motors and
    // perform any other actions.
    void stop()
    {
        challengeStop();
    }
};

//*********************************************************************
// Stop the robot
void alfStop()
{
    // Stop the robot
    robot.stop(millis());
}

//*********************************************************************
// Do the line following
void ALF::challenge()
{
    static uint32_t lastDisplayMsec;
    if ((millis() - lastDisplayMsec) >= (15 * 1000))
    {
        lastDisplayMsec = millis();
        Serial.printf("Add your code to CLF::challenge()\r\n");
    }
}

//*********************************************************************
// Start the line following
void alfStart(const struct _R4A_MENU_ENTRY * menuEntry,
              const char * command,
              Print * display)
{
    static ALF advancedLineFollowing("Advanced Line Following");
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
        robot.init(&advancedLineFollowing,
                   ROBOT_LINE_FOLLOW_DURATION_SEC,
                   display);
}
