/****************************************
 * This file provides the functions for moving one increment.
 * The intention is for higher-level control to make a series of
 * moves to follow a line.
 *
 * The functions included:
 *   - MOVE
 *   - stateTable: Retrieve decision of how to move
 *   - MoveMotor: The command to the motors to make a move
 ****************************************/


#include <array>
#include <functional>
#include "4GenesLineFollowing.h"


// Constants
#define NUM_PT1_STATES 8
#define NUM_PT2_STATES 8
#define NUM_STATES (NUM_PT1_STATES * NUM_PT2_STATES)

// Global variables (since we're using C)
static int g_speed = 0;
static int g_direction = 0;
static String g_comment = "No action taken";

// static bool g_trace = true; // Enable tracing by default
// Forward declarations


// Define the state table as a global constant array
static const StateAction g_stateTable[NUM_STATES] = {

  // PT2=0: Actions for PT1=0 to PT1=7
  { 6, 180, "Lost the track" },                   // PT1=0  Need a find track action
  { 10, 40, "Recenter forward motion" },          // PT1=1
  { 10, 0, "Stay on track" },                     // PT1=2
  { 10, 60, "Missed right turn" },                // PT1=3
  { 10, -40, "Recenter with left drift" },        // PT1=4
  { 10, 30, "Re-centering from a fork" },         // PT1=5
  { 10, -60, "Sharp left turn" },                 // PT1=6
  { 4, 0, "Halting condition?" },                 // PT1=7

  // PT2=1: Actions for PT1=0 to PT1=7
  { 6, 180, "Lost track drifting left" },        // PT1=0
  { 10, 40, "Recenter forward motion" },          // PT1=1
  { 10,  0, "Steady, stay centered" },            // PT1=2
  { 10, 60, "Missed right turn" },                // PT1=3
  { 10, -40, "Recenter with left drift" },        // PT1=4
  { 10, 30, "Re-centering from a fork" },         // PT1=5
  { 10, -60, "Left turn" },                       // PT1=6
  { 4, 0, "Halting condition?" },                 // PT1=7

  // PT2=2: Actions for PT1=0 to PT1=7
  { 6, 180, "Lost track" },                         // PT1=0
  { 10, 40, "Recenter forward motion" },           // PT1=1
  { 10, 0, "Now centered"  },                      // PT1=2
  { 10, 30, "Missed right turn" },                // PT1=3
  { 10, -40, "Recenter with left drift" },        // PT1=4
  { 10, 30, "Re-centering from a fork" },         // PT1=5
  { 10, -60, "Left turn turn" },                  // PT1=6
  { 4, 0, "Halting condition?" },                 // PT1=7

  // PT2=3: Actions for PT1=0 to PT1=7
  { 6, -15, "Lost track drifting right" },        // PT1=0
  { 10, 40, "Recenter forward motion" },          // PT1=1
  { 10, 0, "Now centered" },                      // PT1=2
  { 10, 60, "Missed right turn" },                // PT1=3
  { 10, -40, "Recenter with left drift" },        // PT1=4
  { 10, 30, "Re-centering from a fork" },         // PT1=5
  { 10, -60, "Left turn" },                      // PT1=6
  { 4, 0, "Halting condition?" },                 // PT1=7

  // PT2=4: Actions for PT1=0 to PT1=7
  { 6, 180, "Lost track drifting left" },        // PT1=0
  { 10, 40, "Recenter forward motion" },         // PT1=1
  { 10, 0, "Now centered" },                   // PT1=2
  { 10, 60, "Missed right turn" },               // PT1=3
  { 10, -40, "Recenter with left drift" },       // PT1=4
  { 10, 30, "Re-centering from a fork" },         // PT1=5
  { 10, -60, "Sharp right turn" },                 // PT1=6
  { 4, 0, "Halting condition?" },                 // PT1=7

  // PT2=5: Actions for PT1=0 toPT1=7
  { 6, 180, "Lost track" },                     // PT1=0
  { 10, 40, "Recenter forward motion" },         // PT1=1
  { 10, 0, "Now centered" },                    // PT1=2
  { 10, 60, "Missed right turn" },               // PT1=3
  { 10, -40, "Recenter with left drift" },       // PT1=4
  { 10, 30, "Re-centering from a fork" },         // PT1=5
  { 10, -60, "Sharp left turn" },                 // PT1=6
  { 4, 0, "Halting condition?" },                 // PT1=7

  // PT2=6: Special halt detection states. PT1=0..6 are false detections, PT1=7 is the actual stop.
  { 6, 180, "Lost track drifting left" },              // PT1=0
  { 10, 40, "Over corrected, right to center car" },  // PT1=1
  { 10, 0, "Now centered"  },                        // PT1=2
  { 10, 30, "False detection of halt, keep going" },  // PT1=3
  { 10, -40, "Recenter with left drift" },           // PT1=4
  { 10, 0, "False detection of halt, keep going" },  // PT1=5
  { 10, -30, "Left turn" },                          // PT1=6
  { 4, 0, "[INFO] Halting state encountered" },      // PT1=7

  // PT2=7: Actions for PT1=0 to PT1=7
  { 6, 180, "Lost track, backup" },               // PT1=0
  { 10, 40, "Recenter forward motion" },          // PT1=1
  { 10, 0, "Now centered" },                     // PT1=2
  { 10, 60, "Missed right turn" },                // PT1=3
  { 10, -40, "Recenter with left drift" },        // PT1=4
  { 10, 30, "Re-centering from a fork" },         // PT1=5
  { 10, -60, "Sharp left turn" },                 // PT1=6
  { 4, 0, "Halting condition?" },                 // PT1=7

};

// Implementation of executeAction
void executeAction(const StateAction* action) {
// Serial.println("executeAction called");
    // g_speed = action->speed * inchderate;
    g_speed = action->speed * inchderate; // Use the speed directly from the action
    g_direction = action->direction;
    g_comment = action->comment; //Update the global comment
    // Call your motor control function here
// Serial.printf(" ***** executeAction called Speed:%d, Direction:%d, Comment:%s\n\r",
//  g_speed, g_direction, g_comment.c_str());

    SetMotor(g_speed, g_direction, g_comment);
    // Serial.printf("SetMotor returned using derate %f, speed %d, direction %d, comment %s \r\n",
    //         inchderate, g_speed, g_direction, g_comment.c_str());
    // Serial.printf("SetMotor returned using speed %d, direction %d, comment %s \r\n",
    //           g_speed, g_direction, g_comment.c_str());

// Serial.println("***** 130 executeAction done, returned");
}

// Helper function for logging
void traceLog(const char* message) {
  if (g_trace) {
    Serial.print("[TRACE] ");
    Serial.println(message);
  }
	}



void processState(int PT1, int PT2) {
  // Validate index range
  if (PT1 < 0 || PT1 >= NUM_PT1_STATES || PT2 < 0 || PT2 >= NUM_PT2_STATES) {
    char errorMsg[100];
    sprintf(errorMsg, "Error: PT1=%d, PT2=%d out of range", PT1, PT2);
    traceLog(errorMsg);
    return;
  }

  // Compute state index and retrieve state action
  int stateIndex = PT2 * NUM_PT1_STATES + PT1;

  const StateAction* action = &g_stateTable[stateIndex];

  // Debug/tracing output
  if (g_trace) {
    char traceMsg[200];
    sprintf(traceMsg,
            "State Processed: PT1=%d, PT2=%d, StateIndex=%d | "
            "Action: Speed=%d, Direction=%d, Comment: %s",
            PT1, PT2, stateIndex,
            action->speed, action->direction, action->comment);
    Serial.println(traceMsg);
  }


  // Execute the action
  executeAction(action);
}



// Initialize function (call this in setup())
void initializeStateMachine(int initialSpeed, int initialDirection) {
	Serial.println("Running initializeStateMachine \n\r");
  g_speed = initialSpeed;
  g_direction = initialDirection;
  SetMotor(g_speed, g_direction, "initial Speed and Direction");
}