//**********************************************************************
//  Genes_Line_Following.ino
//   This is Gene's code for the line following challeng.
//*********************************************************************


/***********************************************************************
This sketch is modified code from 01_Basic_Line_Following.ino which is modeled after the Freenove
  04.2_Track_Car example

  Modify the blfChallenge routine later in this file to read the line
  sensors and set the motor speeds (run the challenge.

  Flow of Control (Very Simplified):

                           Power Up
                               |
                               V
                             Setup
                               |
                               V
                     Robot Initialization
                               |
                               V
                               +<----------.
                               |           |
                               V           |
                             Loop          |
                               |           |
                               V           |
                         r4aRobotUpdate    | Infinite loop
                               |           |
                               '-----------'

  Call stack:

    setup - Sketch entry point
        r4aRobotInit - Initialize the robot layer
        r4aRobotStart - Starts the initial delay followed by the challenge

    loop - main routine
        r4aRobotUpdate - Updates the robot layer
            glfChallenge - Perform the basic line following
                pcf8574.read - Read line sensors
                    _i2cBus->_read - R4A I2C layer read routine
                        r4aEsp32I2cBusRead - R4A ESP32 I2C interface
                            requestFrom - ESP32 I2C controller API
                            endTransmission - ESP32 I2C controller API
                robotMotorSetSpeeds - Control motors
                    motor*.speed - Select PWM value
                        bufferLedOnOff - Select one PWM value
                    pca9685.writeBufferedRegisters - Write the PWM values
                        r4aI2cBusWrite - R4A I2C layer write routine
                            _writeWithLock - Generic I2C write routine
                                r4aEsp32I2cBusWriteWithLock - R4A ESP32 I2C interface
                                    write - ESP32 I2C controller API
                                    endTransmission - ESP32 I2C controller API
                r4aRobotStop - Stops the robot

    Where is the code?

        libraries
            R4A_Robot
                Robot layer
            R4A_I2C
                PWM code for motors
            R4A_ESP32
                This example
                ESP32 I2C controller layer
**********************************************************************/


//****************************************
// Constants
//****************************************

#define GLF_DEBUG_MOTORS        0
#define GLF_DEBUG_STATES        0

//****************************************
// Locals
//****************************************

int glfTimeBefore = 0;  //Record each non-blocking time
int glfTimeCount = 0;   //Record the number of non-blocking times
int glfTimeFlag = 0;    //Record the blink time





// The robotRunning routine calls this routine to actually perform
// the challenge.  This routine typically reads a sensor and may
// optionally adjust the motors based upon the sensor reading.  The
// routine then must return.  The robot layer will call this routine
// multiple times during the robot operation.

// The low level Expressif Wi-Fi layer soft Access Point (AP) 
// default config will generate a "ERROR: AP password must be >= 8 characters"
// The message can be suppressed with the setting "nvm/z WiFiApPass"
// wifi

// what result should process() return;
// what is the halting condition


   void glfChallenge(R4A_ROBOT_CHALLENGE* object)
   {
   Serial.printf("***** GlfChallenge called.\r\n");

    // Read the line sensors
       GetLineSensors();
       seeTrack();


       if (GLF_DEBUG_STATES) showsensor();

       static uint32_t lastDisplayMsec;
       const uint32_t DISPLAY_INTERVAL = 15000; // 15 seconds in ms

       if (millis() - lastDisplayMsec >= DISPLAY_INTERVAL) {
           lastDisplayMsec = millis();
        Serial.printf("Add your code to CLF::challenge()\r\n");
       }

   // Calculate the state index and get the action
    int stateIndex = PT2 * NUM_PT1_STATES + PT1;

    // Get the action from the state table
    const StateAction* action = &g_stateTable[stateIndex];

//Update the robot direction
executeAction(action);
}


//*********************************************************************
// Entry point for the application

//*********************************************************************
// The robotStart calls this routine before switching to the initial
// delay state.
void glfInit(R4A_ROBOT_CHALLENGE * object)
{
    challengeInit();
}

//*********************************************************************
// The initial delay routine calls this routine just before calling
// the challenge routine for the first time.
void glfStart(R4A_ROBOT_CHALLENGE * object)
{
    challengeStart();
}

//*********************************************************************
// The robot.stop routine calls this routine to stop the motors and
// perform any other actions.
void glfStop(R4A_ROBOT_CHALLENGE * object)
{
    challengeStop();
}

//*********************************************************************
// Start the line following
void menuGlfStart(const struct _R4A_MENU_ENTRY * menuEntry,
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
                      display);
}
