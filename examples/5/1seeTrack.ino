//This file contains code to read the three IR sensors and
// saves the latest six readings in a circular buffer.

// The two latest states are used to determine direction of travel.
// The last 3 to 6 states are used to determine if the robot encounters the stoping condition.


// delete NUM_xxx.... old code from ring buffer
// Function declaration for external sensor reading
static constexpr int NUM_HISTORY = 6;   // Number of historical states to track (PT1 - PT6)
static constexpr int NUM_READINGS = 5;  // Depth of history (last 5 readings for each state)
                                        // NUM_READINGS is to help with debugging.
                                        // Eventually this will be eliminated.
void showHistory();
void pushHistory(int value);
int seeTrack();


int PT0 = 0;                                               // Initial lineSensors reading
int PT1 = 0, PT2 = 0, PT3 = 0, PT4 = 0, PT5 = 0, PT6 = 0;  // External declarations for PT1 to PT5

extern int PW0, PW1;  //  Previous reading for PT1, not used in the current logic

void getLineSensors() {
  // Read the line sensors
  pcf8574.read(&lineSensors);

  //  old code, bits in reverse order
  // lineSensors &= 7;
  //    Serial.printf("Sensors: %d %d %d %d\n",
  //           lineSensors,
  //           (lineSensors & 4) ? 1 : 0,
  //          (lineSensors & 1) ? 1 : 0);
  //


  lineSensors &= 7;  // Keep only the lowest 3 bits

  // Lookup table for 3-bit reversal
  const uint8_t reverse_lookup[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
  lineSensors = reverse_lookup[lineSensors];

/********************
  Serial.printf("Sensors: %d %d %d %d\n",
                lineSensors,
                (lineSensors & 4) ? 1 : 0,
                (lineSensors & 2) ? 1 : 0,
                (lineSensors & 1) ? 1 : 0);
*********************/

  // Multi-state history array: rows for each PT state, columns for history depth
  PT0 = lineSensors;
}

#define gLF_DEBUG_STATES 0  // Set to 1 to enable debug output for sensor states

// Global variables

// Add new value
void pushHistory(int value) {
  // Serial.printf("***** pushHistory called with PT0: %d\r\n", value);
  PT6 = PT5;
  PT5 = PT4;
  PT4 = PT3;
  PT3 = PT2;
  PT2 = PT1;
  PT1 = PT0;
  Serial.printf("Position History PT1: %d, PT2: %d, PT3: %d, PT4: %d, PT5: %d, PT6: %d\r\n",
                PT1, PT2, PT3, PT4, PT5, PT6);
}

// Get historical value
// void showHistory() {
  // Serial.printf("***** showHistory called\r\n");
  // Serial.printf("***** Show History PT1: %d, PT2: %d, PT3: %d, PT4: %d, PT5: %d, PT6: %d\r\n",
  //              PT1, PT2, PT3, PT4, PT5, PT6);
//  };



//  seeTrack function
int seeTrack() {
  //    Serial.printf("***** seeTrack called.\r\n");

  /*****************
    if (pcf8574.read(&lineSensors) == false) {
        Serial.println("Error reading sensors");     // report error if read fails
        return -1;                                   // should this be "return true"?
    };
*************/

  getLineSensors();
  // Serial.printf("***** lineSensors before read: %d\r\n", PT0);  //

  //  pushHistory(PT1);     // pushHistory will print contents of ring buffer
  // showHistory();
  pushHistory(PT0);
  return PT1;
}
