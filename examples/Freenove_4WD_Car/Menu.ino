/**********************************************************************
  Menu.ino

  Menu routines
**********************************************************************/

#ifdef  USE_ZED_F9P
//*********************************************************************
// Display the computed point
// Inputs:
//   parameter: Callback parameter passed to computeWayPoint
//   comment: Text to display at the start of the line
//   latitude: Latitude in degrees
//   longitude: Longitude in degrees
//   altitude: Altitude in meters
//   horizontalAccuracy: Accuracy in meters
//   satellitesInView: The number of satellites feeding the GNSS receiver
//   latitudeStdDev: Latitude standard deviation in degrees
//   longitudeStdDev: Longitude standard deviation in degrees
//   altitudeStdDev: Altitude standard deviation in meters
//   horizontalAccuracyStdDev: Horizontal accuracy standard deviation in meters
//   display: Device used for output, passed to computeWayPoint
void gnssDisplayPoint(intptr_t parameter,
                      const char * comment,
                      double latitude,
                      double latitudeStdDev,
                      double longitude,
                      double longitudeStdDev,
                      double altitude,
                      double altitudeStdDev,
                      double horizontalAccuracy,
                      double horizontalAccuracyStdDev,
                      uint8_t satellitesInView,
                      Print * display)
{
    zedf9p.displayLocation(comment,
                           latitude,
                           0,
                           longitude,
                           0,
                           altitude,
                           0,
                           horizontalAccuracy,
                           0,
                           satellitesInView,
                           true,    // unitsFeetInches,
                           false,   // displayTime,
                           true,    // displaySiv,
                           true,    // displayLatitude,
                           false,   // displayLatStdDev,
                           true,    // displayLongitude,
                           false,   // displayLongStdDev,
                           true,    // displayHorizAcc,
                           false,   // displayHorizAccStdDev,
                           false,   // displayAltitude,
                           false,   // displayAltitudeStdDev,
                           false,   // displayFixType,
                           display);
}

//*********************************************************************
// Compute point and display point
void gnssMenuComputePoint(const R4A_MENU_ENTRY * menuEntry,
                          const char * command,
                          Print * display)
{
    zedf9p.computePoint(gnssDisplayPoint,
                        0,
                        GNSS_POINTS_TO_AVERAGE,
                        "Location",
                        display);
}

//*********************************************************************
// Display the location
void gnssMenuDisplayLocation(const R4A_MENU_ENTRY * menuEntry,
                             const char * command,
                             Print * display)
{
    zedf9p.displayLocation("Location: ",
                           true,    // unitsFeetInches,
                           false,   // displayTime,
                           true,    // displaySiv,
                           true,    // displayLatitude
                           true,    // displayLongitude
                           true,    // displayHorizAcc,
                           false,   // displayAltitude,
                           false,   // displayFixType,
                           display);
}
#endif  // USE_ZED_F9P

//*********************************************************************
// Display data before the main menu header
void mainMenuPre(Print * display)
{
    if (r4aWifiSsidPasswordEntries)
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
// Toggle WiFi debugging
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void wifiMenuDebug(const R4A_MENU_ENTRY * menuEntry,
                   const char * command,
                   Print * display)
{
    r4aMenuBoolToggle(menuEntry, command, display);
    wifi._debug = wifiDebug ? &Serial : nullptr;
}

//*********************************************************************
// Restart the WiFi station
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void wifiMenuRestart(const R4A_MENU_ENTRY * menuEntry,
                     const char * command,
                     Print * display)
{
    wifi.stationStop();
    delay(5 * 1000);
    wifi.stationStart();
}

//*********************************************************************

enum MENU_TABLE_INDEX
{
    MTI_DEBUG = R4A_MENU_MAIN + 1,
#ifdef  USE_ZED_F9P
    MTI_GNSS,
#endif  // USE_ZED_F9P
    MTI_I2C,
    MTI_LED,
    MTI_MOTOR,
#ifdef  USE_NTRIP
    MTI_NTRIP,
#endif  // USE_NTRIP
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

#ifdef  USE_ZED_F9P
// GNSS menu
const R4A_MENU_ENTRY gnssMenuTable[] =
{
    // Command  menuRoutine         menuParam   HelpRoutine     align   HelpText
    {"l",  gnssMenuDisplayLocation, 0,          nullptr,        0,      "Display location"},
    {"p",     gnssMenuComputePoint, 0,          nullptr,        0,      "Compute point and display point"},
    {"x",       nullptr,         R4A_MENU_MAIN, nullptr,        0,      "Return to the main menu"},
};
#define GNSS_MENU_ENTRIES       sizeof(gnssMenuTable) / sizeof(gnssMenuTable[0])
#endif  // USE_ZED_F9P

// LED menu
const R4A_MENU_ENTRY ledMenuTable[] =
{
    // Command  menuRoutine         menuParam               HelpRoutine         align   HelpText
    {"b",       ledMenuBackup,      0,                      nullptr,            0,      "Toggle backup lights"},
    {"c3",      r4aLEDMenuColor3,   (intptr_t)"ll rrggbb",  r4aMenuHelpSuffix,  9,      "Specify the LED ll color rrggbb (RGB in hex)"},
    {"c4",      r4aLEDMenuColor4,  (intptr_t)"ll wwrrggbb", r4aMenuHelpSuffix,  11,     "Specify the LED ll color cccccccc (RGBW in hex)"},
    {"d",       r4aLEDMenuDisplay,  0,                      nullptr,            0,      "Display the LED status"},
    {"h",       ledMenuHeadlights,  0,                      nullptr,            0,      "Toggle headlights"},
    {"i",      r4aLEDMenuIntensity, (intptr_t)"iii",        r4aMenuHelpSuffix,  4,      "Specify the LED intensity iii (0 - 255)"},
    {"l",       ledMenuTurnLeft,    0,                      nullptr,            0,      "Turn left"},
    {"o",       ledMenuTurnOff,     0,                      nullptr,            0,      "Stop the turn signal blinking"},
    {"off",     ledMenuOff,         0,                      nullptr,            0,      "Turn off all the LEDs"},
    {"r",       ledMenuTurnRight,   0,                      nullptr,            0,      "Turn right"},
    {"s",       ledMenuBrake,       0,                      nullptr,            0,      "Toggle brake lights"},
    {"x",       nullptr,            R4A_MENU_MAIN,          nullptr,            0,      "Return to the main menu"},
};
#define LED_MENU_ENTRIES      sizeof(ledMenuTable) / sizeof(ledMenuTable[0])

// Main menu
const R4A_MENU_ENTRY mainMenuTable[] =
{
    // Command  menuRoutine         menuParam       HelpRoutine align   HelpText
    {"blf",     blfStart,           0,              nullptr,    0,      "Basic line following"},
    {"blt",     bltStart,           0,              nullptr,    0,      "Basic light tracking"},
#ifdef  USE_OV2640
    {"c", r4aMenuBoolToggle, (intptr_t)&ov2640Enable, r4aMenuBoolHelp, 0, "Toggle OV2640 camera"},
#endif  // USE_OV2640
    {"d",       nullptr,            MTI_DEBUG,      nullptr,    0,      "Enter the debug menu"},
#ifdef  USE_ZED_F9P
    {"g",       nullptr,            MTI_GNSS,       nullptr,    0,      "Enter the GNSS menu"},
#endif  // USE_ZED_F9P
    {"i",  r4aMenuBoolToggle, (intptr_t)&ignoreBatteryCheck, r4aMenuBoolHelp, 0, "Ignore the battery check"},
#ifdef  USE_NTRIP
    {"NTRIP",   nullptr,            MTI_NTRIP,      nullptr,    0,      "Enter the NTRIP menu"},
#endif  // USE_NTRIP
    {"nvm",     nullptr,            MTI_NVM,        nullptr,    0,      "Enter the NVM menu"},
    {"r",  r4aEsp32MenuSystemReset, 0,              nullptr,    0,      "System reset"},
    {"s",       robotMenuStop,      0,              nullptr,    0,      "Stop the robot"},
    {"w", r4aMenuBoolToggle, (intptr_t)&webServerEnable, r4aMenuBoolHelp, 0, "Toggle web server"},
    {"wd",     wifiMenuDebug, (intptr_t)&wifiDebug, r4aMenuBoolHelp, 0, "Toggle WiFi debugging"},
    {"wr",      wifiMenuRestart,    0,              nullptr,    0,      "Restart WiFi"},
    {"x",       nullptr,            R4A_MENU_NONE,  nullptr,    0,      "Exit the menu system"},
};
#define MAIN_MENU_ENTRIES       sizeof(mainMenuTable) / sizeof(mainMenuTable[0])

const R4A_MENU_TABLE menuTable[] =
{
    // menuName         preMenu routine firstEntry          entryCount
    {"Main Menu",       mainMenuPre,    mainMenuTable,      MAIN_MENU_ENTRIES},
    {"Debug Menu",      nullptr,        debugMenuTable,     DEBUG_MENU_ENTRIES},
#ifdef  USE_ZED_F9P
    {"GNSS Menu",       nullptr,        gnssMenuTable,      GNSS_MENU_ENTRIES},
#endif  // USE_ZED_F9P
    {"I2C Menu",        nullptr,        r4aI2cMenuTable,    R4A_I2C_MENU_ENTRIES},
    {"LED Menu",        nullptr,        ledMenuTable,       LED_MENU_ENTRIES},
//    {"LED Menu",        nullptr,        r4aLEDMenuTable,    R4A_LED_MENU_ENTRIES},
    {"Motor Menu",      nullptr,  r4aPca9685MotorMenuTable, R4A_PCA9685_MOTOR_MENU_ENTRIES},
#ifdef  USE_NTRIP
    {"NTRIP Menu",      nullptr,  r4aNtripClientMenuTable,  R4A_NTRIP_CLIENT_MENU_ENTRIES},
#endif  // USE_NTRIP
    {"NVM Menu",        nullptr,      r4aEsp32NvmMenuTable, R4A_ESP32_NVM_MENU_ENTRIES},
    {"Servo Menu",      nullptr,  r4aPca9685ServoMenuTable, R4A_PCA9685_SERVO_MENU_ENTRIES},
};
const int menuTableEntries = sizeof(menuTable) / sizeof(menuTable[0]);
