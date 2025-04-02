#include <iostream>
#include <iomanip>
#include "PCF8574.h"

// Function declaration for external sensor reading

static constexpr int NUM_HISTORY = 6;  // Number of historical states to track (PT1 - PT6)
static constexpr int NUM_READINGS = 5; // Depth of history (last 5 readings for each state)

int PT1 = 0;
int PT2 = 0;
int PT3 = 0;
int PT4 = 0;
int PT5 = 0;



// Multi-state history array: rows for each PT state, columns for history depth
int stateHistory[NUM_HISTORY][NUM_READINGS] = {0};

// Updated seeTrack function
int seeTrack(int& PT2, int& PT3, int& PT4, int& PT5, int& PT6) {
    // Read the line sensors to get current PT1
    uint8_t lineSensors;
    if (pcf8574.read(&lineSensors) != 0) {
        std::cerr << "Error reading sensors" << std::endl;
        return -1; // Return an error code
    }
    int PT1 = lineSensors & 7; // Mask to 3 bits (0-7 range for PT1)

    // Update states from oldest to newest
    PT6 = stateHistory[4][0];  // PT6 takes the previous PT5
    PT5 = stateHistory[3][0];  // PT5 takes the previous PT4
    PT4 = stateHistory[2][0];  // PT4 takes the previous PT3
    PT3 = stateHistory[1][0];  // PT3 takes the previous PT2
    PT2 = stateHistory[0][0];  // PT2 takes the previous PT1 (most recent reading)

    // Update history for all states
    // StateHistory(stateHistory, NUM_READINGS, PT1, PT2, PT3, PT4, PT5, PT6);

    // Debugging output
    if (BLF_DEBUG_STATES) {
        std::cout << "Sensor Values (PT1-PT6, Current Reading -> History): " << std::endl;
        for (int state = 0; state < NUM_HISTORY; ++state) {
            std::cout << "PT" << state + 1 << ": ";
            for (int reading = 0; reading < NUM_READINGS; ++reading) {
                std::cout << stateHistory[state][reading] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Current PT1: " << PT1 << ", PT2: " << PT2
                  << ", PT3: " << PT3 << ", PT4: " << PT4 << ", PT5: " << PT5
                  << ", PT6: " << PT6 << "\n";
    }

    return PT1;  // Return PT1 for the FSM to process
}

void showsensor() {
    // Assuming PT1, PT2, and PT3 are global variables or accessible in this scope
    Serial.print("PT1: ");
    Serial.print(stateHistory[0][0]); // Display the value of PT1
    Serial.print(", PT2: ");
    Serial.print(stateHistory[1][0]); // Display the value of PT2
    Serial.print(", PT3: ");
    Serial.println(stateHistory[2][0]); // Display the value of PT3 and move to the next line
}

