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

#include <stdint.h>
#include "4GenesLineFollowing.h"

// Function declarations
void glfChallenge(R4A_ROBOT_CHALLENGE* object);
void glfInit(R4A_ROBOT_CHALLENGE* object);
void glfStart(R4A_ROBOT_CHALLENGE* object);
void glfStop(R4A_ROBOT_CHALLENGE* object);
bool haltingcondition(int PT1, int PT2, int PT3);
void menuGlfStart(const struct _R4A_MENU_ENTRY* menuEntry, const char* command, Print* display);
//****************************************
// Constants
//****************************************

#define GLF_DEBUG_MOTORS        0
#define GLF_DEBUG_STATES        0

//****************************************
// Globals
//****************************************

float inchderate = 0.85f;             // inchderate is a factor to derate the speed
                                    // derate factor to the speeds in the state table
                                    // default is inchderate = 1.0, which is no derate
uint32_t glfRunTime = 0;         // Record the run time in milliseconds
//****************************************
// Locals
//****************************************

int glfTimeBefore = 0;  //Record each non-blocking time
int glfTimeCount = 0;   //Record the number of non-blocking times
int glfTimeFlag = 0;    //Record the blink time
int lastStripTime = 0; // Last time the strip was updated

/****************** code using these already commented out
static bool inchworm = false;      // inchworm = false  ... motors are braked
                                  //          = true    ... motors are running
static uint32_t inchrun = 20;      // time duration for the motors to run (i.e. creep)
// static uint32_t inchpause = 2;   // time interval for the motors to pause (i.e. stop)
static uint32_t inchtimeend = 0;   // time for the next toggle between run and pause
*************************/




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

	// check for halting condition
bool haltingcondition(int PT1, int PT2, int PT3) {
  if (PT1 == 7 && PT2 == 7 && PT3 == 7) {
    traceLog("Halting state detected: PT1=7, PT2=7, PT3=7.");
   return true; // Halting condition met
}
    return false; // No halting condition met
  }



   void glfChallenge(R4A_ROBOT_CHALLENGE* object)
   {
    uint32_t currentMsec;
    currentMsec = millis();
    // Serial.printf("%ld; %s\r\n", currentMsec, inchworm ? "r" : "p"  );

/*********************************   replace this code with minmal pause state

if (inchworm == false) {           // if inchworm is false, check if inchpause has expired
      if (currentMsec > inchtimeend) {  // AND if inchtimeend is passed,
            inchworm = true;           // then set inchworm to true (run) (change state)
            inchtimeend = currentMsec + inchrun; // set next end time for motor run
                                    // fall through to run motors
    //    Serial.printf("***** GlfChallenge called.\r\n");


    // this section is to slow down the loop to allow the motors to run
    // and the robot to creep along the track

    // Serial.printf("***** Inchworm States: Time: %lu, inchworm: %d, inchtimeend: %lu, inchrun: %lu, inchpause: %lu\r\n",
    //            currentMsec, inchworm, inchtimeend, inchrun, inchpause);
 //   Serial.printf("***** Inchworm States: Time: %lu, inchworm: %d, inchtimeend: %lu\r\n",
 //               currentMsec, inchworm, inchtimeend);
    } else {                       // if inchpause has not expired,
            return;                    // pause not expired, stay in pause state
        }
    } else {                          // inchworm == true, motors are running
        if (currentMsec < inchtimeend) { // check if run time has not expired
            return;                   // if run time not expired, return   // if not expired, stay in run state
        } else {                      // run time expired, change to pause state
            inchworm = false;         // set inchworm to false (pause)
            inchtimeend = currentMsec + inchpause; // calculate pause end time
            robotMotorSetSpeeds(0, 0); // stop the motors
            return;
        }
    }
************************************/

/**********************************
ELIMINATE INCHWORM STATES
if (inchworm == false) {           // this is in the pause state
            inchworm = true;           // then set inchworm to true (run) (change state)
            inchtimeend = currentMsec + inchrun; // set next end time for motor run
                                    // fall through to run motors
    //    Serial.printf("***** GlfChallenge called.\r\n");


    // Serial.printf("***** Inchworm States: Time: %lu, inchworm: %d, inchtimeend: %lu, inchrun: %lu, inchpause: %lu\r\n",
    //            currentMsec, inchworm, inchtimeend, inchrun, inchpause);
 //   Serial.printf("***** Inchworm States: Time: %lu, inchworm: %d, inchtimeend: %lu\r\n",
 //               currentMsec, inchworm, inchtimeend);


    } else {                          // inchworm is in the run state
        if (currentMsec < inchtimeend) { // check if run time has not expired
            return;                   // if run time not expired, return   // if not expired, stay in run state
        } else {                      // run time expired, change to pause state
            inchworm = false;         // set inchworm to false (pause)
            robotMotorSetSpeeds(0, 0); // stop the motors
            return;
        }
    }
END OR INCHWORM STATES
**********************************/

if ((currentMsec - lastStripTime) > 5) { // check if current time is not more than
        Serial.printf("**** WARNING TOOK TOO LONG TO START THIS STRIP ***** \r\n");                   // if run time not expired, return   // if not expired, stay in run state
        } else {
         // fall through to update motor settings
    }


    // Read the line sensors
    getLineSensors();
    seeTrack();
    // Serial.printf("seeTrack returned\r\n");

    // check for halting condition
    // PT1, PT2, PT3 are the last three readings of the sensors


if (haltingcondition(PT1, PT2, PT3)) {
    Serial.printf("***** Halting condition met: PT1: %d, PT2: %d, PT3: %d\r\n", PT1, PT2, PT3);
	r4aRobotStop(&robot,millis()); // Stop the robot if halting condition is met
	return; // Exit the challenge routine
    }

    // Update the position history
    pushHistory(PT0);
    // showHistory(); // Debugging output of the history

    // Calculate the state index and get the action
    int stateIndex = PT2 * NUM_PT1_STATES + PT1;
    // Serial.printf("***** Current stateIndex: %d\r\n", stateIndex);
    // Get the action from the state table
    const StateAction* action = &g_stateTable[stateIndex];

    //Update the robot direction
    executeAction(action);
	lastStripTime = millis(); // Update the last strip time
								// expect next update to be within 5 milliseconds
    Serial.printf("***** Current sensors see PT1-PT2: %d-%d Speed: %d, Direction: %d, Trace: %s\r\n",
            PT1, PT2, action->speed, action->direction, action->comment);
     return; // Return to the robot layer, which will call this routine again
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
	lastStripTime = millis(); // Initialize the last strip time

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
        glfChallenge,
        glfInit,
        glfStart,
        glfStop,
        "Gene's Line Following",         // _name
   //     ROBOT_LINE_FOLLOW_DURATION_SEC, // _duration
		600      // allow 10 minutes for the challenge
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