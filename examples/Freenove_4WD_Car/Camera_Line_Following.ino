/**********************************************************************
  Camera_Line_Following.ino

  Line following support using the OV2640 camera
**********************************************************************/

#ifdef  USE_OV2640

//*********************************************************************
// Do camera line following
void clfChallenge(R4A_ROBOT_CHALLENGE * object)
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
void clfInit(R4A_ROBOT_CHALLENGE * object)
{
    challengeInit();
}

//*********************************************************************
// Start the robot
void clfStart(R4A_ROBOT_CHALLENGE * object)
{
    // Set the camera position
    servoPan.positionSet(clfPanStartDegrees);
    servoTilt.positionSet(clfTiltStartDegrees);

    // Set the initial state
    challengeStart();
}

//*********************************************************************
// Stop the robot
void clfStop(R4A_ROBOT_CHALLENGE * object)
{
    // Stop the robot
    challengeStop();
}

//*********************************************************************
// Initialize the line following
void menuClfStart(const struct _R4A_MENU_ENTRY * menuEntry,
                  const char * command,
                  Print * display)
{
    static R4A_ROBOT_CHALLENGE cameraLineFollowing =
    {
        clfChallenge,
        clfInit,
        clfStart,
        clfStop,

        "Camera Line Following",        // _name
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
        r4aRobotStart(&robot,
                      &cameraLineFollowing,
                      ROBOT_LINE_FOLLOW_DURATION_SEC,
                      display);
}

#endif  // USE_OV2640
