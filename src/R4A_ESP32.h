/**********************************************************************
  R4A_ESP32.h

  Declare the ESP32 support
**********************************************************************/

#ifndef __R4A_ESP32_H__
#define __R4A_ESP32_H__

#include "R4A_ESP32_GPIO.h"
#include "R4A_ESP32_Timer.h"

//****************************************
// API
//****************************************

void r4aEsp32DumpBuffer(Print * display,
                        uint32_t offset,
                        const uint8_t *buffer,
                        uint32_t length);

#endif  // __R4A_ESP32_H__
