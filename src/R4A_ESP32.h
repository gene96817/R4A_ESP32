/**********************************************************************
  R4A_ESP32.h

  Robots-For-All (R4A)
  Declare the ESP32 support
**********************************************************************/

#ifndef __R4A_ESP32_H__
#define __R4A_ESP32_H__

#include <Arduino.h>            // Built-in
#include <LittleFS.h>           // Built-in, load and store files in flash
#include <WiFi.h>               // Built-in
#include <WiFiClient.h>         // Built-in
#include <WiFiMulti.h>          // Built-in, multiple WiFi AP support
#include <WiFiServer.h>         // Built-in

#include <esp_camera.h>         // Built-in, needed for OV2640 camera
#include <esp32-hal-spi.h>      // Built-in
#include <esp_http_server.h>    // Built in, needed for camera web server

#include <R4A_Robot.h>          // Robots-For-All robot support
#include <R4A_I2C.h>            // Robots-For-All I2C support
#include "R4A_ESP32_GPIO.h"     // Robots-For-All ESP32 GPIO declarations
#include "R4A_ESP32_Timer.h"    // Robots-For-All ESP32 Timer declarations

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

// System reset
void r4aEsp32SystemReset();

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
// ESP32 Menu API
//****************************************

// Display the heap
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32MenuDisplayHeap(const struct _R4A_MENU_ENTRY * menuEntry,
                             const char * command,
                             Print * display);

// Display the partitions
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32MenuDisplayPartitions(const struct _R4A_MENU_ENTRY * menuEntry,
                                   const char * command,
                                   Print * display);

// Reset the system
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32MenuSystemReset(const struct _R4A_MENU_ENTRY * menuEntry,
                             const char * command,
                             Print * display);

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
// Heap support
//****************************************
// Display the heap
// Inputs:
//   display: Device used for output
void r4aEsp32HeapDisplay(Print * display = &Serial);

//****************************************
// HTTP API
//****************************************

extern const httpd_err_code_t r4aHttpError[];
extern const int r4aHttpErrorCount;
extern const char * r4aHttpErrorName[];

typedef struct _R4A_JPEG_CHUNKING_T
{
        httpd_req_t *req;
        size_t length;
} R4A_JPEG_CHUNKING_T;

typedef struct _R4A_TAG_NAME_T
{
    int tag;
    const char * name;
} R4A_TAG_NAME_T;

//****************************************
// ESP32 I2C Bus Class
//****************************************

class R4A_ESP32_I2C_BUS : public R4A_I2C_BUS
{
  public:

    // Create the R4A_ESP32_I2C object and select the TwoWire bus
    // Inputs:
    //   busNumber: Number of the I2C bus (0 - 2)
    //   deviceTable: Address of the table containing the address and device
    //                descriptions, may be nullptr
    //   deviceTableEntries: Number of entries in the I2C device table
    R4A_ESP32_I2C_BUS(int busNumber,
                      const R4A_I2C_DEVICE_DESCRIPTION * deviceTable,
                      int deviceTableEntries);

    // Delete the R4A_ESP32_I2C object
    ~R4A_ESP32_I2C_BUS();

    // Initialize the I2C bus
    // Inputs:
    //   sdaPin: Number of the pin used for the SDA signal
    //   sclPin: Number of the pin used for the SCL signal
    //   clockHz: Clock speed for the I2C bus in Hertz
    //   display: Device used for output
    void begin(int sdaPin,
               int sclPin,
               int clockHz,
               Print * display = &Serial);

    // Get the TwoWire pointer
    // Outputs:
    //   Returns the TwoWire object address
    TwoWire * getTwoWire();

    // Read data from an I2C peripheral
    // Inputs:
    //   deviceAddress: Device address on the I2C bus (0 - 0x7f)
    //   cmdBuffer: Address of the buffer containing the command bytes, may be nullptr
    //   cmdByteCount: Number of bytes to send from the command buffer
    //   dataBuffer: Address of the buffer to receive the data bytes, may be nullptr
    //   dataByteCount: Size in bytes of the data buffer, maximum receive bytes
    //   display: Device used for debug output
    //   releaseI2cBus: A value of true releases the I2C bus after the transaction
    // Outputs:
    //   Returns the number of bytes read
    size_t read(uint8_t deviceI2cAddress,
                const uint8_t * cmdBuffer, // Does not include I2C address
                size_t cmdByteCount,
                uint8_t * readBuffer,
                size_t readByteCount,
                Print * display = nullptr,
                bool releaseI2cBus = true);

  private:

    // Send data to an I2C peripheral, entered with the I2C bus lock held
    // Inputs:
    //   deviceAddress: Device address on the I2C bus (0 - 0x7f)
    //   cmdBuffer: Address of the buffer containing the command bytes, may be nullptr
    //   cmdByteCount: Number of bytes to send from the command buffer
    //   dataBuffer: Address of the buffer containing the data bytes, may be nullptr
    //   dataByteCount: Number of bytes to send from the data buffer
    //   display: Device used for debug output
    //   releaseI2cBus: A value of true releases the I2C bus after the transaction
    // Outputs:
    //   Returns true upon success, false otherwise
    bool writeWithLock(uint8_t deviceI2cAddress,
                       const uint8_t * cmdBuffer,
                       size_t cmdByteCount,
                       const uint8_t * dataBuffer,
                       size_t dataByteCount,
                       Print * display = nullptr,
                       bool releaseI2cBus = true);
};

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

extern const char * parameterFilePath; // Path to the parameter file
extern const R4A_ESP32_NVM_PARAMETER nvmParameters[];
extern const int nvmParameterCount;

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

// Get a set of parameters
// Inputs:
//   filePath: Address of the address of the path to the file contained in the NVM
//   display: Device used for output, may be nullptr
// Outputs:
//   Returns true if successful and false upon failure
bool r4aEsp32NvmGetParameters(const char ** filePath,
                              Print * display = nullptr);

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
// Outputs:
//   Returns true if successful and false upon failure
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
// Outputs:
//   Returns true if successful and false upon failure
bool r4aEsp32NvmWriteParameters(const char * filePath,
                                const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                int parametersCount,
                                Print * display = &Serial);

//****************************************
// NVM Menu API
//****************************************

extern const R4A_MENU_ENTRY r4aEsp32NvmMenuTable[]; // Menu table for NVM menu
#define R4A_ESP32_NVM_MENU_ENTRIES    8             // Length of NVM menu table

// Display all of the parameters
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32NvmMenuDisplayParameters(const struct _R4A_MENU_ENTRY * menuEntry,
                                      const char * command,
                                      Print * display);

// Dump the parameter file
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32NvmMenuDumpParameterFile(const struct _R4A_MENU_ENTRY * menuEntry,
                                      const char * command,
                                      Print * display);

// Get default parameters
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32NvmMenuGetDefaultParameters(const struct _R4A_MENU_ENTRY * menuEntry,
                                         const char * command,
                                         Print * display);

// Display the help text with PPP
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   align: Zero terminated string of spaces for alignment
//   display: Device used for output
[[deprecated("Use r4aMenuHelpSuffix instead.")]]
void r4aEsp32NvmMenuHelpPppp(const struct _R4A_MENU_ENTRY * menuEntry,
                             const char * align,
                             Print * display);

// Display the help text with PPPP XXXX
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   align: Zero terminated string of spaces for alignment
//   display: Device used for output
[[deprecated("Use r4aMenuHelpSuffix instead.")]]
void r4aEsp32NvmMenuHelpPpppXxxx(const struct _R4A_MENU_ENTRY * menuEntry,
                                 const char * align,
                                 Print * display);

// Clear the parameter
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32NvmMenuParameterClear(const struct _R4A_MENU_ENTRY * menuEntry,
                                   const char * command,
                                   Print * display);

// Display the parameter
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32NvmMenuParameterDisplay(const struct _R4A_MENU_ENTRY * menuEntry,
                                     const char * command,
                                     Print * display);

// Set the parameter value
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32NvmMenuParameterSet(const struct _R4A_MENU_ENTRY * menuEntry,
                                 const char * command,
                                 Print * display);

// Write a string to the parameter file
// Inputs:
//   file: File to which the string is written
//   string: Address of a zero terminated string of characters
// Outputs:
//   Returns true if all of the data was successfully written and false
//   upon error
bool r4aEsp32NvmWriteFileString(File &file, const char * string);

// Write the parameters to the parameter file
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32NvmMenuWriteParameterFile(const struct _R4A_MENU_ENTRY * menuEntry,
                                       const char * command,
                                       Print * display);

//****************************************
// OV2640 API
//****************************************

typedef struct _R4A_OV2640_PINS
{
    // Control pins
    int pinReset;
    int pinPowerDown;
    int pinXCLK;

    // I2C pins
    int pinI2cClk;
    int pinI2cData;

    // Frame synchronization
    int pinVSYNC;   // High at beginning of frame
    int pinHREF;    // High during each horizontal line
    int pinPCLK;    // Pixel clock

    // Image data pins
    int pinY2;
    int pinY3;
    int pinY4;
    int pinY5;
    int pinY6;
    int pinY7;
    int pinY8;
    int pinY9;
} R4A_OV2640_PINS;

// OV2640 class declaration
class R4A_OV2640
{
  private:

    const uint32_t _clockHz;        // Input clock frequency for the OV2640
    R4A_I2C_BUS * const _i2cBus;    // I2C bus to access the OV2640
    const uint8_t  _i2cAddress;     // Address of the OV2640
    const R4A_OV2640_PINS * const _pins; // ESP32 GPIO pins for the 0V2640 camera

  public:

    // Constructor
    // Inputs:
    //   i2cBus: R4A_I2C_BUS object address used to access the OV2640 camera
    //   i2cAddress: I2C address of the OV2640 camera
    //   pins: R4A_OV2640_PINS object containing the ESP32 GPIO pin numbers
    //   clockHz: OV2640 clock frequency input
    R4A_OV2640(R4A_I2C_BUS * i2cBus,
               int i2cAddress,
               const R4A_OV2640_PINS * pins,
               uint32_t clockHz)
        : _i2cBus{i2cBus}, _i2cAddress{i2cAddress & 0x7f}, _pins{pins},
          _clockHz{clockHz}
    {
    }

    // Display a group of registers
    // Inputs:
    //   firstRegister: The register address of the first register to be displayed
    //   bytesToRead: The number of registers to display
    //   display: Address of Print object for output
    void displayRegisters(uint8_t firstRegister,
                          size_t bytesToRead,
                          Print * display);

    // Dump all of the OV2640 registers in hexadecimal
    // Inputs:
    //   display: Address of Print object for output
    void dumpRegisters(Print * display);

    // Process the frame buffer
    // Inputs:
    //   frameBuffer: Buffer containing the raw image data
    //   display: Address of Print object for output
    // Outputs:
    //   Returns true if the processing was successful and false upon error
    virtual bool processFrameBuffer(camera_fb_t * frameBuffer,
                                    Print * display);

    // Process the web server's frame buffer
    // Inputs:
    //   frameBuffer: Buffer containing the raw image data
    // Outputs:
    //   Returns true if the processing was successful and false upon error
    virtual bool processWebServerFrameBuffer(camera_fb_t * frameBuffer);

    // Initialize the camera
    // Inputs:
    //   pixelFormat: Pixel format to use for the image
    //   display: Address of Print object for debug output, may be nullptr
    bool setup(pixformat_t pixelFormat, Print * display = nullptr);

    // Update the camera processing state
    // Inputs:
    //   display: Address of Print object for debug output, may be nullptr
    void update(Print * display = nullptr);
};

// Return a webpage to the requester containing a JPEG image
// Inputs:
//   request: Request from the browser
esp_err_t r4aOV2640JpegHandler(httpd_req_t *request);

extern bool r4aOv2640JpegDisplayTime;   // Set to true to display the JPEG conversion time
extern const R4A_OV2640_PINS r4aOV2640Pins; // ESP32 WRover camera pins

#define R4A_OV2640_JPEG_WEB_PAGE    "/jpeg"
extern const httpd_uri_t r4aOV2640JpegPage; // Page descriptor for the camera image page

//****************************************
// SPI API
//****************************************

class R4A_ESP32_SPI : public R4A_SPI
{
  private:

    uint32_t _div;
    spi_t * _spi;

  public:

    // Allocate DMA buffer
    // Inputs:
    //   length: Number of data bytes to allocate
    // Outputs:
    //   Returns the buffer address if successful and nullptr otherwise
    uint8_t * allocateDmaBuffer(int length)
    {
        return (uint8_t *)heap_caps_malloc(length, MALLOC_CAP_DMA);
    }

    // Initialize the SPI controller
    // Inputs:
    //   spiNumber: Number of the SPI controller
    //   pinMOSI: SPI TX data pin number
    //   clockHz: SPI clock frequency in Hertz
    // Outputs:
    //   Return true if successful and false upon failure
    bool begin(uint8_t spiNumber, uint8_t pinMOSI, uint32_t clockHz)
    {
        // Determine the SPI clock divider
        _div = spiFrequencyToClockDiv(clockHz);

        // Configure the SPI device
        _spi = spiStartBus(spiNumber, _div, SPI_MODE0, SPI_MSBFIRST);
        if (_spi)
        {
            // Connect the SPI TX output to the MOSI pin
            spiAttachMOSI(_spi, pinMOSI);
            return true;
        }
        return false;
    }

    // Transfer the data to the SPI device
    // Inputs:
    //   txBuffer: Address of the buffer containing the data to send
    //   rxBuffer: Address of the receive data buffer
    //   length: Number of data bytes in the buffer
    void transfer(const uint8_t * txBuffer, uint8_t * rxBuffer, uint32_t length)
    {
        spiTransferBytes(_spi, txBuffer, rxBuffer, length);
    }
};

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

//****************************************
// Web Server API
//****************************************

class R4A_WEB_SERVER
{
  private:

    const uint16_t _port;       // Port number for the web server

  protected:

    httpd_handle_t _webServer;  // HTTP server object

  public:

    // Constructor
    // Inputs:
    //   port: Port number for the web server
    R4A_WEB_SERVER(uint16_t port = 80) : _port{port}
    {
    }

    // Process the request error
    // Inputs:
    //   req: httpd_req_t object containing the request from the browser
    //   error: Error detected by the web server
    //   display: Address of Print object for debug output, may be nullptr
    // Outputs:
    //   Returns status indicating if the response was successfully sent
    //   to the browser
    esp_err_t error (httpd_req_t *req,
                     httpd_err_code_t error,
                     Print * display = nullptr);

    // Register the error handlers
    //   display: Address of Print object for debug output, may be nullptr
    // Outputs:
    //   Returns true if the all of the error handlers were installed and
    //   false upon failure
    virtual bool registerErrorHandlers(Print * display = nullptr);

    // Register the URI handlers
    //   display: Address of Print object for debug output, may be nullptr
    // Outputs:
    //   Returns true if the all of the error handlers were installed and
    //   false upon failure
    virtual bool registerUriHandlers(Print * display = nullptr);

    // Start the web server
    // Inputs:
    //   port: Port number to use for the web server
    //   display: Address of Print object for debug output, may be nullptr
    // Outputs:
    //   Returns true if the web server was successfully started and false
    //   upon failure
    bool start(uint16_t port, Print * display = nullptr);

    // Stop the web server
    // Inputs:
    //   display: Address of Print object for debug output, may be nullptr
    void stop(Print * display = nullptr);

    // Update the camera processing state
    // Inputs:
    //   wifiConnected: True when WiFi has an IP address and false otherwise
    //   display: Address of Print object for debug output, may be nullptr
    void update(bool wifiConnected, Print * display = &Serial);
};

// Handle the web server errors
// Inputs:
//   req: httpd_req_t object containing the request from the browser
//   error: Error detected by the web server
// Outputs:
//   Returns status indicating if the response was successfully sent
//   to the browser
esp_err_t r4aWebServerError (httpd_req_t *req, httpd_err_code_t error);

#endif  // __R4A_ESP32_H__
