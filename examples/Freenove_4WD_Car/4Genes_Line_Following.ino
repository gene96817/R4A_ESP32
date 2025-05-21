//**********************************************************************
//  Genes_Line_Following.ino
//
//*********************************************************************
// The robotRunning routine calls this routine to actually perform
// the challenge.  This routine typically reads a sensor and may
// optionally adjust the motors based upon the sensor reading.  The
// routine then must return.  The robot layer will call this routine
// multiple times during the robot operation.

// what result should process() return;
// what is the halting condition


   void GlfChallenge(R4A_ROBOT_CHALLENGE* object) {
       seeTrack();
       if (glf_DEBUG_STATES) showsensor();

       static uint32_t lastDisplayMsec;
       const uint32_t DISPLAY_INTERVAL = 15000; // 15 seconds in ms

       if (millis() - lastDisplayMsec >= DISPLAY_INTERVAL) {
           lastDisplayMsec = millis();
        Serial.printf("Add your code to CLF::challenge()\r\n");
       }
   }

//*********************************************************************
// The robotStart calls this routine before switching to the initial
// delay state.
void GlfInit(R4A_ROBOT_CHALLENGE* object) {
  challengeInit();
}

//*********************************************************************
// The initial delay routine calls this routine just before calling
// the challenge routine for the first time.
void GlfStart(R4A_ROBOT_CHALLENGE* object) {
  challengeStart();

  // Set the reference voltage from the photo-resistor voltage divider
  r4aEsp32VoltageGet(LIGHT_SENSOR_PIN,
                     0,
                     1,
                     &lsAdcReference);
}

//*********************************************************************
// Stop the robot and perform any other actions
void GlfStop(R4A_ROBOT_CHALLENGE * object)
{
    // Stop the robot
    challengeStop();
}

//*********************************************************************
// Start the line following

// read sensors
// call the statemachine to make a move



void menuGlfStart(const struct _R4A_MENU_ENTRY* menuEntry,
                  const char* command,
                  Print* display) {
  static R4A_ROBOT_CHALLENGE GenesLineFollowing = {
    GlfChallenge,
    GlfInit,
    GlfStart,
    GlfStop,

    "Gene's Line Following",          // _name
    ROBOT_LINE_FOLLOW_DURATION_SEC,  // _duration
  };
  float voltage;

  // Only start the robot if the battery is on
  if (!robotCheckBatteryLevel()) {
    voltage = READ_BATTERY_VOLTAGE(nullptr);
    display->printf("Battery: %f4.2V\r\n", voltage);
    display->println("WARNING: Battery voltage to run the robot!");
  } else
    // Start the robot challenge if the robot is not active
        r4aRobotStart(&robot,
                      &GenesLineFollowing,
                      ROBOT_LINE_FOLLOW_DURATION_SEC,
                      display);
}
