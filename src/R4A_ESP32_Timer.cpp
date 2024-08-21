/**********************************************************************
  R4A_ESP32_Timer.cpp

  Decode and display the timer registers
**********************************************************************/

#include "R4A_ESP32.h"

//*********************************************************************
// Display the timer registers
void r4aEsp32TimerDisplayTimerRegs(uint32_t timerAddr, Print * display)
{
    R4A_ESP32_TIMER_REGS * timer = (R4A_ESP32_TIMER_REGS *)timerAddr;
    int n = (timerAddr & 0xff) ? 1 : 0;

    display->printf("Timer @ 0x%08x %s\r\n", timerAddr,
                    (timer->CONFIG & 0x80000000) ? "Enabled" : "Disabled");
    display->printf("    0x%08x: T%d_CONFIG\r\n", timer->CONFIG, n);
    display->printf("    0x%08x: T%d_LO\r\n", timer->LO, n);
    display->printf("    0x%08x: T%d_HI\r\n", timer->HI, n);
    display->printf("    0x%08x: T%d_ALARMLO\r\n", timer->ALARMLO, n);
    display->printf("    0x%08x: T%d_ALARMHI\r\n", timer->ALARMHI, n);
    display->printf("    0x%08x: T%d_LOADLO\r\n", timer->LOADLO, n);
}

//*********************************************************************
// Display the watchdog registers
void r4aEsp32TimerDisplayWatchdogRegs(uint32_t timerAddr, Print * display)
{
    R4A_ESP32_WATCHDOG_REGS * watchdog = (R4A_ESP32_WATCHDOG_REGS *)timerAddr;

    display->printf("Watchdog @ %p, %s\r\n", watchdog,
                    (watchdog->WDTCONFIG0 & 0x80000000) ? "Enabled" : "Disabled");
    display->printf("    0x%08x: WDTCONFIG0\r\n", watchdog->WDTCONFIG0);
    display->printf("    0x%08x: WDTCONFIG1\r\n", watchdog->WDTCONFIG1);
    display->printf("    0x%08x: WDTCONFIG2\r\n", watchdog->WDTCONFIG2);
    display->printf("    0x%08x: WDTCONFIG3\r\n", watchdog->WDTCONFIG3);
    display->printf("    0x%08x: WDTCONFIG4\r\n", watchdog->WDTCONFIG4);
    display->printf("    0x%08x: WDTCONFIG5\r\n", watchdog->WDTCONFIG5);
    display->printf("    0x%08x: WDTWPROTECT\r\n", watchdog->WDTWPROTECT);
}

//*********************************************************************
// Display the timer interrupt registers
void r4aEsp32TimerDisplayTimerIntRegs(uint32_t timerAddr, Print * display)
{
    R4A_ESP32_TIMER_INT_REGS * timerInt = (R4A_ESP32_TIMER_INT_REGS *)timerAddr;

    display->printf("Timer Interrupts @ 0x%08x, %s\r\n", timerAddr,
                    (timerInt->INT_RAW & 7) ? "Enabled" : "Disabled");
    display->printf("    0x%08x: INT_RAW (WDT, T1, T0: Timer Expired)\r\n", timerInt->INT_RAW);
    display->printf("    0x%08x: INT_ENA (WDT, T1, T0: Interrupt Enable)\r\n", timerInt->INT_ENA);
    display->printf("    0x%08x: INT_ST  (WDT, T1, T0: Interrupt Pending)\r\n", timerInt->INT_ST);
}

//*********************************************************************
// Display the timer registers
void r4aEsp32TimerDisplayRegs(Print * display)
{
    // Timer group 0
    r4aEsp32TimerDisplayTimerRegs(0x3FF5F000, display);
    r4aEsp32TimerDisplayTimerRegs(0x3FF5F024, display);
    r4aEsp32TimerDisplayWatchdogRegs(0x3FF5F048, display);
    r4aEsp32TimerDisplayTimerIntRegs(0x3FF5F09c, display);
    display->println();

    // Timer group 1
    r4aEsp32TimerDisplayTimerRegs(0x3FF60000, display);
    r4aEsp32TimerDisplayTimerRegs(0x3FF60024, display);
    r4aEsp32TimerDisplayWatchdogRegs(0x3FF60048, display);
    r4aEsp32TimerDisplayTimerIntRegs(0x3FF6009c, display);
    display->println();
}
