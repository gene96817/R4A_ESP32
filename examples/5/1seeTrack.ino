//This file contains code to read the three IR sensors and
// saves the latest six readings in a circular buffer.

// The two latest states are used to determine direction of travel.
// The last 3 to 6 states are used to determine if the robot encounters the stoping condition.



// Function declaration for external sensor reading
static constexpr int NUM_HISTORY = 6;  // Number of historical states to track (PT1 - PT6)
static constexpr int NUM_READINGS = 5; // Depth of history (last 5 readings for each state)
                                       // NUM_READINGS is to help with debugging.
                                       // Eventually this will be eliminated.

// int PT0 = 0; Initial state for PT0, not used in the current logic
int PT1 =0, PT2 =0, PT3=0, PT4 =0, PT5 =0, PT6 =0; // External declarations for PT1 to PT5

extern int PW0, PW1; //  Previous reading for PT1, not used in the current logic

void GetLineSensors()
{ 
    // Read the line sensors
   pcf8574.read(&lineSensors);
    lineSensors &= 7;
   Serial.printf("Sensors: %d %d %d\n",
          lineSensors & 1,
          (lineSensors & 2) ? 1 : 0,
          (lineSensors & 4) ? 1 : 0);
// Multi-state history array: rows for each PT state, columns for history depth
}

// using a circular buffer
static constexpr int HISTORY_SIZE = 6;
static constexpr int ERROR_VALUE = -1;
#define gLF_DEBUG_STATES        0      // Set to 1 to enable debug output for sensor states
// Global variables
static int stateHistory[HISTORY_SIZE] = {0,0,0,0,0,0}; // Circular buffer for state history
static int writeIndex = 0;      // Head of the circular buffer, the most recent value is written here

// Add new value
void pushHistory(int value) {
Serial.printf("***** pushHistory called with value: %d\r\n", value);
    stateHistory[writeIndex] = value;
    writeIndex = (writeIndex + 1) % HISTORY_SIZE;  // Increment write index and wrap around if necessary
}

// Get historical value
int getHistory(int positionsAgo) {
Serial.printf("***** getHistory called with positionsAgo: %d\r\n", positionsAgo);
 if ((positionsAgo >= HISTORY_SIZE) || positionsAgo < 1) {
    return ERROR_VALUE; // Error case
    }
int readIndex = (writeIndex - positionsAgo - 1 + HISTORY_SIZE) % HISTORY_SIZE;

return stateHistory[readIndex];
}

// Modified seeTrack function
int seeTrack() {
    Serial. printf("***** seeTrack called.\r\n");
    if (pcf8574.read(&lineSensors) == false) {
        Serial.println("Error reading sensors");     // report error if read fails
        return -ERROR_VALUE;                                   // should this be "return true"?
    }
Serial.printf("***** lineSensors before read: %d\r\n", lineSensors);  //
    PT1 = lineSensors & 0x07;       // Mask lower 3 bits (there are only 3 sensors)

    pushHistory(PT1);

   // Get history values if available
    for(int i = 1; i <= 5; i++) {
        int value = getHistory(i);
        if(value == ERROR_VALUE) {
            // Handle error case if needed
            continue;
        }
        switch(i) {
            case 1: PT2 = value; break;
            case 2: PT3 = value; break;
            case 3: PT4 = value; break;
            case 4: PT5 = value; break;
            case 5: PT6 = value; break;
        }
    }

            if (gLF_DEBUG_STATES) {
                 showsensor();
                }
    return PT1;
}


void showsensor() {         // display the last six sensor readings
        Serial.print("Sensor Values: ");
        Serial.print(PT1); Serial.print(", ");
        Serial.print(PT2); Serial.print(", ");
        Serial.print(PT3); Serial.print(", ");
        Serial.print(PT4); Serial.print(", ");
        Serial.print(PT5); Serial.print(", ");
        Serial.println(PT6); Serial.println();
}

