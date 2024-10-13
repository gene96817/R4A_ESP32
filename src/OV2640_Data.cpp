/**********************************************************************
  OV2640_Data.cpp

  Robots-For-All (R4A)
  OV2640 camera data support
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// Constants
//****************************************

const R4A_OV2640_PINS r4aOV2640Pins =
{
    // Control pins
    -1, // pinReset
    -1, // pinPowerDown
    21, // pinXCLK

    // I2C pins
    27, // pinI2cClk
    26, // pinI2cData

    // Frame synchronization
    25, // pinVSYNC
    23, // pinHREF
    22, // pinPCLK

    // Image data pins
    4, // pinY2
    5, // pinY3
    18, // pinY4
    19, // pinY5
    36, // pinY6
    39, // pinY7
    34, // pinY8
    35, // pinY9
};
