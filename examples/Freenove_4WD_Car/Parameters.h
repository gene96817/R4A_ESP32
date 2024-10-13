/**********************************************************************
  Parameters.h

  Declare the parameters and set default values
**********************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

//****************************************
// mDNS
//****************************************

const char * mdnsHostName;

//****************************************
// NVM
//****************************************

const char * parameterFilePath;

//****************************************
// OV2640 camera
//****************************************

bool ov2640Enable;

//****************************************
// PCA9685
//****************************************

uint32_t pca9685FrequencyHz;

//****************************************
// Servos
//****************************************

uint8_t servoPanStartDegrees;
uint8_t servoTiltStartDegrees;

//****************************************
// Time zone offset
//****************************************

int8_t timeZoneHours;
int8_t timeZoneMinutes;
int8_t timeZoneSeconds;

//****************************************
// Web server
//****************************************

bool webServerEnable;

//****************************************
// WiFi Access Points
//****************************************

// Public access point SSID and password
const char * wifiSSID;
const char * wifiPassword;
const char * wifiSSID2;
const char * wifiPassword2;
const char * wifiSSID3;
const char * wifiPassword3;
const char * wifiSSID4;
const char * wifiPassword4;

//****************************************
// NVM Parameters
//****************************************

const R4A_ESP32_NVM_PARAMETER nvmParameters[] =
{
    // Multicast Domain Name Server (mDNS) host name prefix for .local
// Required    Type                  Minimum     Maximum        Address                     Name            Default Value
    {true,  R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &mdnsHostName,              "mdnsHostName", R4A_ESP32_NVM_STRING("robot")},

    // NVM parameters
// Required    Type                  Minimum     Maximum        Address                     Name            Default Value
    {true,  R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &parameterFilePath,         "ParamFile",    R4A_ESP32_NVM_STRING("/Parameters.txt")},

    // OV2640 camera
// Required    Type                  Minimum     Maximum        Address                     Name            Default Value
    {true,  R4A_ESP32_NVM_PT_BOOL,   0,          1,             &ov2640Enable,              "Camera",       false},

    // PCA9685
// Required    Type                  Minimum     Maximum        Address                     Name            Default Value
    {true,  R4A_ESP32_NVM_PT_UINT16, 24,         1525,          &pca9685FrequencyHz,        "pca9685Hz",    50},

    // Servos
    {true,  R4A_ESP32_NVM_PT_UINT8,  0,          180,           &servoPanStartDegrees,      "panDegrees",   SERVO_PAN_START},
    {true,  R4A_ESP32_NVM_PT_UINT8,  0,          180,           &servoTiltStartDegrees,     "tiltDegrees",  SERVO_TILT_START},

    // Time zone
// Required    Type                  Minimum     Maximum        Address                     Name            Default Value
    {true,  R4A_ESP32_NVM_PT_INT8,   -12,        12,            &timeZoneHours,             "tzHours",      -10},
    {true,  R4A_ESP32_NVM_PT_INT8,   -59,        59,            &timeZoneMinutes,           "tzMinutes",    0},
    {true,  R4A_ESP32_NVM_PT_INT8,   -59,        59,            &timeZoneSeconds,           "tzSeconds",    0},

    // Web server
// Required    Type                  Minimum     Maximum        Address                     Name            Default Value
    {true,  R4A_ESP32_NVM_PT_BOOL,   0,          1,             &webServerEnable,           "WebServer",    false},

    // WiFi: Public Access Points (APs)
// Required    Type                  Minimum     Maximum        Address                     Name            Default Value
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiSSID,                  "WifiSsid",     R4A_ESP32_NVM_STRING("IEEE")},
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiPassword,              "WifiPass",     R4A_ESP32_NVM_STRING("Robot-Dev")},
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiSSID2,                  "WifiSsid2",     R4A_ESP32_NVM_STRING("")},
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiPassword2,              "WifiPass2",     R4A_ESP32_NVM_STRING("")},
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiSSID3,                  "WifiSsid3",     R4A_ESP32_NVM_STRING("")},
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiPassword3,              "WifiPass3",     R4A_ESP32_NVM_STRING("")},
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiSSID4,                  "WifiSsid4",     R4A_ESP32_NVM_STRING("")},
    {false, R4A_ESP32_NVM_PT_P_CHAR, 0,          0,             &wifiPassword4,              "WifiPass4",     R4A_ESP32_NVM_STRING("")},
};
const int nvmParameterCount = sizeof(nvmParameters) / sizeof(nvmParameters[0]);

#endif  // __PARAMETERS_H__
