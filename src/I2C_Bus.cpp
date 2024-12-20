/**********************************************************************
  I2C_Bus.cpp

  Robots-For-All (R4A)
  I2C bus support for the ESP32
**********************************************************************/

#include "R4A_ESP32.h"

//*********************************************************************
// Initialize the I2C bus
void r4aEsp32I2cBusBegin(R4A_I2C_BUS * object,
                         int sdaPin,
                         int sclPin,
                         int clockHz,
                         Print * display)
{
    // Connect the pins to the I2C controller
    object->_i2cBus->begin(sdaPin, sclPin);
    object->_i2cBus->setClock(clockHz);

    // Reset the devices on the I2C bus
    object->_i2cBus->beginTransmission(R4A_I2C_GENERAL_CALL_DEVICE_ADDRESS);
    object->_i2cBus->write(R4A_I2C_SWRST);
    object->_i2cBus->endTransmission();

    // Delay while the I2C devices reset
    delay(10);

    // Enumerate the I2C devices
    if (display)
        r4aI2cBusEnumerate(object, display);
}

//*********************************************************************
// Read data from an I2C peripheral
// Return the number of bytes read
size_t r4aEsp32I2cBusRead(R4A_I2C_BUS * object,
                          uint8_t deviceI2cAddress,
                          const uint8_t * cmdBuffer, // Does not include I2C address
                          size_t cmdByteCount,
                          uint8_t * readBuffer,
                          size_t readByteCount,
                          Print * display,
                          bool releaseI2cBus)
{
    size_t bytesRead;

    do
    {
        // Assume read failure
        bytesRead = 0;

        // Single thread the I2C requests
        r4aLockAcquire(&object->_lock);

        // Empty the I2C RX buffer
        object->_i2cBus->flush();

        // Address the I2C device
        if (cmdByteCount)
        {
            if (!r4aEsp32I2cBusWriteWithLock(object,
                                             deviceI2cAddress,
                                             cmdBuffer,   // Register address or other data
                                             cmdByteCount,
                                             nullptr,
                                             0,
                                             display,
                                             false))
                break;
        }

        // Read the data from the I2C device into the I2C RX buffer
        bytesRead = object->_i2cBus->requestFrom(deviceI2cAddress, readByteCount);
        object->_i2cBus->endTransmission(releaseI2cBus);

        // Move the data into the read buffer
        for (size_t index = 0; index < bytesRead; index++)
            readBuffer[index] = object->_i2cBus->read();

        // Display the I2C transaction results
        if (display)
        {
            display->printf("    bytesRead: %d\r\n", bytesRead);
            r4aDumpBuffer((intptr_t)readBuffer, readBuffer, bytesRead, display);
        }
    } while (0);

    // Release the lock
    r4aLockRelease(&object->_lock);

    // Return the number of bytes read
    return bytesRead;
}

//*********************************************************************
// Send data to an I2C peripheral
// Return true upon success, false otherwise
bool r4aEsp32I2cBusWriteWithLock(R4A_I2C_BUS * object,
                                 uint8_t deviceI2cAddress,
                                 const uint8_t * cmdBuffer,
                                 size_t cmdByteCount,
                                 const uint8_t * dataBuffer,
                                 size_t dataByteCount,
                                 Print * display,
                                 bool releaseI2cBus)
{
    size_t bytesWritten;
    size_t cmdBytesWritten;
    size_t dataBytesWritten;

    do
    {
        // Assume write failure
        bytesWritten = 0;
        cmdBytesWritten = 0;
        dataBytesWritten = 0;

        // Display the I2C transaction request
        if (display)
        {
            display->println("I2C Transaction");
            display->printf("    deviceI2cAddress: 0x%02x\r\n", deviceI2cAddress);
            display->printf("    cmdBuffer: %p\r\n", cmdBuffer);
            display->printf("    cmdByteCount: %d\r\n", cmdByteCount);
            display->printf("    dataBuffer: %p\r\n", dataBuffer);
            display->printf("    dataByteCount: %d\r\n", dataByteCount);
            if (cmdByteCount)
                r4aDumpBuffer((intptr_t)cmdBuffer, cmdBuffer, cmdByteCount, display);
            if (dataByteCount)
                r4aDumpBuffer((intptr_t)dataBuffer, dataBuffer, dataByteCount, display);
        }

        // Address the I2C device
        object->_i2cBus->beginTransmission(deviceI2cAddress);

        // Send the command to the device
        if (cmdByteCount)
        {
            cmdBytesWritten = object->_i2cBus->write(cmdBuffer, cmdByteCount);
            bytesWritten += cmdBytesWritten;
        }

        // Send the data to the device
        if (dataByteCount)
        {
            dataBytesWritten = object->_i2cBus->write(dataBuffer, dataByteCount);
            bytesWritten += dataBytesWritten;
        }

        // Done sending data to the I2C device
        if (bytesWritten == (cmdByteCount + dataByteCount))
            object->_i2cBus->endTransmission(releaseI2cBus);
        else
            object->_i2cBus->endTransmission();
    } while (0);

    // Display the I2C transaction results
    if (display)
    {
        display->printf("    cmdBytesWritten: %d\r\n", cmdBytesWritten);
        display->printf("    dataBytesWritten: %d\r\n", dataBytesWritten);
        display->printf("    bytesWritten: %d\r\n", bytesWritten);
    }

    // Return the write status
    return (bytesWritten == (cmdByteCount + dataByteCount));
}
