/**********************************************************************
  R4A_ESP32_Timer.h

  Declare the Timer constants and data structures
**********************************************************************/

#ifndef __R4A_ESP32_TIMER_H__
#define __R4A_ESP32_TIMER_H__

//****************************************
// Types
//****************************************

// Timers: 0x3FF5F000, 0x3FF5F024, 0x3FF60000, 0x3FF60024
typedef struct _R4A_ESP32_TIMER_REGS
{
    uint32_t CONFIG;    // 0x00, R/W
    uint32_t LO;        // 0x04, RO
    uint32_t HI;        // 0x08, RO
    uint32_t UPDATE;    // 0x0c, WO
    uint32_t ALARMLO;   // 0x10, R/W
    uint32_t ALARMHI;   // 0x14, R/W
    uint32_t LOADLO;    // 0x18, R/W
    uint32_t LOAD;      // 0x20, WO
} R4A_ESP32_TIMER_REGS;

// Watchdogs: 0x3FF5F048, 0x3FF60048
typedef struct _R4A_ESP32_WATCHDOG_REGS
{
    uint32_t WDTCONFIG0;    // 0x00, R/W
    uint32_t WDTCONFIG1;    // 0x04, R/W
    uint32_t WDTCONFIG2;    // 0x08, R/W
    uint32_t WDTCONFIG3;    // 0x0c, R/W
    uint32_t WDTCONFIG4;    // 0x10, R/W
    uint32_t WDTCONFIG5;    // 0x14, R/W
    uint32_t WDTFEED;       // 0x18, WO
    uint32_t WDTWPROTECT;   // 0x20, R/W
} R4A_ESP32_WATCHDOG_REGS;

// Watchdogs: 0x3FF5F09c, 0x3FF6009c
typedef struct _R4A_ESP32_TIMER_INT_REGS
{
    uint32_t INT_RAW;   // 0x00, RO
    uint32_t INT_ST;    // 0x04, RO
    uint32_t INT_ENA;   // 0x08, R/W
    uint32_t INT_CLR;   // 0x0c, WO
} R4A_ESP32_TIMER_INT_REGS;

//****************************************
// API
//****************************************

void r4aEsp32TimerDisplayTimerRegs(uint32_t timerAddr, Print * display = &Serial);
void r4aEsp32TimerDisplayWatchdogRegs(uint32_t timerAddr, Print * display = &Serial);
void r4aEsp32TimerDisplayTimerIntRegs(uint32_t timerAddr, Print * display = &Serial);
void r4aEsp32TimerDisplayRegs(Print * display = &Serial);

#endif  // __R4A_ESP32_TIMER_H__
