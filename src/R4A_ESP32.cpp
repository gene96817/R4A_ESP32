/**********************************************************************
  RSA_ESP32.cpp

  ESP32 WROVER module support
**********************************************************************/

//*********************************************************************
// Display the battery voltage
void r4aEsp32DisplayBatteryVoltage(Print * display)
{
    float voltage;
    int adcValue;

    // Get the battery voltage
    voltage = r4aEsp32GetBatteryVoltage(&adcValue);

    // Display the battery voltage
    if (adcValue < 5)
        display->printf("Power switch is off (0x%04x)!\r\n", adcValue);
    else
        display->printf("Battery Voltage (%d, 0x%04x): %.2f Volts\r\n",
                       adcValue, adcValue, voltage);
}

//*********************************************************************
// Flash the ESP32 WROVER blue LED
void r4aEsp32FlashBlueLed()
{
    int blueLED = digitalRead(BLUE_LED_BUZZER_PIN);
    digitalWrite(BLUE_LED_BUZZER_PIN, !blueLED);
    delay(250);
    digitalWrite(BLUE_LED_BUZZER_PIN, blueLED);
}

//*********************************************************************
// Get the battery voltage
float r4aEsp32GetBatteryVoltage(int * adcValue)
{
    float batteryVoltage;
    uint32_t tempFunc32;
    int voltage;

    // Bug: No WS2812 output
    //      The WS2812 code uses the RMT (remote control peripheral) and
    //      the GPIO mux is making the connection.  The pinMode call below
    //      switches the GPIO mux for pin 32 from the RMT to the GPIO
    //      controller.  However setting the pinMode back to output does
    //      not restore the GPIO mux value.
    //
    // Fix: Share the pin between battery voltage input and WS2812 output
    //      Save and restore the GPIO mux value.
    //
    // Remember the GPIO pin mux value
    tempFunc32 = r4aGpioRegs->R4A_GPIO_FUNC_OUT_SEL_CFG_REG[BATTERY_WS2812_PIN];

    // Switch from RMT output for the WS2812 to GPIO input for ADC
    pinMode(BATTERY_WS2812_PIN, INPUT);

    // Read the voltage multiple times and take the average
    voltage = 0;
    for (int index = 0; index < 8; index++)
        voltage += analogRead(BATTERY_WS2812_PIN);
    voltage >>= 3;

    // Restore the GPIO pin to an output for the WS2812 and reconnect the
    // pin to the RMT (remote control peripheral)
    pinMode(BATTERY_WS2812_PIN, OUTPUT);
    r4aGpioRegs->R4A_GPIO_FUNC_OUT_SEL_CFG_REG[BATTERY_WS2812_PIN] = tempFunc32;

    // Return the ADC value
    if (adcValue)
        *adcValue = voltage;

    // Convert the ADC reading into a voltage value
    batteryVoltage = (ADC_REFERENCE_VOLTAGE
                   * (voltage * BATTERY_VOLTAGE_MULTIPLIER))
                   / 4095.;
    return batteryVoltage;
}

//*********************************************************************
// Determine if the address is in the EEPROM (Flash)
bool r4aEsp32IsAddressInEEPROM(void * addr)
{
    return ((addr >= (void *)0x3f400000) && (addr <= (void *)0x3f7fffff));
}

//*********************************************************************
// Determine if the address is in PSRAM (SPI RAM)
bool r4aEsp32IsAddressInPSRAM(void * addr)
{
    return ((addr >= (void *)0x3f800000) && (addr <= (void *)0x3fbfffff));
}

//*********************************************************************
// Determine if the address is in PSRAM or SRAM
bool r4aEsp32IsAddressInRAM(void * addr)
{
    return r4aEsp32IsAddressInSRAM(addr) || r4aEsp32IsAddressInPSRAM(addr);
}

//*********************************************************************
bool r4aEsp32IsAddressInROM(void * addr)
{
    return (((addr >= (void *)0x3ff90000) && (addr <= (void *)0x3ff9ffff))
        ||  ((addr >= (void *)0x40000000) && (addr <= (void *)0x4005ffff)));
}

//*********************************************************************
// Determine if the address is in SRAM
bool r4aEsp32IsAddressInSRAM(void * addr)
{
    return ((addr >= (void *)0x3ffae000) && (addr <= (void *)0x3ffdffff));
}

//*********************************************************************
// Take out a lock
void r4aEsp32LockAcquire(volatile int * lock)
{
    uint32_t expected;

    // Single thread access to the lock
    expected = 0;
    while (!__atomic_compare_exchange_4(lock,
                                        &expected,
                                        xPortGetCoreID() + 1,
                                        false,
                                        __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED))
    {
        expected = 0;
    }
}

//*********************************************************************
// Take out a lock
void r4aEsp32LockRelease(volatile int * lock)
{
    *lock = 0;
}

//*********************************************************************
// Find the specified partition
bool r4aEsp32PartitionFind(const char * name)
{
    // Initialize the partition list iterator
    esp_partition_iterator_t pi = esp_partition_find(ESP_PARTITION_TYPE_ANY,
                                                     ESP_PARTITION_SUBTYPE_ANY,
                                                     NULL);

    // Determine if any partitions exist in the list
    if (pi != NULL)
    {
        // Walk through the list of partitions
        do
        {
            // Get the partition data
            const esp_partition_t *p = esp_partition_get(pi);

            // Determine if the requested partition was found
            if (strcmp(p->label, name) == 0)
                // The partition was found
                return true;

            // Get the next partition in the list
        } while ((pi = (esp_partition_next(pi))));
    }

    // The spiffs partition does not exist
    return false;
}

//*********************************************************************
// Display the partition table
void r4aEsp32PartitionTableDisplay(Print * display)
{
    // Display the table header
    display->println(".-----------------------------------------------------.");
    display->println("|                ESP32 Partition table                |");
    display->println("|-----------------------------------------------------|");
    display->println("| Type | Sub |  Offset  |   Size   |       Label      |");
    display->println("| ---- | --- | -------- | -------- | ---------------- |");

    // Initialize the partition list iterator
    esp_partition_iterator_t pi = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);

    // Determine if any partitions exist in the list
    if (pi != NULL)
    {
        // Walk through the list of partitions
        do
        {
            // Get the partition data
            const esp_partition_t *p = esp_partition_get(pi);

            // Display the partition data
            display->printf("|  %02x  | %02x  | 0x%06X | 0x%06X | %-16s |\r\n",
                            p->type,
                            p->subtype,
                            p->address,
                            p->size,
                            p->label);

            // Get the next partition in the list
        } while ((pi = (esp_partition_next(pi))));
    }

    // Display the table footer
    display->println("'-----------------------------------------------------'");
}

//*********************************************************************
// Read a command from USB serial
void r4aEsp32UsbSerialUpdate()
{
    static String command = "";
    char data;

    // Wait for a command
    while (Serial.available())
    {
        // Get the command
        data = Serial.read();
        if ((data != '\r') && (data != '\n'))
        {
            // Handle backspace
            if (data == 8)
            {
                if (command.length() <= 0)
                    Serial.write(7);
                else
                {
                    // Remove the character from the line
                    Serial.write(data);
                    Serial.write(' ');
                    Serial.write(data);

                    // Remove the character from the command
                    command = command.substring(0, command.length() - 1);
                }
            }
            else
            {
                // Echo the character
                Serial.write(data);

                // Add the character to the command line
                command += data;
            }
        }
        else if (data == '\r')
        {
            // Echo the carriage return
            Serial.println();

            // Start at the main menu if necessary
            if (!wcsMenu)
                wcsMenu = wcsMainMenu;

            // Process the command
            wcsMenu(command.c_str(), &Serial);
            command = "";

            // Display the menu
            if (wcsMenu)
                wcsMenu(nullptr, &Serial);
        }
    }
}
