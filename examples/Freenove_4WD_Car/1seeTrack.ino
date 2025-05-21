

// Function declaration for external sensor reading

static constexpr int NUM_HISTORY = 6;  // Number of historical states to track (PT1 - PT6)
static constexpr int NUM_READINGS = 5; // Depth of history (last 5 readings for each state)

// int PT0 = 0; Initial state for PT0, not used in the current logic
int PT1 =0, PT2 =0, PT3=0, PT4 =0, PT5 =0, PT6 =0; // External declarations for PT1 to PT5

extern int PW0, PW1; //  Previous reading for PT1, not used in the current logic
extern bool glf_DEBUG_STATES; // Debug flag for state tracking
extern uint8_t lineSensors; // Line sensor reading variable

// Multi-state history array: rows for each PT state, columns for history depth

// useing a circular buffer
#define HISTORY_SIZE 6

// Global variables
static int stateHistory[HISTORY_SIZE] = {6};
static int writeIndex = 0;

// Add new value
void pushHistory(int value) {
    stateHistory[writeIndex] = value;
    writeIndex = (writeIndex + 1) % HISTORY_SIZE;
}

// Get historical value
int getHistory(int positionsAgo) {
    if (positionsAgo >= HISTORY_SIZE) {
        return -1;  // Error case
    }
    int readIndex = (writeIndex - 1 - positionsAgo + HISTORY_SIZE) % HISTORY_SIZE;
    return stateHistory[readIndex];
}

// Modified seeTrack function with simplified buffer
int seeTrack() {
    uint8_t lineSensors;

    if (pcf8574.read(&lineSensors) != 0) {
        Serial.println("Error reading sensors");
        return -1;
    }

    PT1 = lineSensors & 7;
    pushHistory(PT1);

    PT2 = getHistory(1);
    PT3 = getHistory(2);
    PT4 = getHistory(3);
    PT5 = getHistory(4);
    PT6 = getHistory(5);


    if (glf_DEBUG_STATES) {
        showsensor();
    }

    return PT1;
}


void showsensor() {
        Serial.print("Sensor Values: ");
        Serial.print(PT1); Serial.print(", ");
        Serial.print(PT2); Serial.print(", ");
        Serial.print(PT3); Serial.print(", ");
        Serial.print(PT4); Serial.print(", ");
        Serial.print(PT5); Serial.print(", ");
        Serial.println(PT6); Serial.println();
}
;
