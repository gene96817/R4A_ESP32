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
// Car class
//****************************************

// The Freenove 4WD Car has 12 multicolor LEDs (WS2812) arranged as:
//
//                      Front
//          L1  L2  L3         R3  R2  R1
//          10  11  12         13  14  15
//
//          21  20  19         18  17  16
//                       Back

#define FRONT_L1        0
#define FRONT_L2        1
#define FRONT_L3        2
#define FRONT_R3        3
#define FRONT_R2        4
#define FRONT_R1        5
#define BACK_R1         6
#define BACK_R2         7
#define BACK_R3         8
#define BACK_L3         9
#define BACK_L2         10
#define BACK_L1         11

class R4A_Freenove_4WD_Car
{
  private:

    uint32_t _backupLightColor; // Color for LEDS 17 - 20
    const uint32_t _blinkMsec;  // Milliseconds between LED state changes
    uint32_t _brakeLightColor;  // Color for LEDS 17 - 20
    uint32_t _headlightColor;   // Color for LEDS 11 - 14
    uint8_t  _ledState;         // State of the LEDs
    volatile uint32_t _timer;   // Turn signal timer in milliseconds
    volatile bool _updateLEDs;  // Set true when changing LED color

    enum
    {
        STATE_OFF = 0,
        STATE_BLINK_L3,
        STATE_BLINK_L2,
        STATE_BLINK_L1,
        STATE_BLINK_L_OFF,
        STATE_BLINK_R3,
        STATE_BLINK_R2,
        STATE_BLINK_R1,
        STATE_BLINK_R_OFF,
    };

  public:

    const int numberOfLEDs = 12;

    // Constructor
    R4A_Freenove_4WD_Car(uint32_t blinkMsec = 150)
        : _backupLightColor{R4A_LED_OFF}, _blinkMsec{blinkMsec},
          _brakeLightColor{R4A_LED_OFF}, _headlightColor{R4A_LED_OFF},
          _ledState{0}, _timer{0}, _updateLEDs{false}
    {
    }

    // Get the backup light color
    // Outputs:
    //   Returns the color of the backup lights
    uint32_t backupLightColorGet();

    // Set the backup light color
    // Inputs:
    //   color: New color to use for the backup lights
    void backupLightColorSet(uint32_t color);

    // Turn off the backup lights
    void backupLightsOff();

    // Turn on the backup lights
    void backupLightsOn();

    // Toggle the backup lights
    void backupLightsToggle();

    // Get the brake light color
    // Outputs:
    //   Returns the color of the brake lights
    uint32_t brakeLightColorGet();

    // Set the brake light color
    // Inputs:
    //   color: New color to use for the brake lights
    void brakeLightColorSet(uint32_t color);

    // Turn off the brake lights
    void brakeLightsOff();

    // Turn on the brake lights
    void brakeLightsOn();

    // Toggle the brake lights
    void brakeLightsToggle();

    // Get the headlight color
    // Outputs:
    //   Returns the color of the headlights
    uint32_t headlightColorGet();

    // Set the headlight color
    // Inputs:
    //   color: New color to use for the headlights
    void headlightColorSet(uint32_t color);

    // Turn off the headlights
    void headlightsOff();

    // Turn on the headlights
    void headlightsOn();

    // Toggle the headlights
    void headlightsToggle();

    // Turn off all the LEDs
    void ledsOff();

    // Use LEDs to indicate left turn
    void ledsTurnLeft();

    // Stop using the LEDs to indicate a turn
    void ledsTurnOff();

    // Use LEDs to indicate right turn
    void ledsTurnRight();

    // Write the new color values to the WS2812 LEDs
    // Inputs:
    //   currentMsec: Number of milliseconds since boot
    void ledsUpdate(uint32_t currentMsec);
};

//****************************************
// ESP32 WRover Module Pins
//****************************************

#define BLUE_LED_BUZZER_PIN     2   // Blue LED and buzzer output
#define BATTERY_WS2812_PIN      32  // WS2812 output, Battery voltage input
#define LIGHT_SENSOR_PIN        33

#define ESP32_WROVER_BLUE_LED_OFF       0
#define ESP32_WROVER_BLUE_LED_ON        1

//****************************************
// Line sensors
//****************************************

// Line sensor bits
#define LINE_SENSOR_LEFT_SHIFT      0
#define LINE_SENSOR_CENTER_SHIFT    1
#define LINE_SENSOR_RIGHT_SHIFT     2

#define LINE_SENSOR_LEFT_MASK       (1 << LINE_SENSOR_LEFT_SHIFT)
#define LINE_SENSOR_CENTER_MASK     (1 << LINE_SENSOR_CENTER_SHIFT)
#define LINE_SENSOR_RIGHT_MASK      (1 << LINE_SENSOR_RIGHT_SHIFT)

#endif  // __FREENOVE_4WD_CAR_H__
