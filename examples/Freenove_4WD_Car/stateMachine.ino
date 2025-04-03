// Line Following Robot - Inchworm state machine
// The Challenge code calls this service to get a reading from the sensors
// and to compute the next move for  the next move.
// Created by Eugene Chang on 3/7/25.
//

#include <iostream>
#include <array>
#include <functional>
// #include "Genes_Basic_Line_Following.h"

// External function declarations
void MOVE(int speed, int direction);
// void showsensor();

class LineFollowingService {

private:
    static constexpr int NUM_PT1_STATES = 8;  // Number of PT1 sensor states
    static constexpr int NUM_PT2_STATES = 8;  // Number of PT2 sensor states
    static constexpr int NUM_STATES = NUM_PT1_STATES * NUM_PT2_STATES;  // Total states

 public:
     LineFollowingService();  // look up stateAction and build call to MOVE
        // Constructor
    }




// PT1: Sensor A readings (0-7) - Reflects position over the centerline
// PT2: Sensor B readings (0-7) - Reflects previous position over the centerline
// PT3: Sensor C readings -- three readings of 7 is the halt condition.
// State Table: Maps [PT2 * NUM_PT1_STATES + PT1] to a specific motor action to the next position.



    // Struct to define state actions
    // State table: [PT2 * NUM_PT1_STATES + PT1] -> {speed, direction, comment}
    struct StateAction {
        int speed;       // Motor speed
        int direction;   // Steering direction
        const char* comment; // Description of the action
    };



    // Complete 64-state table
     // State table: [PT2 * NUM_PT1_STATES + PT1] -> {speed, direction, comment}
    std::array<StateAction, NUM_STATES> stateTable = {
        // PT2=0: Actions for PT1=0 to PT1=7
        StateAction{12,  45, "Correcting slight right drift"}, // PT1=0
        StateAction{12,  45, "Steady forward motion"},         // PT1=1
        StateAction{12,  30, "Correcting left drift"},         // PT1=2
        StateAction{12,  30, "Missed right turn"},             // PT1=3
        StateAction{12, -60, "Hard correction for left drift"}, // PT1=4
        StateAction{12,  30, "Re-centering from a fork"},      // PT1=5
        StateAction{12,  60, "Sharp right turn"},              // PT1=6
        StateAction{8,  0, "Halting condition?"},              // PT1=7

        // PT2=1: Actions for PT1=0 to PT1=7
        StateAction{12,  30, "Gradual left correction"},       // PT1=0
        StateAction{12,  45, "Steady forward motion"},         // PT1=1
        StateAction{12,  30, "Correcting left"},               // PT1=2
        StateAction{12,  15, "Adjusting to stay centered"},    // PT1=3
        StateAction{12, -15, "Light correction for left drift"}, // PT1=4
        StateAction{12,  30, "Minor re-adjustment to right"},  // PT1=5
        StateAction{12,  45, "Heading rightward with caution"}, // PT1=6
        StateAction{8,  0, "Halting condition?"},              // PT1=7

        // PT2=2: Actions for PT1=0 to PT1=7
        StateAction{12, -30, "Gradual left turn"},             // PT1=0
        StateAction{12,  45, "On-target forward motion"},      // PT1=1
        StateAction{12,  30, "Drifting slightly to the left"}, // PT1=2
        StateAction{12,  15, "Course correction"},             // PT1=3
        StateAction{12, -30, "Heavy leftward course correction"}, // PT1=4
        StateAction{12,  30, "Centering"},                     // PT1=5
        StateAction{12,  45, "Minor turn to right"},           // PT1=6
        StateAction{8,  0, "Halting condition?"},              // PT1=7

        // PT2=3: Actions for PT1=0 to PT1=7
        StateAction{12, -60, "Hard left adjustment"},          // PT1=0
        StateAction{12,  15, "Forward slowing slightly"},      // PT1=1
        StateAction{12,  30, "Correcting direction"},          // PT1=2
        StateAction{12,   0, "Staying straight"},              // PT1=3
        StateAction{12, -15, "Slight correction, left drift"}, // PT1=4
        StateAction{12,  30, "Rightward centering"},           // PT1=5
        StateAction{12, -45, "Larger left turn correction"},   // PT1=6
        StateAction{8,  0, "Halting condition?"},              // PT1=7

        // PT2=4: Actions for PT1=0 to PT1=7
        StateAction{12, -30, "Gentle leftward adjustment"},    // PT1=0
        StateAction{12, -15, "Minor left drift correction"},   // PT1=1
        StateAction{12, -45, "Moderate left turn"},            // PT1=2
        StateAction{12,  10, "Re-aligning straight"},          // PT1=3
        StateAction{12,  15, "Fine tuning to the right"},      // PT1=4
        StateAction{12,  30, "Centered movement"},             // PT1=5
        StateAction{12, -60, "Hard left to correct positioning"}, // PT1=6
        StateAction{8,  0, "Halting condition?"},              // PT1=7

        // PT2=5: Actions for PT1=0 to PT1=7
        StateAction{12,  15, "Small right drift correction"},  // PT1=0
        StateAction{12,  30, "Aligning forward"},              // PT1=1
        StateAction{12, -15, "Drifting left slightly"},        // PT1=2
        StateAction{12, -30, "Turning leftward slightly"},     // PT1=3
        StateAction{12, -60, "Major correction left"},         // PT1=4
        StateAction{12, -45, "Moderate course correction left"}, // PT1=5
        StateAction{12, -15, "Light correction left drift"},   // PT1=6
        StateAction{8,  0, "Halting condition?"},              // PT1=7

        // PT2=6: Actions for PT1=0 to PT1=7
        StateAction{12,  45, "Onward with small right adjustment"}, // PT1=0
        StateAction{12,  30, "Tracking forward"},              // PT1=1
        StateAction{12, -15, "Slight left adjustment"},        // PT1=2
        StateAction{12,  15, "Re-centering the path"},         // PT1=3
        StateAction{12, -60, "Correcting a large left drift"}, // PT1=4
        StateAction{12, -45, "Moderate left correction"},      // PT1=5
        StateAction{12,  60, "Hard right adjustment"},         // PT1=6
        StateAction{8,  0, "Halting condition?"},              // PT1=7

        // PT2=7: Special halt detection states. PT1=0..6: False detections, PT1=7: Actual stop.
        StateAction{12,   0, "False detection of halt, keep going"},       // PT1=0
        StateAction{12,   0, "False detection of halt, keep going"},        // PT1=1
        StateAction{12,   0, "False detection of halt, keep going"},       // PT1=2
        StateAction{12,   0, "False detection of halt, keep going"},       // PT1=3
        StateAction{12,   0, "False detection of halt, keep going"},       // PT1=4
        StateAction{12,   0, "False detection of halt, keep going"},       // PT1=5
        StateAction{12,   0, "False detection of halt, keep going"},       // PT1=6
        StateAction{0,    0, "[INFO] Halting state encountered"},         // PT1=7
    };


    // Helper method for executing state actions
    void executeAction(const StateAction& action) {
        if (trace) {
            std::cout << "[TRACE] Executing action: Speed=" << action.speed
                      << ", Direction=" << action.direction
                      << " | Comment: " << action.comment << "\n";
        }
        MOVE(action.speed, action.direction);
    }

    // Helper method for logging trace messages
    void traceLog(const std::string& message) {
        if (trace) {
            std::cout << "[TRACE] " << message << "\n";
        }
    }

public:
    int currentState = 0; // Current state index (0 by default)

    // WHAT UPDATRES currentState

    // Main FSM Process Logic
    void process(int PT1, int PT2, int PT3) {

    // Handle halting logic
        if (PT1 == 7 && PT2 == 7 && PT3 == 7) {
            traceLog("Halting state detected: PT1=7, PT2=7, PT3=7.");
            return;  //stop proceswsing
        }

        // Validate index range
        if (PT1 < 0 || PT1 >= NUM_PT1_STATES || PT2 < 0 || PT2 >= NUM_PT2_STATES) {
            traceLog("Error: PT1=" + std::to_string(PT1) + ", PT2=" + std::to_string(PT2) +
                      " out of range. Valid values: PT1 [0-" + std::to_string(NUM_PT1_STATES - 1) +
                      "], PT2 [0-" + std::to_string(NUM_PT2_STATES - 1) + "].");
            return;
        return;
    }

    // Compute state index and retrieve state action
    int stateIndex = PT2 * NUM_PT1_STATES + PT1;
    StateAction action = stateTable[stateIndex];


    // Execute action
    executeAction(action);
    traceLog("State Processed: PT1=" + std::to_string(PT1) +
         ", PT2=" + std::to_string(PT2) +
         ", StateIndex=" + std::to_string(stateIndex) +
         ", Speed=" + std::to_string(action.speed) +
         ", Direction=" + std::to_string(action.direction) +
         " | Comment: " + action.comment);


    // Debug/tracing output
    if (trace) {
        std::cout << "[TRACE] State Processed: "
          << "PT1=" << PT1
          << ", PT2=" << PT2
          << ", StateIndex=" << stateIndex
          << ", Speed=" << action.speed
          << ", Direction=" << action.direction
          << " | Comment: " << action.comment
          << "\n";
    }
}
