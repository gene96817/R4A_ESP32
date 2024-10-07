/**********************************************************************
  R4A_Freenove_4WD_Car.h

  Robots-For-All (R4A)
  Freenove 4WD Car robot declarations
  https://store.freenove.com/products/fnk0053
  https://www.amazon.com/gp/product/B08X6PTQFM/ref=ox_sc_act_title_1
**********************************************************************/

#ifndef __FREENOVE_4WD_CAR_H__
#define __FREENOVE_4WD_CAR_H__

#include "R4A_ESP32.h"          // Robots-For-All ESP32 support

//****************************************
// Battery
//****************************************

#define BATTERY_VOLTAGE_MULTIPLIER  4

#define DISPLAY_BATTERY_VOLTAGE(display)  \
    r4aEsp32VoltageDisplay(BATTERY_WS2812_PIN, 0, BATTERY_VOLTAGE_MULTIPLIER, display)

#define READ_BATTERY_VOLTAGE(adcValue)  \
    r4aEsp32VoltageGet(BATTERY_WS2812_PIN, 0, BATTERY_VOLTAGE_MULTIPLIER, adcValue)

//****************************************
// Buzzer
//****************************************

#define BUZZER_PWM_CHANNEL      0
#define BUZZER_FREQUENCY        2000

//****************************************
// ESP32 WRover Module Pins
//****************************************

#define BLUE_LED_BUZZER_PIN     2   // Blue LED and buzzer output
#define BATTERY_WS2812_PIN      32  // WS2812 output, Battery voltage input
#define LIGHT_SENSOR_PIN        33

#define ESP32_WROVER_BLUE_LED_OFF       0
#define ESP32_WROVER_BLUE_LED_ON        1

#endif  // __FREENOVE_4WD_CAR_H__
