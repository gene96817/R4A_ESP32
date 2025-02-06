/**********************************************************************
  Genes_Line_Following.ino

  Gene's algorithm is modified code from 04.2_Track_Car example
**********************************************************************/

#include <Arduino.h>
#include <math.h>
#include "Freenove_4WD_Car_For_ESP32.h"
#include <HardwareSerial.h>




//****************************************
// Constants
//****************************************

#define BLF_DEBUG_MOTORS        0
#define BLF_DEBUG_STATES        0

#define BLF_SPEED_LV4   4000
#define BLF_SPEED_LV3   3000
#define BLF_SPEED_LV2   2500
#define BLF_SPEED_LV1   1500

#define BLT_SPEED_FAST      1500
#define BLT_SPEED_MEDIUM    1200
#define BLT_SPEED_SLOW      1000

#define BLT_TURN_THRESHOLD  100












//  ***********************
// calculate instructions for Move_Motor
//  ************************



int speed;      //relative speed from -20 to 20
float direction;  //degrees

int leftWheelSpeed;
int rightWheelSpeed;

void MOVE(int speed, float direction) {
  // Speed adjustment (PWM range depends on your motor driver, adjust if needed):
  // arguments are speed, input-lower-bound, input-upper-bound, output-lower-bound, output-upper-bound
  int power = map(speed, -20, 20, -4095, 4095);  // Assuming 4095 is your max PWM

  //what is relative wheel speed -- is there an offset for cos & sin?
  // for the relative wheel speed to be equal, we need the direction offset to be 45 degrees
 
  float  DirectionOffset = 45;

  // Direction conversion (from degrees to radians);
  // calculate heading in radians; directions is looking down;

  float directionRadians = (direction + DirectionOffset) * 2 * 3.1415 / 360; 
  // Serial.print ("  directionRadians  ");
  // Serial.print(directionRadians);


  // Wheel direction calculations:
  float leftWheelFactor = sin(directionRadians);
  float rightWheelFactor = cos(directionRadians);
  // Serial.print("  LeftWheelFactor  ");
  // Serial.print(leftWheelFactor);
  // Serial.print("  RightWheelFactor  ");
  // Serial.print(rightWheelFactor);
  // Serial.print("  \n");

  // Apply power to each wheel based on the factors:
  int leftWheelSpeed = power * leftWheelFactor;

if (leftWheelSpeed < 0) {leftWheelSpeed = min(leftWheelSpeed, -SPEED_MIN);}
         else if (leftWheelSpeed == 0) {leftWheelSpeed = 0;}
         else if (leftWheelSpeed > SPEED_MIN) {leftWheelSpeed = max(leftWheelSpeed, SPEED_MIN);};


  int rightWheelSpeed = power * rightWheelFactor;

  if (rightWheelSpeed < 0) {rightWheelSpeed = min(rightWheelSpeed, -SPEED_MIN);}
         else if (rightWheelSpeed == 0) {rightWheelSpeed = 0;}
         else if (rightWheelSpeed > SPEED_MIN) {rightWheelSpeed = max(rightWheelSpeed, SPEED_MIN);};

  

  // Motor control:
  Motor_Move(leftWheelSpeed, leftWheelSpeed, rightWheelSpeed, rightWheelSpeed);

//  ***********************
// calculate instructions for Move_Motor
//  ************************


int speed;      //relative speed from -20 to 20
float direction;  //degrees

int leftWheelSpeed;
int rightWheelSpeed;

void MOVE(int speed, float direction) {
  // Speed adjustment (PWM range depends on your motor driver, adjust if needed):
  // arguments are speed, input-lower-bound, input-upper-bound, output-lower-bound, output-upper-bound
  int power = map(speed, -20, 20, -4095, 4095);  // Assuming 4095 is your max PWM

  //what is relative wheel speed -- is there an offset for cos & sin?
  // for the relative wheel speed to be equal, we need the direction offset to be 45 degrees
 
  float  DirectionOffset = 45;

  // Direction conversion (from degrees to radians);
  // calculate heading in radians; directions is looking down;

  float directionRadians = (direction + DirectionOffset) * 2 * 3.1415 / 360; 
  // Serial.print ("  directionRadians  ");
  // Serial.print(directionRadians);


  // Wheel direction calculations:
  float leftWheelFactor = sin(directionRadians);
  float rightWheelFactor = cos(directionRadians);
  // Serial.print("  LeftWheelFactor  ");
  // Serial.print(leftWheelFactor);
  // Serial.print("  RightWheelFactor  ");
  // Serial.print(rightWheelFactor);
  // Serial.print("  \n");

  // Apply power to each wheel based on the factors:
  int leftWheelSpeed = power * leftWheelFactor;

if (leftWheelSpeed < 0) {leftWheelSpeed = min(leftWheelSpeed, -SPEED_MIN);}
         else if (leftWheelSpeed == 0) {leftWheelSpeed = 0;}
         else if (leftWheelSpeed > SPEED_MIN) {leftWheelSpeed = max(leftWheelSpeed, SPEED_MIN);};


  int rightWheelSpeed = power * rightWheelFactor;

  if (rightWheelSpeed < 0) {rightWheelSpeed = min(rightWheelSpeed, -SPEED_MIN);}
         else if (rightWheelSpeed == 0) {rightWheelSpeed = 0;}
         else if (rightWheelSpeed > SPEED_MIN) {rightWheelSpeed = max(rightWheelSpeed, SPEED_MIN);};

  

  // Motor control:
  Motor_Move(leftWheelSpeed, leftWheelSpeed, rightWheelSpeed, rightWheelSpeed);

  // ******************

bool trace = false;    // this enables all the print statements for debugging.

 if (trace) {
  // THis is debug output  suppress these messages to speed up the run time.
  Serial.print("  Move ");
  Serial.print(speed);
  Serial.print("  ");
  Serial.print(direction);
  Serial.print("  Left Wheel ");
  Serial.print(leftWheelSpeed);
  Serial.print("  Right Wheel ");
  Serial.print(rightWheelSpeed);
  Serial.print("  \n");
  };
}

//****************************************
// Locals
//****************************************


// this is memory of track detection
// emory of current and just previous wheel power (PW) and theta (direction)
int PT0 = 2;
int PT1 = 2;
int PT2 = 2;
int PT3 = 2;
int PT4 = 0;
int PT5 = 0;
int PW0 = 0;
int PW1 = 0;
int theta1 = 0;
int theta0 = 0;
// int start_time;  // read the internal real time clock = start time = 0
// int run_time;    // the runtime = run_time - start_time



int blfTimeBefore = 0;  //Record each non-blocking time
int blfTimeCount = 0;   //Record the number of non-blocking times
int blfTimeFlag = 0;    //Record the blink time


void showsensor(){
  if (trace) {
    Serial.print("  Sensor3210 ");  // Print the sensor value
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
  };
}


//*********************************************************************
// The robotRunning routine calls this routine to actually perform
// the challenge.  This routine typically reads a sensor and may
// optionally adjust the motors based upon the sensor reading.  The
// routine then must return.  The robot layer will call this routine
// multiple times during the robot operation.

//****************************
//  Track_Setup();   //Trace module initialization
//  PCA9685_Setup(); //Motor drive initialization
//  Emotion_Setup();

void blfChallenge(R4A_ROBOT_CHALLENGE * object)
{
    // Read the line sensors
    // was Track_Read();
    pcf8574.read(&lineSensors);
    lineSensors &= 7;
    if (BLF_DEBUG_STATES)
        Serial.printf("%d %d %d\r\n",
                      lineSensors & 1,
                      (lineSensors & 2) ? 1 : 0,
                      (lineSensors & 4) ? 1 : 0);

    // Update the robot direction
    // was 	switch (sensorValue[3])
    switch (lineSensors)
    {
    //     RcL
    case 0b000:
    case 0b111:
    default:
        // No line or stop circle detected
	// was  Motor_Move(0, 0, 0, 0);                                    //Stop
        r4aPca9685MotorBrakeAll();
        r4aRobotStop(&robot, millis());
        break;

    //     RcL
    case 0b010:
    case 0b101:
        // Robot over center of line
	// was    Motor_Move(SPEED_LV1, SPEED_LV1, SPEED_LV1, SPEED_LV1);    //Move Forward
        robotMotorSetSpeeds(BLF_SPEED_LV1,  BLF_SPEED_LV1); // Move Forward
        break;

    //     RcL
    case 0b001:
    case 0b011:
        // Robot over left sensor, need to turn left
	// was    Motor_Move(-SPEED_LV3, -SPEED_LV3, SPEED_LV4, SPEED_LV4);  //Turn Left
        robotMotorSetSpeeds(-BLF_SPEED_LV3, BLF_SPEED_LV4); // Turn left
        break;

    //     RcL
    case 0b100:
    case 0b110:
        // Robot over right sensor, need to turn right
	// was  Motor_Move(SPEED_LV4, SPEED_LV4 , - SPEED_LV3, -SPEED_LV3);//Turn Right
        robotMotorSetSpeeds(BLF_SPEED_LV4, -BLF_SPEED_LV3); // Turn right
        break;
    }
}

//*********************************************************************
// The robotStart calls this routine before switching to the initial
// delay state.
void blfInit(R4A_ROBOT_CHALLENGE * object)
{
    challengeInit();
}

_















//*********************************************************************
// The robot.stop routine calls this routine to stop the motors and
// perform any other actions.
void blfStop(R4A_ROBOT_CHALLENGE * object)
{
    // Stop the robot
    challengeStop();
    }

//*********************************************************************
// The initial delay routine calls this routine just before calling
// the challenge routine for the first time.
void blfStart(R4A_ROBOT_CHALLENGE * object)
{
    challengeStart();

    // Set the reference voltage from the photo-resistor voltage divider
    r4aEsp32VoltageGet(LIGHT_SENSOR_PIN,
                       0,
                       1,
                       &lsAdcReference);
}






//*********************************************************************
// Start the line following
void menuBlfStart(const struct _R4A_MENU_ENTRY * menuEntry,
                  const char * command,
                  Print * display)
{
    static R4A_ROBOT_CHALLENGE basicLineFollowing =
    {
        blfChallenge,
        blfInit,
        blfStart,
        blfStop,

        "Basic Line Following",         // _name
        ROBOT_LINE_FOLLOW_DURATION_SEC, // _duration
    };
    
    

void loop() {    

  char receivedChar = Serial.read();
  
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
                      &basicLineFollowing,
                      ROBOT_LINE_FOLLOW_DURATION_SEC,
                      display);
}


    
    
