/**********************************************************************
  R4A_I2C_Bus.ino

  Handle I2c transactions
**********************************************************************/

#include "R4A_ESP32.h"

//*********************************************************************
// Ping an I2C device and see if it responds
// Return true if device detected, false otherwise
bool r4aEsp32I2cBusIsDevicePresent(R4A_I2C * i2c, uint8_t deviceAddress)
{
    int status;

    // Single thread the I2C requests
    r4aEsp32LockAcquire(&i2c->lock);

    // Check for an I2C device
    i2c->bus->beginTransmission(deviceAddress);
    status = i2c->bus->endTransmission();

    // Release the lock
    r4aEsp32LockRelease(&i2c->lock);

    // Return the I2C device found status
    if (status == 0)
        return true;
    return false;
}

//*********************************************************************
// Enumerate the I2C bus
void r4aEsp32I2cBusEnumerate(R4A_I2C * i2c,
                             const R4A_I2C_DEVICE_DESCRIPTION * deviceTable,
                             int deviceTableEntries,
                             Print * display)
{
    bool deviceFound;
    int index;
    uint32_t timer;

    // Validate the device table
    if (!deviceTable)
    {
        display->println("ERROR: Please specify a table containing the I2C addresses and device names!");
        return;
    }
    if (!deviceTableEntries)
    {
        display->println("ERROR: There must be at least one entry in the table!");
        return;
    }

    // Display the device addresses
    deviceFound = false;
    for (uint8_t addr = 0; addr <= 0x7f; addr++)
    {
        timer = millis();
        if (r4aEsp32I2cBusIsDevicePresent(i2c, addr))
        {
            if (deviceFound == false)
            {
                display->println();
                display->println("I2C Devices:");
                deviceFound = true;
            }

            // Look up the display name
            for (index = 0; index < deviceTableEntries; index++)
                if (deviceTable[index].deviceAddress == addr)
                {
                    deviceFound = true;
                    break;
                }

            if (index < deviceTableEntries)
                display->printf("    0x%02x: %s\r\n", addr, deviceTable[index].displayName);
            else
                display->printf("    0x%02x: ???\r\n", addr);
        }
        else if ((millis() - timer) > 50)
        {
            display->println("ERROR: I2C bus not responding!");
            return;
        }
    }

    // Determine if any devices are on the bus
    if (!deviceFound)
        display->println("ERROR: No devices found on the I2C bus!");
}

//*********************************************************************
// Send data to an I2C peripheral
// Return true upon success, false otherwise
bool r4aEsp32I2cBusWriteWithLock(R4A_I2C * i2c,
                                 uint8_t deviceI2cAddress,
                                 const uint8_t * cmdBuffer,
                                 size_t cmdByteCount,
                                 const uint8_t * dataBuffer,
                                 size_t dataByteCount,
                                 bool debug,
                                 bool releaseI2cBus,
                                 Print * display)
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
        if (debug)
        {
            display->println("I2C Transaction");
            display->printf("    deviceI2cAddress: 0x%02x\r\n", deviceI2cAddress);
            display->printf("    cmdBuffer: %p\r\n", cmdBuffer);
            display->printf("    cmdByteCount: %d\r\n", cmdByteCount);
            display->printf("    dataBuffer: %p\r\n", dataBuffer);
            display->printf("    dataByteCount: %d\r\n", dataByteCount);
            if (cmdByteCount)
                r4aEsp32DumpBuffer((intptr_t)cmdBuffer, cmdBuffer, cmdByteCount, display);
            if (dataByteCount)
                r4aEsp32DumpBuffer((intptr_t)dataBuffer, dataBuffer, dataByteCount, display);
        }

        // Address the I2C device
        i2c->bus->beginTransmission(deviceI2cAddress);

        // Send the command to the device
        if (cmdByteCount)
        {
            cmdBytesWritten = i2c->bus->write(cmdBuffer, cmdByteCount);
            bytesWritten += cmdBytesWritten;
        }

        // Send the data to the device
        if (dataByteCount)
        {
            dataBytesWritten = i2c->bus->write(dataBuffer, dataByteCount);
            bytesWritten += dataBytesWritten;
        }

        // Done sending data to the I2C device
        if (bytesWritten == (cmdByteCount + dataByteCount))
            i2c->bus->endTransmission(releaseI2cBus);
        else
            i2c->bus->endTransmission();
    } while (0);

    // Display the I2C transaction results
    if (debug)
    {
        display->printf("    cmdBytesWritten: %d\r\n", cmdBytesWritten);
        display->printf("    dataBytesWritten: %d\r\n", dataBytesWritten);
        display->printf("    bytesWritten: %d\r\n", bytesWritten);
    }

    // Return the write status
    return (bytesWritten == (cmdByteCount + dataByteCount));
}

//*********************************************************************
// Send data to an I2C peripheral
// Return true upon success, false otherwise
bool r4aEsp32I2cBusWrite(R4A_I2C * i2c,
                         uint8_t deviceI2cAddress,
                         const uint8_t * cmdBuffer,
                         size_t cmdByteCount,
                         const uint8_t * dataBuffer,
                         size_t dataByteCount,
                         bool debug,
                         bool releaseI2cBus,
                         Print * display)
{
    bool status;

    // Single thread the I2C requests
    r4aEsp32LockAcquire(&i2c->lock);

    // Perform the I2C write operation
    status = r4aEsp32I2cBusWriteWithLock(i2c,
                                         deviceI2cAddress,
                                         cmdBuffer,
                                         cmdByteCount,
                                         dataBuffer,
                                         dataByteCount,
                                         debug,
                                         releaseI2cBus,
                                         display);

    // Release the lock
    r4aEsp32LockRelease(&i2c->lock);

    // Return the write status
    return status;
}

//*********************************************************************
// Read data from an I2C peripheral
// Return the number of bytes read
size_t r4aEsp32I2cBusRead(R4A_I2C * i2c,
                          uint8_t deviceI2cAddress,
                          const uint8_t * cmdBuffer, // Does not include I2C address
                          size_t cmdByteCount,
                          uint8_t * readBuffer,
                          size_t readByteCount,
                          bool debug,
                          bool releaseI2cBus,
                          Print * display)
{
    size_t bytesRead;

    do
    {
        // Assume read failure
        bytesRead = 0;

        // Single thread the I2C requests
        r4aEsp32LockAcquire(&i2c->lock);

        // Empty the I2C RX buffer
        i2c->bus->flush();

        // Address the I2C device
        if (cmdByteCount)
        {
            if (!r4aEsp32I2cBusWriteWithLock(i2c,
                                             deviceI2cAddress,
                                             cmdBuffer,   // Register address or other data
                                             cmdByteCount,
                                             nullptr,
                                             0,
                                             debug,
                                             false,
                                             display))
                break;
        }

        // Read the data from the I2C device into the I2C RX buffer
        bytesRead = i2c->bus->requestFrom(deviceI2cAddress, readByteCount);
        i2c->bus->endTransmission(releaseI2cBus);

        // Move the data into the read buffer
        for (size_t index = 0; index < bytesRead; index++)
            readBuffer[index] = i2c->bus->read();

        // Display the I2C transaction results
        if (debug)
        {
            display->printf("    bytesRead: %d\r\n", bytesRead);
            r4aEsp32DumpBuffer((intptr_t)readBuffer, readBuffer, bytesRead, display);
        }
    } while (0);

    // Release the lock
    r4aEsp32LockRelease(&i2c->lock);

    // Return the number of bytes read
    return bytesRead;
}

//*********************************************************************
// Initialize the I2C bus
void r4aEsp32I2cBusSetup(R4A_I2C * i2c,
                         int sdaPin,
                         int sclPin,
                         int clockHz,
                         const R4A_I2C_DEVICE_DESCRIPTION * deviceTable,
                         int deviceTableEntries,
                         Print * display)
{
    // Connect the pins to the I2C controller
    i2c->bus->begin(sdaPin, sclPin);
    i2c->bus->setClock(clockHz);

    // Reset the devices on the I2C bus
    i2c->bus->beginTransmission(I2C_GENERAL_CALL_DEVICE_ADDRESS);
    i2c->bus->write(I2C_SWRST);
    i2c->bus->endTransmission();

    // Delay while the I2C devices reset
    delay(10);

    // Enumerate the I2C devices
    if (deviceTable)
        r4aEsp32I2cBusEnumerate(i2c, deviceTable, deviceTableEntries, display);
}
