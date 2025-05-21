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

// #include <iostream>
#include <array>
#include <functional>
#include <Arduino.h>

// State action structure
typedef struct {
  int speed;
  int direction;
  const char* comment;
} StateAction;


// Constants
#define NUM_PT1_STATES 8
#define NUM_PT2_STATES 8
#define NUM_STATES (NUM_PT1_STATES * NUM_PT2_STATES)

// Global variables (since we're using C)
static int g_speed = 0;
static int g_direction = 0;
static int g_trace = 1;


// Forward declarations
void SetMotor(int speed, int direction);
void executeAction(const StateAction* action);  // Add this forward declaration
void traceLog(const char* message);



// Define the state table as a global constant array
static const StateAction g_stateTable[NUM_STATES] = {

  // PT2=0: Actions for PT1=0 to PT1=7
  { 12, 45, "Correcting slight right drift" },    // PT1=0
  { 12, 45, "Steady forward motion" },            // PT1=1
  { 12, 30, "Correcting left drift" },            // PT1=2
  { 12, 30, "Missed right turn" },                // PT1=3
  { 12, -60, "Hard correction for left drift" },  // PT1=4
  { 12, 30, "Re-centering from a fork" },         // PT1=5
  { 12, 60, "Sharp right turn" },                 // PT1=6
  { 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=1: Actions for PT1=0 to PT1=7
  { 12, 45, "Correcting slight right drift" },    // PT1=0
  { 12, 45, "Steady forward motion" },            // PT1=1
  { 12, 30, "Correcting left drift" },            // PT1=2
  { 12, 30, "Missed right turn" },                // PT1=3
  { 12, -60, "Hard correction for left drift" },  // PT1=4
  { 12, 30, "Re-centering from a fork" },         // PT1=5
  { 12, 60, "Sharp right turn" },                 // PT1=6
  { 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=2: Actions for PT1=0 to PT1=7
  { 12, 45, "Correcting slight right drift" },    // PT1=0
  { 12, 45, "Steady forward motion" },            // PT1=1
  { 12, 30, "Correcting left drift" },            // PT1=2
  { 12, 30, "Missed right turn" },                // PT1=3
  { 12, -60, "Hard correction for left drift" },  // PT1=4
  { 12, 30, "Re-centering from a fork" },         // PT1=5
  { 12, 60, "Sharp right turn" },                 // PT1=6
  { 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=3: Actions for PT1=0 to PT1=7
  { 12, 45, "Correcting slight right drift" },    // PT1=0
  { 12, 45, "Steady forward motion" },            // PT1=1
  { 12, 30, "Correcting left drift" },            // PT1=2
  { 12, 30, "Missed right turn" },                // PT1=3
  { 12, -60, "Hard correction for left drift" },  // PT1=4
  { 12, 30, "Re-centering from a fork" },         // PT1=5
  { 12, 60, "Sharp right turn" },                 // PT1=6
  { 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=4: Actions for PT1=0 to PT1=7
  { 12, 45, "Correcting slight right drift" },    // PT1=0
  { 12, 45, "Steady forward motion" },            // PT1=1
  { 12, 30, "Correcting left drift" },            // PT1=2
  { 12, 30, "Missed right turn" },                // PT1=3
  { 12, -60, "Hard correction for left drift" },  // PT1=4
  { 12, 30, "Re-centering from a fork" },         // PT1=5
  { 12, 60, "Sharp right turn" },                 // PT1=6
  { 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=5: Actions for PT1=0 to PT1=7
  { 12, 45, "Correcting slight right drift" },    // PT1=0
  { 12, 45, "Steady forward motion" },            // PT1=1
  { 12, 30, "Correcting left drift" },            // PT1=2
  { 12, 30, "Missed right turn" },                // PT1=3
  { 12, -60, "Hard correction for left drift" },  // PT1=4
  { 12, 30, "Re-centering from a fork" },         // PT1=5
  { 12, 60, "Sharp right turn" },                 // PT1=6
  { 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=6: Special halt detection states. PT1=0..6 are false detections, PT1=7 is the actual stop.
  { 12, 0, "False detection of halt, keep going" },  // PT1=0
  { 12, 0, "False detection of halt, keep going" },  // PT1=1
  { 12, 0, "False detection of halt, keep going" },  // PT1=2
  { 12, 0, "False detection of halt, keep going" },  // PT1=3
  { 12, 0, "False detection of halt, keep going" },  // PT1=4
  { 12, 0, "False detection of halt, keep going" },  // PT1=5
  { 12, 0, "False detection of halt, keep going" },  // PT1=6
  { 0, 0, "[INFO] Halting state encountered" },       // PT1=7

  // PT2=7: Actions for PT1=0 to PT1=7
  { 12, 45, "Correcting slight right drift" },    // PT1=0
  { 12, 45, "Steady forward motion" },            // PT1=1
  { 12, 30, "Correcting left drift" },            // PT1=2
  { 12, 30, "Missed right turn" },                // PT1=3
  { 12, -60, "Hard correction for left drift" },  // PT1=4
  { 12, 30, "Re-centering from a fork" },         // PT1=5
  { 12, 60, "Sharp right turn" },                 // PT1=6
  { 8, 0, "Halting condition?" },                 // PT1=7

};

// Implementation of executeAction
void executeAction(const StateAction* action) {
    g_speed = action->speed;
    g_direction = action->direction;
    // Call your motor control function here
    SetMotor(g_speed, g_direction);
}


// Helper function for logging
void traceLog(const char* message) {
  if (g_trace) {
    Serial.print("[TRACE] ");
    Serial.println(message);
  }
}


// Main FSM process logic
void process(int PT1, int PT2, int PT3) {
  // Handle halting logic
  if (PT1 == 7 && PT2 == 7 && PT3 == 7) {
    traceLog("Halting state detected: PT1=7, PT2=7, PT3=7.");
    return;  // Stop processing
  }

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
  g_speed = initialSpeed;
  g_direction = initialDirection;
  SetMotor(g_speed, g_direction);
}
