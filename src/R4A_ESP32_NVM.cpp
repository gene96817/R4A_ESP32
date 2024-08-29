/**********************************************************************
  R4A_ESP32_NVM.ino

  Load data from and store data to non-volatile memory (NVM).
**********************************************************************/

#include "R4A_ESP32.h"

//*********************************************************************
// Support routines
//*********************************************************************
// Parse the value parameter
// Inputs:
//   parameter: Address of the specified parameter in the table
//   valueString: Character string containing the new value
//   value: Address to receive the new value
//   display: Device used for output
// Outputs:
//   Returns true if the value was set and false otherwise
bool r4aEsp32NvmParseValue(const R4A_ESP32_NVM_PARAMETER * parameter,
                           const char * valueString,
                           R4A_ESP32_NVM_VALUE * value,
                           Print * display)
{
    int length;
    int maximum;
    int minimum;
    char * newValue;
    bool valid;

    // Determine the parameter type
    valid = false;
    value->u64 = 0;
    switch (parameter->type)
    {
    default:
        display->printf("ERROR: Unknown parameter type (%d) for %s\r\n", parameter->type, parameter->name);
        r4aEsp32ReportFatalError("r4aEsp32NvmParseValue: Invalid parameter type!");
        break;

    case R4A_ESP32_NVM_PT_BOOL:
    case R4A_ESP32_NVM_PT_INT8:
    case R4A_ESP32_NVM_PT_UINT8:
    case R4A_ESP32_NVM_PT_INT16:
    case R4A_ESP32_NVM_PT_UINT16:
    case R4A_ESP32_NVM_PT_INT32:
    case R4A_ESP32_NVM_PT_UINT32:
    case R4A_ESP32_NVM_PT_INT64:
    case R4A_ESP32_NVM_PT_UINT64:
        // Convert the string into a numeric value
        valid = ((sscanf(valueString, "0x%llx", &value->u64) == 1)
                || (sscanf(valueString, "%llu", &value->u64) == 1));
        if (valid)
        {
            // Validate the value
            switch (parameter->type)
            {
            case R4A_ESP32_NVM_PT_BOOL:
                valid = (value->u64 >= parameter->minimum) && (value->u64 <= parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_INT8:
                valid = (value->i64 >= (int64_t)parameter->minimum) && (value->i64 <= (int64_t)parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_UINT8:
                valid = (value->u64 >= parameter->minimum) && (value->u64 <= parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_INT16:
                valid = (value->i64 >= (int64_t)parameter->minimum) && (value->i64 <= (int64_t)parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_UINT16:
                valid = (value->u64 >= parameter->minimum) && (value->u64 <= parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_INT32:
                valid = (value->i64 >= (int64_t)parameter->minimum) && (value->i64 <= (int64_t)parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_UINT32:
                valid = (value->u64 >= parameter->minimum) && (value->u64 <= parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_INT64:
                valid = (value->i64 >= (int64_t)parameter->minimum) && (value->i64 <= (int64_t)parameter->maximum);
                break;

            case R4A_ESP32_NVM_PT_UINT64:
                valid = (value->u64 >= parameter->minimum) && (value->u64 <= parameter->maximum);
                break;
            }
        }
        break;

    case R4A_ESP32_NVM_PT_FLOAT:
    case R4A_ESP32_NVM_PT_DOUBLE:
        // Convert the string into a numeric value
        valid = (sscanf(valueString, "%llu", &value->u64) == 1);
        if (valid)
        {
            // Validate the value
            value->d = (double)value->i64 / R4A_ESP32_NVM_FLOAT_CONV;
            valid = false;
            switch (parameter->type)
            {
            case R4A_ESP32_NVM_PT_FLOAT:
            case R4A_ESP32_NVM_PT_DOUBLE:
                // Store the value as a double in the value structure
                maximum = ((double)parameter->maximum) / R4A_ESP32_NVM_FLOAT_CONV;
                minimum = ((double)parameter->minimum) / R4A_ESP32_NVM_FLOAT_CONV;
                valid = (value->d >= minimum) && (value->d <= maximum);
                break;
           }
        }
        break;

    case R4A_ESP32_NVM_PT_NULLPTR:
        newValue = nullptr;
        valid = true;
        break;

    case R4A_ESP32_NVM_PT_P_CHAR:
        length = strlen(valueString) + 1;
        newValue = (char *)malloc(length);
        if (!newValue)
            display->println("ERROR: Failed to allocate parameter value string!");
        else
        {
            strcpy(newValue, valueString);
            value->pcc = newValue;
            valid = true;
        }
        break;
    }

    // Return the valid status
    return valid;
}

//*********************************************************************
// Set the parameter value
// Inputs:
//   parameter: Address of the specified parameter in the table
//   data: New parameter value represented as a 64-bit value
//         binary for values, address for strings
void r4aEsp32NvmSetParameterValue(const R4A_ESP32_NVM_PARAMETER * parameter,
                                  uint64_t data)
{
    void * dataAddress;
    R4A_ESP32_NVM_VALUE value;

    // Determine the parameter type
    value.u64 = data;
    switch (parameter->type)
    {
    default:
        Serial.printf("ERROR: Invalid parameter type: %d\r\n", parameter->type);
        r4aEsp32ReportFatalError("nvmSetParameterValue: Invalid parameter type!");
        break;

    case R4A_ESP32_NVM_PT_BOOL:
        *(bool *)(parameter->addr) = (bool)value.u64;
        break;

    case R4A_ESP32_NVM_PT_INT8:
        *(int8_t *)(parameter->addr) = (int8_t)(value.i64);
        break;

    case R4A_ESP32_NVM_PT_UINT8:
        *(uint8_t *)(parameter->addr) = (uint8_t)value.u64;
        break;

    case R4A_ESP32_NVM_PT_INT16:
        *(int16_t *)(parameter->addr) = (int16_t)value.i64;
        break;

    case R4A_ESP32_NVM_PT_UINT16:
        *(uint16_t *)(parameter->addr) = (uint16_t)value.u64;
        break;

    case R4A_ESP32_NVM_PT_INT32:
        *(int32_t *)(parameter->addr) = (int32_t)value.i64;
        break;

    case R4A_ESP32_NVM_PT_UINT32:
        *(uint32_t *)(parameter->addr) = (uint32_t)value.u64;
        break;

    case R4A_ESP32_NVM_PT_INT64:
    case R4A_ESP32_NVM_PT_UINT64:
        *(int64_t *)(parameter->addr) = value.i64;
        break;

    case R4A_ESP32_NVM_PT_FLOAT:
        // Float is represented as a double value in the value structure
        *(float *)(parameter->addr) = (float)value.d;
        break;

    case R4A_ESP32_NVM_PT_DOUBLE:
        // Double is represented as a double value in the value structure
        *(double *)(parameter->addr) = value.d;
        break;

    case R4A_ESP32_NVM_PT_NULLPTR:
        // Free the previously allocated value
        dataAddress = *(char **)(parameter->addr);
        if (dataAddress && (r4aEsp32IsAddressInRAM(dataAddress)))
            free(dataAddress);
        *(const char **)(parameter->addr) = nullptr;
        break;

    case R4A_ESP32_NVM_PT_P_CHAR:
        // Free the previously allocated value
        dataAddress = *(char **)(parameter->addr);
        if (dataAddress && (r4aEsp32IsAddressInRAM(dataAddress)))
            free(dataAddress);
        *(const char **)(parameter->addr) = value.pcc;
        break;
    }
}

//*********************************************************************
// Write a string to the parameter file
bool r4aEsp32NvmWriteParameterValue(File &parameterFile, const char * string)
{
    size_t bytesWritten;
    size_t length;

    length = strlen(string) + 1;
    bytesWritten = parameterFile.write((const uint8_t *)string, length);
    return (bytesWritten == length);
}

//*********************************************************************
// Write the parameter value
bool r4aEsp32NvmWriteParameterValue(File &parameterFile,
                                    const R4A_ESP32_NVM_PARAMETER * parameter,
                                    Print * display)
{
    const char * data;
    uint8_t type;
    char typeString[3 + 1];
    char valueString[32];
    bool valid;
    R4A_ESP32_NVM_VALUE value;

    // Determine the parameter type
    type = parameter->type;
    value.u64 = 0;
    switch (type)
    {
    default:
        Serial.printf("ERROR: Invalid parameter type: %d\r\n", parameter->type);
        r4aEsp32ReportFatalError("r4aEsp32NvmWriteParameterValue: Invalid parameter type!");
        break;

    case R4A_ESP32_NVM_PT_BOOL:
        value.b = *(bool *)(parameter->addr);
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_INT8:
        value.i64 = (int64_t)(*(int8_t *)(parameter->addr));
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_UINT8:
        value.u64 = (uint64_t)(*(uint8_t *)(parameter->addr));
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_INT16:
        value.i64 = (int64_t)(*(int16_t *)(parameter->addr));
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_UINT16:
        value.u16 = (uint64_t)(*(uint16_t *)(parameter->addr));
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_INT32:
        value.i64 = (int64_t)(*(int32_t *)(parameter->addr));
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_UINT32:
        value.u32 = (uint64_t)(*(uint32_t *)(parameter->addr));
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_UINT64:
    case R4A_ESP32_NVM_PT_INT64:
        value.u64 = *(uint64_t *)(parameter->addr);
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_FLOAT:
        value.d = ((double)(*(float *)(parameter->addr))) * R4A_ESP32_NVM_FLOAT_CONV;
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_DOUBLE:
        value.d = (*(double *)(parameter->addr)) * R4A_ESP32_NVM_FLOAT_CONV;
        sprintf(valueString, "0x%016llx", value.u64);
        data = valueString;
        break;

    case R4A_ESP32_NVM_PT_NULLPTR:
        data = "nullptr";
        type = R4A_ESP32_NVM_PT_NULLPTR;
        break;

    case R4A_ESP32_NVM_PT_P_CHAR:
        value.pcc = *(const char **)(parameter->addr);
        if (value.pcc)
            data = value.pcc;
        else
        {
            data = "nullptr";
            type = R4A_ESP32_NVM_PT_NULLPTR;
        }
        break;
    }

    // Write the parameter to the file as a triplet of three strings:
    //
    //  +-------+-------+-------+
    //  | Name  | Type  | Value |
    //  +-------+-------+-------+
    sprintf(typeString, "%d", type);
    valid = (r4aEsp32NvmWriteParameterValue(parameterFile, parameter->name)
        && r4aEsp32NvmWriteParameterValue(parameterFile, typeString)
        && r4aEsp32NvmWriteParameterValue(parameterFile, data));
    return valid;
}

//*********************************************************************
// NVM API routines
//*********************************************************************
// Display a parameter
void r4aEsp32NvmDisplayParameter(const R4A_ESP32_NVM_PARAMETER * parameter,
                                 Print * display)
{
    R4A_ESP32_NVM_VALUE value;

    // Determine the parameter type
    switch (parameter->type)
    {
    default:
        display->printf("ERROR: Invalid parameter type: %d\r\n", parameter->type);
        r4aEsp32ReportFatalError("nvmDisplayParameter: Invalid parameter type!");
        break;

    case R4A_ESP32_NVM_PT_BOOL:
        value.b = *(bool *)(parameter->addr);
        display->printf("%s: %s\r\n", parameter->name, value.b ? "true" : "false");
        break;

    case R4A_ESP32_NVM_PT_INT8:
        value.i8 = *(int8_t *)(parameter->addr);
        display->printf("%s: %d\r\n", parameter->name, value.i8);
        break;

    case R4A_ESP32_NVM_PT_UINT8:
        value.u8 = *(uint8_t *)(parameter->addr);
        display->printf("%s: %u\r\n", parameter->name, value.u8);
        break;

    case R4A_ESP32_NVM_PT_INT16:
        value.i16 = *(int16_t *)(parameter->addr);
        display->printf("%s: %d\r\n", parameter->name, value.i16);
        break;

    case R4A_ESP32_NVM_PT_UINT16:
        value.u16 = *(uint16_t *)(parameter->addr);
        display->printf("%s: %u\r\n", parameter->name, value.u16);
        break;

    case R4A_ESP32_NVM_PT_INT32:
        value.i32 = *(int32_t *)(parameter->addr);
        display->printf("%s: %d\r\n", parameter->name, value.i32);
        break;

    case R4A_ESP32_NVM_PT_UINT32:
        value.u32 = *(uint32_t *)(parameter->addr);
        display->printf("%s: %u\r\n", parameter->name, value.u32);
        break;

    case R4A_ESP32_NVM_PT_INT64:
        value.i64 = *(int64_t *)(parameter->addr);
        display->printf("%s: %lld\r\n", parameter->name, value.i64);
        break;

    case R4A_ESP32_NVM_PT_UINT64:
        value.u64 = *(uint64_t *)(parameter->addr);
        display->printf("%s: %llu\r\n", parameter->name, value.u64);
        break;

    case R4A_ESP32_NVM_PT_FLOAT:
        value.d = (double)(*(float *)(parameter->addr));
        display->printf("%s: %f\r\n", parameter->name, (float)value.d);
        break;

    case R4A_ESP32_NVM_PT_DOUBLE:
        value.d = *(double *)(parameter->addr);
        display->printf("%s: %f\r\n", parameter->name, value.d);
        break;

    case R4A_ESP32_NVM_PT_NULLPTR:
        value.pv = *(void **)(parameter->addr);
        if (value.pv)
            display->printf("%s: %p\r\n", parameter->name, value.pv);
        else
            display->printf("%s: nullptr\r\n", parameter->name);
        break;

    case R4A_ESP32_NVM_PT_P_CHAR:
        value.pcc = *(const char **)(parameter->addr);
        if (value.pcc)
            display->printf("%s: \"%s\"\r\n", parameter->name, value.pcc);
        else
            display->printf("%s: nullptr\r\n", parameter->name);
        break;
    }
}

//*********************************************************************
// Display the parameters
void r4aEsp32NvmDisplayParameters(const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                  int parameterCount,
                                  Print * display)
{
    // Walk the list of parameters
    for (int parameter = 0; parameter < parameterCount; parameter++)
        r4aEsp32NvmDisplayParameter(&parameterTable[parameter], display);
}

//*********************************************************************
// Dump the parameter file
void r4aEsp32NvmDumpParameterFile(const char * filePath, Print * display)
{
    size_t bytesRead;
    size_t fileBytes;
    File parameterFile;
    uint8_t * nvmData;

    nvmData = nullptr;
    do
    {
        // Determine if the file exists
        if (!LittleFS.exists(filePath))
        {
            display->printf("Parameter file %s not found!\r\n", filePath);
            break;
        }

        // Open the parameter file
        parameterFile = LittleFS.open(filePath, "r");
        if (!parameterFile)
        {
            display->printf("ERROR: Failed to open file %s!\r\n", filePath);
            break;
        }

        // Determine the file size
        fileBytes = parameterFile.size();
        if (!fileBytes)
        {
            display->println("Empty parameter file!");
            break;
        }

        // Allocate the file data buffer
        nvmData = (uint8_t *)malloc(fileBytes);
        if (!nvmData)
        {
            display->println("ERROR: Failed to allocate read buffer!");
            break;
        }

        // Read the file into memory
        bytesRead = parameterFile.read(nvmData, fileBytes);
        if (bytesRead != fileBytes)
        {
            display->println("ERROR: Failed to read file into memory!");
            break;
        }

        r4aDumpBuffer(0, nvmData, bytesRead, display);
    } while (0);

    // Free the NVM data
    if (nvmData)
        free(nvmData);

    // Close the file
    if (parameterFile)
        parameterFile.close();
}

//*********************************************************************
// Get the default set of parameters
void r4aEsp32NvmGetDefaultParameters(const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                     int parameterCount)
{
    const R4A_ESP32_NVM_PARAMETER * parameter;
    R4A_ESP32_NVM_VALUE value;

    // Walk the list of parameters
    for (int index = 0; index < parameterCount; index++)
    {
        // Get the default parameter value
        parameter = &parameterTable[index];
        value.u64 = parameter->value;
        switch (parameter->type)
        {
        // The default float and double parameter values are multiplied
        // by NVM_FLOAT_CONV before being stored in the parameter table.
        // Undo this conversion and restore the float or double value.
        case R4A_ESP32_NVM_PT_FLOAT:
            value.u64 = parameter->value;
            value.d = (double)value.u64 / R4A_ESP32_NVM_FLOAT_CONV;
            // Both float and double values are represented as doubles in
            // the value structure
            break;

        case R4A_ESP32_NVM_PT_DOUBLE:
            value.u64 = parameter->value;
            value.d = (double)value.u64 / R4A_ESP32_NVM_FLOAT_CONV;
            // Both float and double values are represented as doubles in
            // the value structure
            break;
        }
        r4aEsp32NvmSetParameterValue(parameter, value.u64);
    }
}

//*********************************************************************
// Get a string from the parameter file
bool r4aEsp32NvmGetString(const char ** string, const char ** data, size_t * fileBytes)
{
    bool stringValid;
    char value;

    // Save the string address
    *string = *data;

    // Verify that the file contains a string
    stringValid = false;
    while (*fileBytes)
    {
        // Consume this file byte
        *fileBytes -= 1;
        value = **data;
        *data += 1;

        // Check for end-of-string (zero)
        if (!value)
        {
            stringValid = true;
            break;
        }
    }

    // Return the string found and is zero terminated
    return stringValid;
}

//*********************************************************************
// Clear a parameter by setting its value to zero
void r4aEsp32NvmParameterClear(const char * filePath,
                               const R4A_ESP32_NVM_PARAMETER * parameterTable,
                               int parameterCount,
                               const char * name,
                               Print * display)
{
    const R4A_ESP32_NVM_PARAMETER * parameter;

    // Lookup the parameter
    parameter = r4aEsp32NvmParameterLookup(parameterTable, parameterCount, name, display);
    if (!parameter)
        // Unknown parameter
        display->printf("WARNING: %s is not a parameter!\r\n", name);
    else
    {
        // Clear the parameter
        r4aEsp32NvmSetParameterValue(parameter, 0);

        // Write the parameters to the file
        r4aEsp32NvmWriteParameters(filePath,
                                   parameterTable,
                                   parameterCount,
                                   display);
    }
}

//*********************************************************************
// Look up a parameter by name
const R4A_ESP32_NVM_PARAMETER * r4aEsp32NvmParameterLookup(const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                                           int parameterCount,
                                                           const char * name,
                                                           Print * display)
{
    int index;
    const R4A_ESP32_NVM_PARAMETER * parameter;

    // Look up the parameter
    parameter = nullptr;
    for (index = 0; index < parameterCount; index++)
    {
        if (r4aEsp32Stricmp(parameterTable[index].name, name) == 0)
        {
            parameter = &parameterTable[index];
            break;
        }
    }
    return parameter;
}

//*********************************************************************
// Set a parameter value
void r4aEsp32NvmParameterSet(const char * filePath,
                             const R4A_ESP32_NVM_PARAMETER * parameterTable,
                             int parameterCount,
                             const R4A_ESP32_NVM_PARAMETER * parameter,
                             const char * valueString,
                             Print * display)
{
    R4A_ESP32_NVM_VALUE value;

    // Convert the valueString into a number or an allocated string
    if (r4aEsp32NvmParseValue(parameter, valueString, &value, display))
    {
        // Successful conversion, set the value
        r4aEsp32NvmSetParameterValue(parameter, value.u64);

        // Write the parameters to the file
        r4aEsp32NvmWriteParameters(filePath, parameterTable, parameterCount, display);
    }
}

//*********************************************************************
// Parse the parameter file data
bool r4aEsp32NvmParseParameters(const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                int parameterCount,
                                const char * nvmData,
                                size_t fileBytes,
                                uint8_t * availableParameters,
                                Print * display)
{
    const char * data;
    size_t delta;
    int index;
    const char * name;
    const R4A_ESP32_NVM_PARAMETER * parameter;
    const char * typeString;
    int type;
    bool validParameters;
    R4A_ESP32_NVM_VALUE value;
    const char * valueString;

    // Walk through the file data
    data = nvmData;
    validParameters = false;
    while (fileBytes)
    {
        // Read the parameter triplet of strings from the file:
        //
        //  +-------+-------+-------+
        //  | Name  | Type  | Value |
        //  +-------+-------+-------+
        validParameters = true;
        if (!(r4aEsp32NvmGetString(&name, &data, &fileBytes)
              && r4aEsp32NvmGetString(&typeString, &data, &fileBytes)
              && r4aEsp32NvmGetString(&valueString, &data, &fileBytes)))
        {
            delta = data - nvmData;
            display->printf("ERROR: String at offset %d (0x%x) is invalid!\r\n",
                            delta, delta);
            validParameters = false;
            break;
        }

        // The type string should be a numeric value
        if (sscanf(typeString, "%d", &type) != 1)
        {
            delta = typeString - nvmData;
            display->printf("ERROR: Type string not a number at offset %d (0x%x)!\r\n",
                            delta, delta);
            validParameters = false;
            break;
        }

        // Look up the parameter
        parameter = r4aEsp32NvmParameterLookup(parameterTable, parameterCount, name, display);
        if (!parameter)
        {
            // Unknown parameter, skip this triplet
            display->printf("WARNING: %s is not a parameter!\r\n", name);
            continue;
        }

        // Parse the value
        value.u64 = 0;
        if (!r4aEsp32NvmParseValue(parameter, valueString, &value, display))
        {
            delta = valueString - nvmData;
            display->printf("ERROR: Invalid %s value string at offset %d (0x%x)!\r\n",
                            name, delta, delta);
            validParameters = false;
            break;
        }

        // Set the value
        r4aEsp32NvmSetParameterValue(parameter, value.u64);
        index = parameter - parameterTable;
        availableParameters[index >> 3] |= 1 << (index & 7);
    }

    // Return the file read status
    if (fileBytes)
        validParameters = false;
    return validParameters;
}

//*********************************************************************
// Read the parameters from a file
bool r4aEsp32NvmReadParameters(const char * filePath,
                               const R4A_ESP32_NVM_PARAMETER * parameterTable,
                               int parameterCount,
                               Print * display)
{
    size_t bytesRead;
    size_t fileBytes;
    File parameterFile;
    uint8_t * nvmData;
    bool status;

    // Allocate the available parameter bitmap
    int parameterBytes = (parameterCount + 7) >> 3;
    uint8_t availableParameters[parameterBytes];
    memset(availableParameters, 0, parameterBytes);

    nvmData = nullptr;
    status = false;
    do
    {
        // Determine if the file exists
        if (!LittleFS.exists(filePath))
        {
            display->printf("Parameter file %s not found!\r\n", filePath);
            break;
        }

        // Open the parameter file
        parameterFile = LittleFS.open(filePath, "r");
        if (!parameterFile)
        {
            display->printf("ERROR: Failed to open file %s!\r\n", filePath);
            break;
        }

        // Determine the file size
        fileBytes = parameterFile.size();
        if (!fileBytes)
            break;

        // Allocate the file data buffer
        nvmData = (uint8_t *)malloc(fileBytes);
        if (!nvmData)
        {
            display->println("ERROR: Failed to allocate read buffer!");
            break;
        }

        // Read the file into memory
        bytesRead = parameterFile.read(nvmData, fileBytes);
        if (bytesRead != fileBytes)
        {
            display->println("ERROR: Failed to read file into memory!");
            break;
        }

        display->printf("Loading parameters from %s\r\n", filePath);

        // Parse the parameters
        if (!r4aEsp32NvmParseParameters(parameterTable,
                                        parameterCount,
                                        (const char *)nvmData,
                                        fileBytes,
                                        availableParameters,
                                        display))
            break;

        // Display any missing parameters
        for (int index = 0; index < parameterCount; index++)
            if (parameterTable[index].required
                && (!(availableParameters[index >> 3] & (1 << (index & 7)))))
            {
                display->printf("Parameter %s is using the default value\r\n", parameterTable[index].name);
            }
        status = true;
    } while (0);

    // Free the NVM data
    if (nvmData)
        free(nvmData);

    // Close the file
    if (parameterFile)
        parameterFile.close();

    // Return status indicating if the parameters were successfully read
    return status;
}

//*********************************************************************
// Write the parameters to a file
void r4aEsp32NvmWriteParameters(const char * filePath,
                                const R4A_ESP32_NVM_PARAMETER * parameterTable,
                                int parameterCount,
                                Print * display)
{
    // Attempt to open the file
    File parameterFile = LittleFS.open(filePath, "w");
    if (!parameterFile)
        display->printf("ERROR: Failed to create file %s!\r\n", filePath);
    else
    {
        display->printf("Saving parameters to %s\r\n", filePath);

        // Walk the list of parameters
        for (int parameter = 0; parameter < parameterCount; parameter++)
            // Save the parameter value
            if (!r4aEsp32NvmWriteParameterValue(parameterFile,
                                                &parameterTable[parameter],
                                                display))
                display->printf("ERROR: Failed to write parameter %s to file %s!\r\n",
                                parameterTable[parameter].name, filePath);

        // Done with the file
        parameterFile.close();
    }
}
