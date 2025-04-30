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
// #include <SetMotor.ino>

// #include "Genes_Basic_Line_Following.h"

class LineFollowingService {
private:
  static constexpr size_t NUM_STATES = 64;  // Total states

public:
  static constexpr size_t NUM_PT1_STATES = 8;  // Number of PT1 sensor states
  static constexpr size_t NUM_PT2_STATES = 8;  // Number of PT2 sensor states
  int speed = 0;
  int direction = 0;

  // Constructor for the LineFollowingService class
  LineFollowingService(int initSpeed, int initDirection)
      : speed(initSpeed), direction(initDirection) {
      // Initialize the motor with speed and direction
      SetMotor motor(speed, direction);
      motor.executeMotorMove(); // Move the motors
  }

  // Define the cell in the state table
struct StateAction {    // Action in each state table cell
  int speed;            // Motor speed
  int direction;        // Steering direction
  const char* comment;  // Description of why action was taken
};

  // Helper method for executing state actions
  void executeAction(const StateAction& action) {
    // Update the speed and direction
    speed = action.speed;
    direction = action.direction;

    // Create a motor object and execute the motor move
    SetMotor motor(speed, direction);
    motor.executeMotorMove();
  }

  // Helper method for logging trace messages
  void traceLog(const std::string& message) {
    if (trace) {
      std::cout << "[TRACE] " << message << "\n";
    }
  }



// Complete the 64-state table
std::array<StateAction, NUM_STATES> stateTable = {
  // PT2=0: Actions for PT1=0 to PT1=7
  StateAction{ 12, 45, "Correcting slight right drift" },    // PT1=0
  StateAction{ 12, 45, "Steady forward motion" },            // PT1=1
  StateAction{ 12, 30, "Correcting left drift" },            // PT1=2
  StateAction{ 12, 30, "Missed right turn" },                // PT1=3
  StateAction{ 12, -60, "Hard correction for left drift" },  // PT1=4
  StateAction{ 12, 30, "Re-centering from a fork" },         // PT1=5
  StateAction{ 12, 60, "Sharp right turn" },                 // PT1=6
  StateAction{ 8, 0, "Halting condition?" },                 // PT1=7

  // Add all other PT2 states here (PT2=1 to PT2=7)...
  // PT2=1: Actions for PT1=0 to PT1=7
  StateAction{ 12, 45, "Correcting slight right drift" },    // PT1=0
  StateAction{ 12, 45, "Steady forward motion" },            // PT1=1
  StateAction{ 12, 30, "Correcting left drift" },            // PT1=2
  StateAction{ 12, 30, "Missed right turn" },                // PT1=3
  StateAction{ 12, -60, "Hard correction for left drift" },  // PT1=4
  StateAction{ 12, 30, "Re-centering from a fork" },         // PT1=5
  StateAction{ 12, 60, "Sharp right turn" },                 // PT1=6
  StateAction{ 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=2: Actions for PT1=0 to PT1=7
  StateAction{ 12, 45, "Correcting slight right drift" },    // PT1=0
  StateAction{ 12, 45, "Steady forward motion" },            // PT1=1
  StateAction{ 12, 30, "Correcting left drift" },            // PT1=2
  StateAction{ 12, 30, "Missed right turn" },                // PT1=3
  StateAction{ 12, -60, "Hard correction for left drift" },  // PT1=4
  StateAction{ 12, 30, "Re-centering from a fork" },         // PT1=5
  StateAction{ 12, 60, "Sharp right turn" },                 // PT1=6
  StateAction{ 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=3: Actions for PT1=0 to PT1=7
  StateAction{ 12, 45, "Correcting slight right drift" },    // PT1=0
  StateAction{ 12, 45, "Steady forward motion" },            // PT1=1
  StateAction{ 12, 30, "Correcting left drift" },            // PT1=2
  StateAction{ 12, 30, "Missed right turn" },                // PT1=3
  StateAction{ 12, -60, "Hard correction for left drift" },  // PT1=4
  StateAction{ 12, 30, "Re-centering from a fork" },         // PT1=5
  StateAction{ 12, 60, "Sharp right turn" },                 // PT1=6
  StateAction{ 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=4: Actions for PT1=0 to PT1=7
  StateAction{ 12, 45, "Correcting slight right drift" },    // PT1=0
  StateAction{ 12, 45, "Steady forward motion" },            // PT1=1
  StateAction{ 12, 30, "Correcting left drift" },            // PT1=2
  StateAction{ 12, 30, "Missed right turn" },                // PT1=3
  StateAction{ 12, -60, "Hard correction for left drift" },  // PT1=4
  StateAction{ 12, 30, "Re-centering from a fork" },         // PT1=5
  StateAction{ 12, 60, "Sharp right turn" },                 // PT1=6
  StateAction{ 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=5: Actions for PT1=0 to PT1=7
  StateAction{ 12, 45, "Correcting slight right drift" },    // PT1=0
  StateAction{ 12, 45, "Steady forward motion" },            // PT1=1
  StateAction{ 12, 30, "Correcting left drift" },            // PT1=2
  StateAction{ 12, 30, "Missed right turn" },                // PT1=3
  StateAction{ 12, -60, "Hard correction for left drift" },  // PT1=4
  StateAction{ 12, 30, "Re-centering from a fork" },         // PT1=5
  StateAction{ 12, 60, "Sharp right turn" },                 // PT1=6
  StateAction{ 8, 0, "Halting condition?" },                 // PT1=7

  // PT2=6: Special halt detection states. PT1=0..6 are false detections, PT1=7 is the actual stop.
  StateAction{ 12, 0, "False detection of halt, keep going" },  // PT1=0
  StateAction{ 12, 0, "False detection of halt, keep going" },  // PT1=1
  StateAction{ 12, 0, "False detection of halt, keep going" },  // PT1=2
  StateAction{ 12, 0, "False detection of halt, keep going" },  // PT1=3
  StateAction{ 12, 0, "False detection of halt, keep going" },  // PT1=4
  StateAction{ 12, 0, "False detection of halt, keep going" },  // PT1=5
  StateAction{ 12, 0, "False detection of halt, keep going" },  // PT1=6
  StateAction{ 0, 0, "[INFO] Halting state encountered" },       // PT1=7

  // PT2=7: Actions for PT1=0 to PT1=7
  StateAction{ 12, 45, "Correcting slight right drift" },    // PT1=0
  StateAction{ 12, 45, "Steady forward motion" },            // PT1=1
  StateAction{ 12, 30, "Correcting left drift" },            // PT1=2
  StateAction{ 12, 30, "Missed right turn" },                // PT1=3
  StateAction{ 12, -60, "Hard correction for left drift" },  // PT1=4
  StateAction{ 12, 30, "Re-centering from a fork" },         // PT1=5
  StateAction{ 12, 60, "Sharp right turn" },                 // PT1=6
  StateAction{ 8, 0, "Halting condition?" },                 // PT1=7

};

// Main FSM process logic
void process(int PT1, int PT2, int PT3) {
  // Handle halting logic
  if (PT1 == 7 && PT2 == 7 && PT3 == 7) {
    traceLog("Halting state detected: PT1=7, PT2=7, PT3=7.");
    return;  // Stop processing
  }

  // Validate index range
  if (PT1 < 0 || PT1 >= NUM_PT1_STATES || PT2 < 0 || PT2 >= NUM_PT2_STATES) {
    traceLog("Error: PT1=" + std::to_string(PT1) + ", PT2=" + std::to_string(PT2) + " out of range. Valid values: PT1 [0-" + std::to_string(NUM_PT1_STATES - 1) + "], PT2 [0-" + std::to_string(NUM_PT2_STATES - 1) + "].");
    return;
  }

  // Compute state index and retrieve state action
  int stateIndex = PT2 * NUM_PT1_STATES + PT1;
  StateAction action = stateTable[stateIndex];

  // Debug/tracing output
  if (trace) {
    Serial.print("[TRACE] State Processed: ")
      Serial.print("PT1= ")
        Serial.print(PT1);
            Serial.print (", PT2= ")
            Serial.print(PT2)
           Serial.print(", StateIndex= ")
           Serial.print(stateIndex)
           Serial.print(" | Action: Speed= "
           Serial.print (action.speed)
              Serial.print( "Direction= ")
              Serial.print(action.direction)
              Serial.print(" Comment: ")
                Serial.println(action.comment);
  }

  // Execute the action
  executeAction(action);
}
}
;