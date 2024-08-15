/**********************************************************************
  R4A_ESP32_GPIO.cpp

  Decode and display the GPIO pins
**********************************************************************/

#include "R4A_ESP32_GPIO.h"

//*********************************************************************
// Return the index into the I/O mux registers, return -1 if none
int r4aEsp32GpioGetIoMuxRegisterIndex(int portNumber)
{
    // Validate the port number
    if (portNumber >= R4A_GPIO_MAX_PORTS)
        return -1;

    // Lookup the I/O mux register index
    return r4aGpioPortToIoMuxIndex[portNumber];
}

//*********************************************************************
// Return the I/O mux register value, return -1
int r4aEsp32GpioGetIoMuxRegister(int portNumber)
{
    uint32_t * muxReg;
    int index;
    uint32_t regValue;

    // Validate the register index
    index = r4aGpioGetIoMuxRegisterIndex(portNumber);
    if (index < 0)
        return (uint32_t)-1;

    // Lookup the
    muxReg = &r4aIoMux->R4A_IO_MUX_GPIO36_REG;
    regValue = muxReg[index];
    return regValue;
}

//*********************************************************************
// Return the output function select value, return -1 if none
int r4aEsp32GpioGetOutputFuncSelect(int portNumber)
{
    uint32_t outputSelect;

    // Validate the port number
    if (portNumber >= R4A_GPIO_MAX_PORTS)
        return -1;

    // Get the function select value
    outputSelect = r4aGpioRegs->R4A_GPIO_FUNC_OUT_SEL_CFG_REG[portNumber];
    return outputSelect;
}

//*********************************************************************
// Display the GPIO registers
void r4aEsp32GpioDisplayRegisters(Print * display)
{
    // Display the GPIO registers
    display->println("GPIO Registers");
    display->printf("    0x%08x: GPIO_OUT_REG\r\n", r4aGpioRegs->R4A_GPIO_OUT_REG);
    display->printf("    0x%08x: GPIO_OUT_W1TS_REG\r\n", r4aGpioRegs->R4A_GPIO_OUT_W1TS_REG);
    display->printf("    0x%08x: GPIO_OUT_W1TC_REG\r\n", r4aGpioRegs->R4A_GPIO_OUT_W1TC_REG);
    display->printf("    0x%08x: GPIO_OUT1_REG\r\n", r4aGpioRegs->R4A_GPIO_OUT1_REG);
    display->printf("    0x%08x: GPIO_OUT1_W1TS_REG\r\n", r4aGpioRegs->R4A_GPIO_OUT1_W1TS_REG);
    display->printf("    0x%08x: GPIO_OUT1_W1TC_REG\r\n", r4aGpioRegs->R4A_GPIO_OUT1_W1TC_REG);
    display->printf("    0x%08x: GPIO_ENABLE_REG\r\n", r4aGpioRegs->R4A_GPIO_ENABLE_REG);
    display->printf("    0x%08x: GPIO_ENABLE_W1TS_REG\r\n", r4aGpioRegs->R4A_GPIO_ENABLE_W1TS_REG);
    display->printf("    0x%08x: GPIO_ENABLE_W1TC_REG\r\n", r4aGpioRegs->R4A_GPIO_ENABLE_W1TC_REG);
    display->printf("    0x%08x: GPIO_ENABLE1_REG\r\n", r4aGpioRegs->R4A_GPIO_ENABLE1_REG);
    display->printf("    0x%08x: GPIO_ENABLE1_W1TS_REG\r\n", r4aGpioRegs->R4A_GPIO_ENABLE1_W1TS_REG);
    display->printf("    0x%08x: GPIO_ENABLE1_W1TC_REG\r\n", r4aGpioRegs->R4A_GPIO_ENABLE1_W1TC_REG);
    display->printf("    0x%08x: GPIO_STRAP_REG\r\n", r4aGpioRegs->R4A_GPIO_STRAP_REG);
    display->printf("    0x%08x: GPIO_IN_REG\r\n", r4aGpioRegs->R4A_GPIO_IN_REG);
    display->printf("    0x%08x: GPIO_IN1_REG\r\n", r4aGpioRegs->R4A_GPIO_IN1_REG);
    display->printf("    0x%08x: GPIO_STATUS_REG\r\n", r4aGpioRegs->R4A_GPIO_STATUS_REG);
    display->printf("    0x%08x: GPIO_STATUS_W1TS_REG\r\n", r4aGpioRegs->R4A_GPIO_STATUS_W1TS_REG);
    display->printf("    0x%08x: GPIO_STATUS_W1TC_REG\r\n", r4aGpioRegs->R4A_GPIO_STATUS_W1TC_REG);
    display->printf("    0x%08x: GPIO_STATUS1_REG\r\n", r4aGpioRegs->R4A_GPIO_STATUS1_REG);
    display->printf("    0x%08x: GPIO_STATUS1_W1TS_REG\r\n", r4aGpioRegs->R4A_GPIO_STATUS1_W1TS_REG);
    display->printf("    0x%08x: GPIO_STATUS1_W1TC_REG\r\n", r4aGpioRegs->R4A_GPIO_STATUS1_W1TC_REG);
    display->printf("    0x%08x: GPIO_ACPU_INT_REG\r\n", r4aGpioRegs->R4A_GPIO_ACPU_INT_REG);
    display->printf("    0x%08x: GPIO_ACPUNMI_INT_REG\r\n", r4aGpioRegs->R4A_GPIO_ACPUNMI_INT_REG);
    display->printf("    0x%08x: GPIO_PCPU_INT_REG\r\n", r4aGpioRegs->R4A_GPIO_PCPU_INT_REG);
    display->printf("    0x%08x: GPIO_PCPU_NMI_INT_REG\r\n", r4aGpioRegs->R4A_GPIO_PCPU_NMI_INT_REG);
    display->printf("    0x%08x: GPIO_ACPU_INT1_REG\r\n", r4aGpioRegs->R4A_GPIO_ACPU_INT1_REG);
    display->printf("    0x%08x: GPIO_ACPU_NMI_INT1_REG\r\n", r4aGpioRegs->R4A_GPIO_ACPU_NMI_INT1_REG);
    display->printf("    0x%08x: GPIO_PCPU_INT1_REG\r\n", r4aGpioRegs->R4A_GPIO_PCPU_INT1_REG);
    display->printf("    0x%08x: GPIO_PCPU_NMI_INT1_REG\r\n", r4aGpioRegs->R4A_GPIO_PCPU_NMI_INT1_REG);
    for (int i = 0; i < R4A_GPIO_MAX_PORTS; i++)
        display->printf("    0x%08x: GPIO_PIN%d_REG\r\n", r4aGpioRegs->R4A_GPIO_PIN_REG[i], i);
    for (int i = 0; i < 256; i++)
        display->printf("    0x%08x: GPIO_FUNC%d_IN_SEL_CFG_REG\r\n", r4aGpioRegs->R4A_GPIO_FUNC_IN_SEL_CFG_REG[i], i);
    for (int i = 0; i < R4A_GPIO_MAX_PORTS; i++)
        display->printf("    0x%08x: GPIO_FUNC%d_OUT_SEL_CFG_REG\r\n", r4aGpioRegs->R4A_GPIO_FUNC_OUT_SEL_CFG_REG[i], i);
}

//*********************************************************************
// Decode the IO Mux registers
void r4aEsp32GpioDisplayIoMuxRegisters(int portNumber, uint32_t regValue, Print * display)
{
    const char * function;

    function = r4aIoMuxNames[portNumber][(regValue & IO_MUX_MCU_SEL) >> 12];
    display->printf(", %s", function);
    display->printf(", DRV: %d", (regValue & IO_MUX_FUN_DRV) >> 10);
    if (regValue & IO_MUX_IN_IE) display->print(", IE");
    if (regValue & IO_MUX_FUN_WPU) display->print(", Pull-up");
    if (regValue & IO_MUX_FUN_WPD) display->print(", Pull-down");
    display->print(", Sleep Mode: ");
    display->printf(", DRV: %d", (regValue & IO_MUX_MCU_DRV) >> 5);
    if (regValue & IO_MUX_MCU_IE) display->print(", IE");
    if (regValue & IO_MUX_MCU_WPU) display->print(", PU");
    if (regValue & IO_MUX_MCU_WPD) display->print(", PD");
    if (regValue & IO_MUX_SLP_SEL) display->print(", Sleep");
    if (regValue & IO_MUX_MCU_OE) display->print(", OE");
    display->println();
}

//*********************************************************************
// Display the IO MUX registers
void r4aEsp32GpioDisplayIoMuxRegisters(Print * display)
{
    int portNumber;
    uint32_t regValue;

    display->println("IO Mux Registers");
    display->printf("    0x%08x: R4A_IO_MUX_PIN_CTRL\r\n", r4aIoMux->R4A_IO_MUX_PIN_CTRL);
    for (portNumber = 0; portNumber < R4A_GPIO_MAX_PORTS; portNumber++)
    {
        regValue = r4aGpioGetIoMuxRegister(portNumber);
        if (regValue != (uint32_t)-1)
        {
            display->printf("    0x%08x: IO_MUX_GPIO%d_REG", regValue, portNumber);
            r4aGpioDisplayIoMuxRegisters(portNumber, regValue, display);
        }
    }
}
