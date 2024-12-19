/**********************************************************************
  OV2640.cpp

  Robots-For-All (R4A)
  SPI support
**********************************************************************/

#include "R4A_ESP32.h"

//*********************************************************************
// Constructor
R4A_ESP32_SPI::R4A_ESP32_SPI() : _div{0}, _spi{nullptr}
{
}

//*********************************************************************
// Allocate DMA buffer
// Inputs:
//   length: Number of data bytes to allocate
// Outputs:
//   Returns the buffer address if successful and nullptr otherwise
uint8_t * R4A_ESP32_SPI::allocateDmaBuffer(int length)
{
    return (uint8_t *)heap_caps_malloc(length, MALLOC_CAP_DMA);
}

//*********************************************************************
// Initialize the SPI controller
// Inputs:
//   spiNumber: Number of the SPI controller
//   pinMOSI: SPI TX data pin number
//   clockHz: SPI clock frequency in Hertz
// Outputs:
//   Return true if successful and false upon failure
bool R4A_ESP32_SPI::begin(uint8_t spiNumber,
                          uint8_t pinMOSI,
                          uint32_t clockHz)
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

//*********************************************************************
// Transfer the data to the SPI device
// Inputs:
//   txBuffer: Address of the buffer containing the data to send
//   rxBuffer: Address of the receive data buffer
//   length: Number of data bytes in the buffer
void R4A_ESP32_SPI::transfer(const uint8_t * txBuffer,
                             uint8_t * rxBuffer,
                             uint32_t length)
{
    spiTransferBytes(_spi, txBuffer, rxBuffer, length);
}
