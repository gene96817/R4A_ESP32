/**********************************************************************
  Advanced_Line_Following.ino

  Advanced line following support
**********************************************************************/

//*********************************************************************
// Do the line following
void alfChallenge(R4A_ROBOT_CHALLENGE * object)
{
    static uint32_t lastDisplayMsec;
    if ((millis() - lastDisplayMsec) >= (15 * 1000))
    {
        lastDisplayMsec = millis();
        Serial.printf("Add your code to CLF::challenge()\r\n");
    }
}

//*********************************************************************
// The robotStart calls this routine before switching to the initial
// delay state.
void alfInit(R4A_ROBOT_CHALLENGE * object)
{
    challengeInit();
}

//*********************************************************************
// The initial delay routine calls this routine just before calling
// the challenge routine for the first time.
void alfStart(R4A_ROBOT_CHALLENGE * object)
{
    challengeStart();

    // Set the reference voltage from the photo-resistor voltage divider
    r4aEsp32VoltageGet(LIGHT_SENSOR_PIN,
                       0,
                       1,
                       &lsAdcReference);
}

//*********************************************************************
// Stop the robot and perform any other actions
void alfStop(R4A_ROBOT_CHALLENGE * object)
{
    // Stop the robot
    challengeStop();
}

//*********************************************************************
// Start the line following
void menuAlfStart(const struct _R4A_MENU_ENTRY * menuEntry,
                  const char * command,
                  Print * display)
{
    static R4A_ROBOT_CHALLENGE advancedLineFollowing =
    {
        alfChallenge,
        alfInit,
        alfStart,
        alfStop,

        "Advanced Line Following",      // _name
        ROBOT_LINE_FOLLOW_DURATION_SEC, // _duration
    };
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
