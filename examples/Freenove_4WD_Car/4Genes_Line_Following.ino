
bool trace = false;  // set to true to see debug messages

void search() {  //use this to find the track if overrun the track

  if (trace) {
    Serial.print("Lost track, starting search  ");
    Serial.print("\n");
  };
  //backup
  showsensor();
  SetMotor(10, -180);
  showsensor();
  SetMotor(10, -180);
  showsensor();
  SetMotor(10, -180);
  showsensor();
  SetMotor(10, -180);

  /* skip searching straight ahead... already failed...
//search straight ahead
for ( int i=0; i<5; i++) 
 { 
  Serial.print("Search forward");
   SetMotor(10, 0);
   Track_Read();  // get a new sensor reading
   if (sensorValue[3] != 0) break;
 }
// falls to here if did not find track straight ahead


//backup 
showsensor(); SetMotor(10,-180);
showsensor(); SetMotor(10,-180);
showsensor(); SetMotor(10,-180);
showsensor(); SetMotor(10,-180);

*/


  //search to right  up to 4 stepts
  showsensor();
  SetMotor(12, 70);

  for (int i = 0; i < 5; i++) {
    if (trace) { Serial.print("Searching to right  "); };
    showsensor();
    SetMotor(10, 60);
    Track_Read();  // get a new sensor reading
    if (sensorValue[3] != 0) break;
  }
  //backup and start pointing to the left
  showsensor();
  SetMotor(10, -160);
  showsensor();
  SetMotor(10, -160);
  showsensor();
  SetMotor(10, -160);
  showsensor();
  SetMotor(10, -160);

  //search to left
  showsensor();
  SetMotor(14, -220);

  for (int i = 0; i < 5; i++) {
    Serial.print("Searching to left  ");
    SetMotor(12, -20);
    Track_Read();  // get a new sensor reading


if (sensorValue[3] != 0) break;
  }
  //backup
  showsensor();
  SetMotor(10, -200);
  showsensor();
  SetMotor(10, -200);
  showsensor();
  SetMotor(10, -200);
  showsensor();
  SetMotor(10, -200);

  if (trace) { Serial.print("Search failed  "); };
}


// get a sensor reading

/****************************************
// make a move
void process(int PT1, int PT2, int PT3);
	// process looks up action in the state table
    // see stateMachine.cpp

// what result should process() return;
// what is the halting condition

*****************************************/





//*********************************************************************
// The robotRunning routine calls this routine to actually perform
// the challenge.  This routine typically reads a sensor and may
// optionally adjust the motors based upon the sensor reading.  The
// routine then must return.  The robot layer will call this routine
// multiple times during the robot operation.


void glfChallenge(R4A_ROBOT_CHALLENGE* object) {
  // Read the line sensors
  pcf8574.read(&lineSensors);
  lineSensors &= 7;
  if (glf_DEBUG_STATES)
    Serial.printf("%d %d %d\r\n",
                  lineSensors & 1,
                  (lineSensors & 2) ? 1 : 0,
                  (lineSensors & 4) ? 1 : 0);

  // Update the robot direction
  switch (lineSensors) {
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
      robotMotorSetSpeeds(glf_SPEED_LV1, glf_SPEED_LV1);  // Move Forward
      break;

    //     RcL
    case 0b001:
    case 0b011:
      // Robot over left sensor, need to turn left
      robotMotorSetSpeeds(-glf_SPEED_LV3, glf_SPEED_LV4);  // Turn left
      break;

    //     RcL
    case 0b100:
    case 0b110:
      // Robot over right sensor, need to turn right
      robotMotorSetSpeeds(glf_SPEED_LV4, -glf_SPEED_LV3);  // Turn right
      break;
  }
}

//*********************************************************************
// The robotStart calls this routine before switching to the initial
// delay state.
void glfInit(R4A_ROBOT_CHALLENGE* object) {
  challengeInit();
}

//*********************************************************************
// The initial delay routine calls this routine just before calling
// the challenge routine for the first time.
void glfStart(R4A_ROBOT_CHALLENGE* object) {
  challengeStart();

  // Set the reference voltage from the photo-resistor voltage divider
  r4aEsp32VoltageGet(LIGHT_SENSOR_PIN,
                     0,
                     1,
                     &lsAdcReference);
}

//*********************************************************************
// The robot.stop routine calls this routine to stop the motors and
// perform any other actions.
void glfStop(R4A_ROBOT_CHALLENGE* object) {
  challengeStop();
}

//*********************************************************************
// Start the line following
// original code

// void setup() {
// setup is done in Freenove_4WD_Car.ino


// void loop() {

/* this old code used to initialize the line following routine. Now not needed.

  char receivedChar = Serial.read();
  

  // Wait for user to press the "Enter" (carriage return) key

  // if (trace) {
    Serial.print("Press enter to start the car \n");
    // };
  while (!Serial.available()) {
    // Wait until a char arrives
    };
  Serial.read();
  // if (trace) {
  Serial.print("received a character, begin execution of code\n");
    // };

********

 */

// while (true) {
// change this to a single step routine that is repeatedly called
// change loop condition to loop forever

// gets here when a char is read

void GLF_make_a_step() {
  // run_time = millis() - start_time;  // calculate the time stamp
  Track_Read();  // get a sensor reading

  // update path history
  PT5 = PT4;
  PT4 = PT3;
  PT3 = PT2;
  PT2 = PT1;
  PT1 = PT0;
  PT0 = sensorValue[3];
  PW1 = PW0;


  // Suppress these debug messages to speed up run time.

  if (trace) {
    Serial.print("  Sensor3210 ");
    Serial.print(" ");
    Serial.print(PT5);
    Serial.print(" ");
    Serial.print(PT4);
    Serial.print(" ");
    Serial.print(PT3);
    Serial.print(" ");
    Serial.print(PT2);
    Serial.print(" ");
    Serial.print(PT1);
    Serial.print(" ");
    Serial.print(PT0);
    Serial.print("   ");
  }

  // WARNING.... check all code for PT2 and PT1 usage
  // PT1 is latest reading.... PT2 is previous reading



     //  delay(20);               // Move for 0.1 seconds then halt -- Warning: This halts code execution
     Motor_Move(0, 0, 0, 0);  // This halts the motors. Effect is the movement started above halts after the delay.
     // delay(10);

  // WARNING.... check all code for PT2 and PT1 usage
  // PT1 is latest reading.... PT2 is previous reading

  // stateTable(PT2 * 8 + PT1);

}


//end original code
//*****************************

void menuglfStart(const struct _R4A_MENU_ENTRY* menuEntry,
                  const char* command,
                  Print* display) {
  static R4A_ROBOT_CHALLENGE basicLineFollowing = {
    glfChallenge,
    glfInit,
    glfStart,
    glfStop,

    "Basic Line Following",          // _name
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
    robot.init(&basicLineFollowing,
               ROBOT_LINE_FOLLOW_DURATION_SEC,
               display);
}
