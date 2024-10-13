/**********************************************************************
  Menu.ino

  Menu routines
**********************************************************************/

//*********************************************************************
// Display data before the main menu header
void mainMenuPre(Print * display)
{
    if (wifiApCount)
        r4aNtpDisplayDateTime(display);
    DISPLAY_BATTERY_VOLTAGE(display);
}

//*********************************************************************
// Toggle the backup lights
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void ledMenuBackup(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    car.backupLightsToggle();
}

//*********************************************************************
// Toggle the brake lights
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void ledMenuBrake(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    car.brakeLightsToggle();
}

//*********************************************************************
// Toggle the headlights
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void ledMenuHeadlights(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    car.headlightsToggle();
}

//*********************************************************************
// Turn off all the LEDs
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void ledMenuOff(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    car.backupLightsOff();
    car.brakeLightsOff();
    car.headlightsOff();
    car.ledsTurnOff();
    r4aLEDsOff();
}

//*********************************************************************
// Turn left indicator
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void ledMenuTurnLeft(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    car.ledsTurnLeft();
}


//*********************************************************************
// Stop the turn signal blinking
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void ledMenuTurnOff(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    car.ledsTurnOff();
}

//*********************************************************************
// Turn right indicator
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void ledMenuTurnRight(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    car.ledsTurnRight();
}

//*********************************************************************
// Stop the robot
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void robotMenuStop(const R4A_MENU_ENTRY * menuEntry,
                   const char * command,
                   Print * display)
{
    robot.stop(millis(), display);
}

//*********************************************************************

enum MENU_TABLE_INDEX
{
    MTI_DEBUG = R4A_MENU_MAIN + 1,
    MTI_I2C,
    MTI_LED,
    MTI_MOTOR,
    MTI_NVM,
    MTI_SERVO,
};

// Debug menu
const R4A_MENU_ENTRY debugMenuTable[] =
{
    // Command  menuRoutine                 menuParam       HelpRoutine align   HelpText
    {"h",       r4aEsp32MenuDisplayHeap,    0,              nullptr,    0,      "Display the heap"},
    {"i",       nullptr,                    MTI_I2C,        nullptr,    0,      "I2C menu"},
    {"l",       nullptr,                    MTI_LED,        nullptr,    0,      "LED menu"},
    {"m",       nullptr,                    MTI_MOTOR,      nullptr,    0,      "Motor menu"},
    {"p",    r4aEsp32MenuDisplayPartitions, 0,              nullptr,    0,      "Display the partitions"},
    {"s",       nullptr,                    MTI_SERVO,      nullptr,    0,      "Servo menu"},
    {"x",       nullptr,                    R4A_MENU_MAIN,  nullptr,    0,      "Return to the main menu"},
};
#define DEBUG_MENU_ENTRIES      sizeof(debugMenuTable) / sizeof(debugMenuTable[0])

// LED menu
const R4A_MENU_ENTRY ledMenuTable[] =
{
    // Command  menuRoutine         menuParam   HelpRoutine     align   HelpText
    {"b",       ledMenuBackup,      0,          nullptr,        0,      "Toggle backup lights"},                             // 0
    {"c3",      r4aLEDMenuColor3,   0,  r4aLEDMenuHelpllcccc,   12,      "Specify the LED ll color cccccc (RGB in hex)"},    // 1
    {"c4",      r4aLEDMenuColor4,   0,  r4aLEDMenuHelpllcccc,   12,      "Specify the LED ll color cccccccc (RGBW in hex)"}, // 2
    {"d",       r4aLEDMenuDisplay,  0,          nullptr,        0,      "Display the LED status"},
    {"h",       ledMenuHeadlights,  0,          nullptr,        0,      "Toggle headlights"},                                // 3
    {"i",      r4aLEDMenuIntensity, 0,       r4aLEDMenuHelpiii, 4,      "Specify the LED intensity iii (0 - 255)"},          // 4
    {"l",       ledMenuTurnLeft,    0,          nullptr,        0,      "Turn left"},                                        // 5
    {"o",       ledMenuTurnOff,     0,          nullptr,        0,      "Stop the turn signal blinking"},                    // 6
    {"off",     ledMenuOff,         0,          nullptr,        0,      "Turn off all the LEDs"},                            // 7
    {"r",       ledMenuTurnRight,   0,          nullptr,        0,      "Turn right"},                                       // 8
    {"s",       ledMenuBrake,       0,          nullptr,        0,      "Toggle brake lights"},                              // 9
    {"x",       nullptr,         R4A_MENU_MAIN, nullptr,        0,      "Return to the main menu"},                          // 10
};                                                                                                                           // 11
#define LED_MENU_ENTRIES      sizeof(ledMenuTable) / sizeof(ledMenuTable[0])

// Main menu
const R4A_MENU_ENTRY mainMenuTable[] =
{
    // Command  menuRoutine         menuParam       HelpRoutine align   HelpText
    {"blf",     blfStart,           0,              nullptr,    0,      "Basic line following"},
    {"blt",     bltStart,           0,              nullptr,    0,      "Basic light tracking"},
    {"c", r4aMenuBoolToggle, (intptr_t)&ov2640Enable, r4aMenuBoolHelp, 0, "Toggle OV2640 camera"},
    {"d",       nullptr,            MTI_DEBUG,      nullptr,    0,      "Enter the debug menu"},
    {"i",  r4aMenuBoolToggle, (intptr_t)&ignoreBatteryCheck, r4aMenuBoolHelp, 0, "Ignore the battery check"},
    {"nvm",     nullptr,            MTI_NVM,        nullptr,    0,      "Enter the NVM menu"},
    {"r",  r4aEsp32MenuSystemReset, 0,              nullptr,    0,      "System reset"},
    {"s",       robotMenuStop,      0,              nullptr,    0,      "Stop the robot"},
    {"w", r4aMenuBoolToggle, (intptr_t)&webServerEnable, r4aMenuBoolHelp, 0, "Toggle web server"},
    {"x",       nullptr,            R4A_MENU_NONE,  nullptr,    0,      "Exit the menu system"},
};
#define MAIN_MENU_ENTRIES       sizeof(mainMenuTable) / sizeof(mainMenuTable[0])

const R4A_MENU_TABLE menuTable[] =
{
    // menuName         preMenu routine firstEntry          entryCount
    {"Main Menu",       mainMenuPre,    mainMenuTable,      MAIN_MENU_ENTRIES},
    {"Debug Menu",      nullptr,        debugMenuTable,     DEBUG_MENU_ENTRIES},
    {"I2C Menu",        nullptr,        r4aI2cMenuTable,    R4A_I2C_MENU_ENTRIES},
    {"LED Menu",        nullptr,        ledMenuTable,       LED_MENU_ENTRIES},
//    {"LED Menu",        nullptr,        r4aLEDMenuTable,    R4A_LED_MENU_ENTRIES},
    {"Motor Menu",      nullptr,  r4aPca9685MotorMenuTable, R4A_PCA9685_MOTOR_MENU_ENTRIES},
    {"NVM Menu",        nullptr,      r4aEsp32NvmMenuTable, R4A_ESP32_NVM_MENU_ENTRIES},
    {"Servo Menu",      nullptr,  r4aPca9685ServoMenuTable, R4A_PCA9685_SERVO_MENU_ENTRIES},
};
const int menuTableEntries = sizeof(menuTable) / sizeof(menuTable[0]);
