/**********************************************************************
  R4A_ESP32.h

  Declare the ESP32 support
**********************************************************************/

#ifndef __R4A_ESP32_H__
#define __R4A_ESP32_H__

#include <Arduino.h>            // Built-in
//#include <HardwareSerial.h>     // Built-in
#include <LittleFS.h>           // Built-in, load and store files in flash
#include <WiFiClient.h>         // Built-in
#include <Wire.h>               // Built-in

#include "R4A_Common.h"
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

// Define time constants
#define R4A_ESP32_MILLISECONDS_IN_A_SECOND  1000
#define R4A_ESP32_SECONDS_IN_A_MINUTE       60
#define R4A_ESP32_MILLISECONDS_IN_A_MINUTE  (R4A_ESP32_SECONDS_IN_A_MINUTE * R4A_ESP32_MILLISECONDS_IN_A_SECOND)
#define R4A_ESP32_MINUTES_IN_AN_HOUR        60
#define R4A_ESP32_MILLISECONDS_IN_AN_HOUR   (R4A_ESP32_MINUTES_IN_AN_HOUR * R4A_ESP32_MILLISECONDS_IN_A_MINUTE)
#define R4A_ESP32_HOURS_IN_A_DAY            24
#define R4A_ESP32_MILLISECONDS_IN_A_DAY     (R4A_ESP32_HOURS_IN_A_DAY * R4A_ESP32_MILLISECONDS_IN_AN_HOUR)

#define R4A_ESP32_SECONDS_IN_AN_HOUR        (R4A_ESP32_MINUTES_IN_AN_HOUR * R4A_ESP32_SECONDS_IN_A_MINUTE)

//****************************************
// Dump Buffer API
//****************************************

// Display a buffer contents in hexadecimal and ASCII
// Inputs:
//   offset: Offset of the first byte in the buffer, 0 or buffer address
//   buffer: Address of the buffer containing the data
//   length: Length of the buffer in bytes
//   display: Device used for output
void r4aDumpBuffer(uint32_t offset,
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

// Repeatedly display a fatal error message
// Inputs:
//   errorMessage: Zero terminated string of characters containing the
//                 error mesage to be displayed
//   display: Device used for output
void r4aEsp32ReportFatalError(const char * errorMessage,
                              Print * display = &Serial);

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

// Display the IO MUX registers
// Inputs:
//   display: Device used for output
void r4aEsp32GpioDisplayIoMuxRegisters(Print * display = &Serial);

// Display the GPIO registers
// Inputs:
//   display: Device used for output
void r4aEsp32GpioDisplayRegisters(Print * display = &Serial);

//****************************************
// I2C Bus API
//****************************************

typedef struct _R4A_ESP32_I2C
{
    TwoWire * bus;      // API for the I2C bus
    volatile int lock;  // Synchronize access to the I2C bus
} R4A_ESP32_I2C;

typedef struct _R4A_ESP32_I2C_DEVICE_DESCRIPTION
{
    uint8_t deviceAddress;  // 0 - 0x7f
    char * displayName;     // Name to display when the device is found
} R4A_ESP32_I2C_DEVICE_DESCRIPTION;

// Enumerate the I2C bus
// Inputs:
//   i2c: Address of R4A_ESP32_I2C structure
//   deviceTable: Address of the table containing the address and device
//                descriptions
//   deviceTableEntries: Number of entries in the I2C device table, >= 1
//   display: Device used for output
void r4aEsp32I2cBusEnumerate(R4A_ESP32_I2C * i2c,
                             const R4A_ESP32_I2C_DEVICE_DESCRIPTION * deviceTable,
                             int deviceTableEntries,
                             Print * display = &Serial);

// Ping an I2C device and see if it responds
// Inputs:
//   i2c: Address of R4A_ESP32_I2C structure
//   deviceAddress: Device address on the I2C bus (0 - 0x7f)
// Outputs:
//   Returns true if device detected, false otherwise
bool r4aEsp32I2cBusIsDevicePresent(R4A_ESP32_I2C * i2c, uint8_t deviceAddress);

// Read data from an I2C peripheral
// Inputs:
//   i2c: Address of R4A_ESP32_I2C structure
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
size_t r4aEsp32I2cBusRead(R4A_ESP32_I2C * i2c,
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
//   i2c: Address of R4A_ESP32_I2C structure
//   sdaPin: Number of the pin used for the SDA signal
//   sclPin: Number of the pin used for the SCL signal
//   clockHz: Clock speed for the I2C bus in Hertz
//   deviceTable: Address of the table containing the address and device
//                descriptions, may be nullptr
//   deviceTableEntries: Number of entries in the I2C device table
//   display: Device used for output
void r4aEsp32I2cBusSetup(R4A_ESP32_I2C * i2c,
                         int sdaPin,
                         int sclPin,
                         int clockHz,
                         const R4A_ESP32_I2C_DEVICE_DESCRIPTION * deviceTable,
                         int deviceTableEntries,
                         Print * display = &Serial);

// Send data to an I2C peripheral
// Inputs:
//   i2c: Address of R4A_ESP32_I2C structure
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
bool r4aEsp32I2cBusWrite(R4A_ESP32_I2C * i2c,
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
//   i2c: Address of R4A_ESP32_I2C structure
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
bool r4aEsp32I2cBusWriteWithLock(R4A_ESP32_I2C * i2c,
                                 uint8_t deviceI2cAddress,
                                 const uint8_t * cmdBuffer,
                                 size_t cmdByteCount,
                                 const uint8_t * dataBuffer,
                                 size_t dataByteCount,
                                 bool debug = false,
                                 bool releaseI2cBus = true,
                                 Print * display = &Serial);

//****************************************
// Lock API
//****************************************

// Take out a lock
// Inputs:
//   lock: Address of the lock
void r4aLockAcquire(volatile int * lock);

// Release a lock
// Inputs:
//   lock: Address of the lock
void r4aLockRelease(volatile int * lock);

//****************************************
// NVM API
//****************************************

#define R4A_ESP32_NVM_STRING(x)     ((uint64_t)(intptr_t)(const char *)x)
#define R4A_ESP32_NVM_FLOAT_CONV    ((double)(0x10000000ull))
#define R4A_ESP32_NVM_FLT(x)        ((uint64_t)(((double)x) * R4A_ESP32_NVM_FLOAT_CONV))

enum R4A_ESP32_NVM_PARAMETER_TYPE
{
    R4A_ESP32_NVM_PT_NULLPTR = 0,
    R4A_ESP32_NVM_PT_BOOL,        //  1
    R4A_ESP32_NVM_PT_INT8,        //  2
    R4A_ESP32_NVM_PT_UINT8,       //  3
    R4A_ESP32_NVM_PT_INT16,       //  4
    R4A_ESP32_NVM_PT_UINT16,      //  5
    R4A_ESP32_NVM_PT_INT32,       //  6
    R4A_ESP32_NVM_PT_UINT32,      //  7
    R4A_ESP32_NVM_PT_INT64,       //  8
    R4A_ESP32_NVM_PT_UINT64,      //  9
    R4A_ESP32_NVM_PT_FLOAT,       // 10
    R4A_ESP32_NVM_PT_DOUBLE,      // 11
    R4A_ESP32_NVM_PT_P_CHAR,      // 12
};

typedef union
{
    bool     b;
    int8_t   i8;
    uint8_t  u8;
    int16_t  i16;
    uint16_t u16;
    int32_t  i32;
    uint32_t u32;
    int64_t  i64;
    uint64_t u64;
    double   d;     // Float values are cast when read and written
    const char * pcc;
    void * pv;
} R4A_ESP32_NVM_VALUE;

typedef struct _R4A_ESP32_NVM_PARAMETER
{
    bool required;
    uint8_t type;
    uint64_t minimum;
    uint64_t maximum;
    void * addr;
    const char * name;
    uint64_t value;
} R4A_ESP32_NVM_PARAMETER;

// Clear a parameter by setting its value to zero
// Inputs:
//   filePath: Path to the file to be stored in NVM
//   parameterTable: Address of the first entry in the parameter table
//   parameterCount: Number of entries in the parameter table
//   name: Name of the parameter to be cleared
//   display: Device used for output
void r4aEsp32NvmParameterClear(const char * filePath,
                               const R4A_ESP32_NVM_PARAMETER * parameterTable,
                               int parameterCount,
                               const char * name,
                               Print * display = &Serial);

// Display a parameter
// Inputs:
//   parameter: Address of the entry in the parameter table to display
//   display: Device used for output
void r4aEsp32NvmDisplayParameter(const R4A_ESP32_NVM_PARAMETER * parameter,
                                 Print * display = &Serial);

// Display the parameters
// Inputs:
//   parameterTable: Address of the first entry in the parameter table
//   parameterCount: Number of entries in the parameter table
//   display: Device used for output
void r4aEsp32NvmDisplayParameters(const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                  int parameterCount,
                                  Print * display = &Serial);

// Dump the parameter file
// Inputs:
//   filePath: Path to the file contained in the NVM
//   display: Device used for output
void r4aEsp32NvmDumpParameterFile(const char * filePath,
                                  Print * display = &Serial);

// Get the default set of parameters
// Inputs:
//   parameterTable: Address of the first entry in the parameter table
//   parametersCount: Number of parameters in the table
void r4aEsp32NvmGetDefaultParameters(const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                     int parametersCount);

// Look up a parameter by name
// Inputs:
//   parameterTable: Address of the first entry in the parameter table
//   parameterCount: Number of entries in the parameter table
//   name: Name of the parameter to be found
//   display: Device used for output
// Outputs:
//   Returns the address of the found entry in the parameter table or
//   nullptr if the parameter was not found
const R4A_ESP32_NVM_PARAMETER * r4aEsp32NvmParameterLookup(const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                                           int parameterCount,
                                                           const char * name,
                                                           Print * display = &Serial);

// Read the parameters from a file
// Inputs:
//   filePath: Path to the file contained in the NVM
//   parameterTable: Address of the first entry in the parameter table
//   parametersCount: Number of parameters in the table
//   display: Device used for output
bool r4aEsp32NvmReadParameters(const char * filePath,
                               const R4A_ESP32_NVM_PARAMETER * parameterTable,
                               int parametersCount,
                               Print * display = &Serial);

// Set a parameter value
// Inputs:
//   filePath: Path to the file to be stored in NVM
//   parameterTable: Address of the first entry in the parameter table
//   parametersCount: Number of parameters in the table
//   parameter: Address of the specified parameter in the table
//   valueString: Character string containing the new value
//   display: Device used for output
void r4aEsp32NvmParameterSet(const char * filePath,
                             const R4A_ESP32_NVM_PARAMETER * parameterTable,
                             int parameterCount,
                             const R4A_ESP32_NVM_PARAMETER * parameter,
                             const char * valueString,
                             Print * display = &Serial);

// Write the parameters to a file
// Inputs:
//   filePath: Path to the file to be stored in NVM
//   parameterTable: Address of the first entry in the parameter table
//   parametersCount: Number of parameters in the table
//   display: Device used for output
void r4aEsp32NvmWriteParameters(const char * filePath,
                                const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                int parametersCount,
                                Print * display = &Serial);

//****************************************
// Stream API
//****************************************

// Read a line of input from a serial port into a character array
// Inputs:
//   port: Address of a HardwareSerial port structure
//   echo: Specify true to enable echo of input characters and false otherwise
//   buffer: Address of a string that contains the input line
// Outputs:
//   nullptr when the line is not complete
String * r4aEsp32ReadLine(bool echo, String * buffer, HardwareSerial * port = &Serial);

// Read a line of input from a WiFi client into a character array
// Inputs:
//   port: Address of a WiFiClient port structure
//   echo: Specify true to enable echo of input characters and false otherwise
//   buffer: Address of a string that contains the input line
// Outputs:
//   nullptr when the line is not complete
String * r4aEsp32ReadLine(bool echo, String * buffer, WiFiClient * port);

//****************************************
// Stricmp API
//****************************************

// Compare two strings ignoring case
// Inputs:
//   str1: Address of a zero terminated string of characters
//   str2: Address of a zero terminated string of characters
// Outputs:
//   Returns the delta value of the last comparison (str1[x] - str2[x])
int r4aEsp32Stricmp(const char *str1, const char *str2);

//****************************************
// Timer API
//****************************************

// Display the timer registers
// Inputs:
//   timerAddr: Address of the timer registers of interest
//   display: Device used for output
void r4aEsp32TimerDisplayTimerRegs(uint32_t timerAddr, Print * display = &Serial);

// Display the timer interrupt registers
// Inputs:
//   timerAddr: Address of the timer registers of interest
//   display: Device used for output
void r4aEsp32TimerDisplayTimerIntRegs(uint32_t timerAddr, Print * display = &Serial);

// Display the watchdog registers
// Inputs:
//   timerAddr: Address of the timer registers of interest
//   display: Device used for output
void r4aEsp32TimerDisplayWatchdogRegs(uint32_t timerAddr, Print * display = &Serial);

// Display the timer registers
// Inputs:
//   display: Device used for output
void r4aEsp32TimerDisplayRegs(Print * display = &Serial);

#endif  // __R4A_ESP32_H__
