/**********************************************************************
  R4A_ESP32.h

  Declare the ESP32 support
**********************************************************************/

#ifndef __R4A_ESP32_H__
#define __R4A_ESP32_H__

#include <Arduino.h>
#include <Wire.h>

#include "R4A_ESP32_GPIO.h"
#include "R4A_ESP32_Timer.h"

//****************************************
// Constants
//****************************************

extern R4A_GPIO_REGS * const r4aGpioRegs;
extern R4A_IO_MUX_REGS * const r4aIoMux;
extern R4A_RTCIO_REGS * const r4aRtcioRegs;

extern const int r4aGpioPortToIoMuxIndex[R4A_GPIO_MAX_PORTS];
extern const char * const r4aIoMuxNames[R4A_GPIO_MAX_PORTS][8];
extern const uint8_t r4aIoMuxIsGpio[R4A_GPIO_MAX_PORTS];
extern const R4A_GPIO_MATRIX r4aGpioMatrixNames[256];

// I2C General call values
const uint8_t I2C_GENERAL_CALL_DEVICE_ADDRESS = 0x00;
const uint8_t I2C_SWRST = 0x06;

//****************************************
// Dump Buffer API
//****************************************

// Display a buffer contents in hexadecimal and ASCII
// Inputs:
//   offset: Offset of the first byte in the buffer, 0 or buffer address
//   buffer: Address of the buffer containing the data
//   length: Length of the buffer in bytes
//   display: Device used for output
void r4aEsp32DumpBuffer(uint32_t offset,
                        const uint8_t *buffer,
                        uint32_t length,
                        Print * display = &Serial);

//****************************************
// ESP32 API
//****************************************

// Determine if the address is in the EEPROM (Flash)
// Inputs:
//   addr: Address in question
// Outputs:
//   Returns true if the address is in EEPROM and false otherwise
bool r4aEsp32IsAddressInEEPROM(void * addr);

// Determine if the address is in PSRAM (SPI RAM)
// Inputs:
//   addr: Address in question
// Outputs:
//   Returns true if the address is in PSRAM and false otherwise
bool r4aEsp32IsAddressInPSRAM(void * addr);

// Determine if the address is in PSRAM or SRAM
// Inputs:
//   addr: Address in question
// Outputs:
//   Returns true if the address is in PSRAM or SRAM and false otherwise
bool r4aEsp32IsAddressInRAM(void * addr);

// Determine if the address is in ROM
// Inputs:
//   addr: Address in question
// Outputs:
//   Returns true if the address is in ROM and false otherwise
bool r4aEsp32IsAddressInROM(void * addr);

// Determine if the address is in SRAM
// Inputs:
//   addr: Address in question
// Outputs:
//   Returns true if the address is in SRAM and false otherwise
bool r4aEsp32IsAddressInSRAM(void * addr);

// Take out a lock
// Inputs:
//   lock: Address of the lock
void r4aEsp32LockAcquire(volatile int * lock);

// Release a lock
// Inputs:
//   lock: Address of the lock
void r4aEsp32LockRelease(volatile int * lock);

// Find the specified partition
// Inputs:
//   name: Partition name to be found
// Outputs:
//   Returns true if the partition was found and false otherwise
bool r4aEsp32PartitionFind(const char * name);

// Display the partition table
// Inputs:
//   display: Device used for output
void r4aEsp32PartitionTableDisplay(Print * display = &Serial);

// Set and save the pin mode
// Inputs:
//   pin: Number of the pin to set
//   mode: Mode of operation for the pin
// Outputs:
//   Returns the previous mode of operation for the pin
uint8_t r4aEsp32PinMode(uint8_t pin, uint8_t mode);

// Read a line of input from a Stream into a String
// Inputs:
//   buffer: Address of a string that contains the input line
//   stream: Address of a stream in input characters
// Outputs:
//   Returns the address of a String containing a line of characters or
//   nullptr when the line is not complete
String * r4aEsp32ReadLine(String * buffer, Stream * stream);

// Display the voltage
// Inputs:
//   adcPin: GPIO pin number for the ADC pin
//   offset: Ground level offset correction
//   multiplier: Multiplier for each of the ADC bits
//   display: Device used for output
void r4aEsp32VoltageDisplay(int adcPin,
                            float offset,
                            float multiplier,
                            Print * display = &Serial);

// Read the voltage
// Inputs:
//   adcPin: GPIO pin number for the ADC pin
//   offset: Ground level offset correction
//   multiplier: Multiplier for each of the ADC bits
//   adcValue: Return the value read from the ADC
// Outputs:
//   Returns the computed voltage
float r4aEsp32VoltageGet(int adcPin,
                         float offset,
                         float multiplier,
                         int16_t * adcValue);

// Set the reference voltage
// Inputs:
//   maximumVoltage: Maximum voltage represented by the ADC
void r4aEsp32VoltageSetReference(float maximumVoltage);

//****************************************
// GPIO API
//****************************************

// Display the GPIO registers
// Inputs:
//   display: Device used for output
void r4aEsp32GpioDisplayRegisters(Print * display = &Serial);

// Display the IO MUX registers
// Inputs:
//   display: Device used for output
void r4aEsp32GpioDisplayIoMuxRegisters(Print * display = &Serial);

//****************************************
// I2C Bus API
//****************************************

typedef struct _R4A_I2C
{
    TwoWire * bus;
    volatile int lock;
} R4A_I2C;

typedef struct _R4A_I2C_DEVICE_DESCRIPTION
{
    uint8_t deviceAddress;  // 0 - 0x7f
    char * displayName;     // Name to display when the device is found
} R4A_I2C_DEVICE_DESCRIPTION;

// Ping an I2C device and see if it responds
// Inputs:
//   i2c: Address of R4A_I2C structure
//   deviceAddress: Device address on the I2C bus (0 - 0x7f)
// Outputs:
//   Returns true if device detected, false otherwise
bool r4aEsp32I2cBusIsDevicePresent(R4A_I2C * i2c, uint8_t deviceAddress);

// Enumerate the I2C bus
// Inputs:
//   i2c: Address of R4A_I2C structure
//   deviceTable: Address of the table containing the address and device
//                descriptions
//   deviceTableEntries: Number of entries in the I2C device table, >= 1
//   display: Device used for output
void r4aEsp32I2cBusEnumerate(R4A_I2C * i2c,
                             const R4A_I2C_DEVICE_DESCRIPTION * deviceTable,
                             int deviceTableEntries,
                             Print * display = &Serial);

// Send data to an I2C peripheral
// Inputs:
//   i2c: Address of R4A_I2C structure
//   deviceAddress: Device address on the I2C bus (0 - 0x7f)
//   cmdBuffer: Address of the buffer containing the command bytes, may be nullptr
//   cmdByteCount: Number of bytes to send from the command buffer
//   dataBuffer: Address of the buffer containing the data bytes, may be nullptr
//   dataByteCount: Number of bytes to send from the data buffer
//   debug: A true value enables debugging for the I2C transaction
//   releaseI2cBus: A value of true releases the I2C bus after the transaction
//   display: Device used for output
// Outputs:
//   Returns true upon success, false otherwise
bool r4aEsp32I2cBusWriteWithLock(R4A_I2C * i2c,
                                 uint8_t deviceI2cAddress,
                                 const uint8_t * cmdBuffer,
                                 size_t cmdByteCount,
                                 const uint8_t * dataBuffer,
                                 size_t dataByteCount,
                                 bool debug = false,
                                 bool releaseI2cBus = true,
                                 Print * display = &Serial);

// Send data to an I2C peripheral
// Inputs:
//   i2c: Address of R4A_I2C structure
//   deviceAddress: Device address on the I2C bus (0 - 0x7f)
//   cmdBuffer: Address of the buffer containing the command bytes, may be nullptr
//   cmdByteCount: Number of bytes to send from the command buffer
//   dataBuffer: Address of the buffer containing the data bytes, may be nullptr
//   dataByteCount: Number of bytes to send from the data buffer
//   debug: A true value enables debugging for the I2C transaction
//   releaseI2cBus: A value of true releases the I2C bus after the transaction
//   display: Device used for output
// Outputs:
//   Returns true upon success, false otherwise
bool r4aEsp32I2cBusWrite(R4A_I2C * i2c,
                         uint8_t deviceI2cAddress,
                         const uint8_t * cmdBuffer,
                         size_t cmdByteCount,
                         const uint8_t * dataBuffer,
                         size_t dataByteCount,
                         bool debug = false,
                         bool releaseI2cBus = true,
                         Print * display = &Serial);

// Read data from an I2C peripheral
// Inputs:
//   i2c: Address of R4A_I2C structure
//   deviceAddress: Device address on the I2C bus (0 - 0x7f)
//   cmdBuffer: Address of the buffer containing the command bytes, may be nullptr
//   cmdByteCount: Number of bytes to send from the command buffer
//   dataBuffer: Address of the buffer to receive the data bytes, may be nullptr
//   dataByteCount: Size in bytes of the data buffer, maximum receive bytes
//   debug: A true value enables debugging for the I2C transaction
//   releaseI2cBus: A value of true releases the I2C bus after the transaction
//   display: Device used for output
// Outputs:
//   Returns the number of bytes read
size_t r4aEsp32I2cBusRead(R4A_I2C * i2c,
                          uint8_t deviceI2cAddress,
                          const uint8_t * cmdBuffer, // Does not include I2C address
                          size_t cmdByteCount,
                          uint8_t * readBuffer,
                          size_t readByteCount,
                          bool debug = false,
                          bool releaseI2cBus = true,
                          Print * display = &Serial);

// Initialize the I2C bus
// Inputs:
//   i2c: Address of R4A_I2C structure
//   sdaPin: Number of the pin used for the SDA signal
//   sclPin: Number of the pin used for the SCL signal
//   clockHz: Clock speed for the I2C bus in Hertz
//   deviceTable: Address of the table containing the address and device
//                descriptions, may be nullptr
//   deviceTableEntries: Number of entries in the I2C device table
//   display: Device used for output
void r4aEsp32I2cBusSetup(R4A_I2C * i2c,
                         int sdaPin,
                         int sclPin,
                         int clockHz,
                         const R4A_I2C_DEVICE_DESCRIPTION * deviceTable,
                         int deviceTableEntries,
                         Print * display = &Serial);

//****************************************
// Timer API
//****************************************

// Display the timer registers
// Inputs:
//   timerAddr: Address of the timer registers of interest
//   display: Device used for output
void r4aEsp32TimerDisplayTimerRegs(uint32_t timerAddr, Print * display = &Serial);

// Display the watchdog registers
// Inputs:
//   timerAddr: Address of the timer registers of interest
//   display: Device used for output
void r4aEsp32TimerDisplayWatchdogRegs(uint32_t timerAddr, Print * display = &Serial);

// Display the timer interrupt registers
// Inputs:
//   timerAddr: Address of the timer registers of interest
//   display: Device used for output
void r4aEsp32TimerDisplayTimerIntRegs(uint32_t timerAddr, Print * display = &Serial);

// Display the timer registers
// Inputs:
//   display: Device used for output
void r4aEsp32TimerDisplayRegs(Print * display = &Serial);

#endif  // __R4A_ESP32_H__
