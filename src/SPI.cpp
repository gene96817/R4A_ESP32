/**********************************************************************
  SPI.cpp

  Robots-For-All (R4A)
  SPI support
**********************************************************************/

#include "R4A_ESP32.h"

//*********************************************************************
// Initialize the SPI controller
// Inputs:
//   spi: Address of an R4A_ESP32_SPI data structure
//   spiNumber: Number of the SPI controller
//   pinMOSI: SPI TX data pin number
//   clockHz: SPI clock frequency in Hertz
// Outputs:
//   Return true if successful and false upon failure
bool r4aEsp32SpiBegin(struct _R4A_ESP32_SPI * spi,
                      uint8_t spiNumber,
                      uint8_t pinMOSI,
                      uint32_t clockHz)
{
    // Determine the SPI clock divider
    spi->_div = spiFrequencyToClockDiv(clockHz);

    // Configure the SPI device
    spi->_spi = spiStartBus(spiNumber, spi->_div, SPI_MODE0, SPI_MSBFIRST);
    if (spi->_spi)
    {
        // Connect the SPI TX output to the MOSI pin
        spiAttachMOSI(spi->_spi, pinMOSI);

        // Set the routine addresses
        spi->spi.allocateDmaBuffer = r4aEsp32AllocateDmaBuffer;
        spi->spi.transfer = r4aEsp32SpiTransfer;
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
void r4aEsp32SpiTransfer(struct _R4A_SPI * spi,
                         const uint8_t * txBuffer,
                         uint8_t * rxBuffer,
                         uint32_t length)
{
    spiTransferBytes(((R4A_ESP32_SPI *)spi)->_spi, txBuffer, rxBuffer, length);
}
