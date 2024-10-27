/**********************************************************************
  Waypoint.cpp

  Robots-For-All (R4A)
  Waypoint support
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// Constants
//****************************************

static const char * wpFormat  = "%14.9f   %14.9f   %10.3f   %9.3f   %9.3f   %3d   %s\r\n";
//                               -123.123456789   -123.123456789   123456.123   12345.123   12345.123   123   1234567
static const char * wpHeader1 = "   Latitude        Longitude       Altitude    Horiz Acc   Std. Dev.   SIV   Comment\r\n";
static const char * wpHeader2 = "--------------   --------------   ----------   ---------   ---------   ---   -------\r\n";

//****************************************
// Globals
//****************************************

int r4aEsp32WpPointsToAverage = 50;
const char * r4aEsp32WpFileName = "Waypoints.txt";

//*********************************************************************
// Add a point to the waypoint file.  This routine is called indirectly
// by loop after the mean latitude and longitude are calculated.
// Inputs:
//   parameter: Callback parameter passed to computeWayPoint
//   comment: Text to display at the start of the line
//   latitude: Latitude in degrees
//   latitudeStdDev: Latitude standard deviation in degrees
//   longitude: Longitude in degrees
//   longitudeStdDev: Longitude standard deviation in degrees
//   altitude: Altitude in meters
//   altitudeStdDev: Altitude standard deviation in meters
//   horizontalAccuracy: Accuracy in meters
//   horizontalAccuracyStdDev: Accuracy standard deviation in meters
//   satellitesInView: The number of satellites feeding the GNSS receiver
//   display: Device used for output, passed to computeWayPoint
void r4aEsp32WpAddPoint(intptr_t parameter,
                        const char * comment,
                        double latitude,
                        double latitudeStdDev,
                        double longitude,
                        double longitudeStdDev,
                        double altitude,
                        double altitudeStdDev,
                        double horizontalAccuracy,
                        double horizontalAccuracyStdDev,
                        uint8_t satellitesInView,
                        Print * display)
{
    size_t bytesToWrite;
    size_t bytesWritten;
    File file;
    size_t fileLength;
    String filePath;
    char line[256];
    const char * path;

    // Attempt to open the file
    do
    {
        filePath = String("/") + String(r4aEsp32WpFileName);
        path = filePath.c_str();
        file = LittleFS.open(path, FILE_APPEND);
        if (!file)
        {
            if (display)
                display->printf("ERROR: Failed to create file %s!\r\n", path);
            break;
        }

        // Get the file size
        fileLength = file.size();

        // Add the file header if necessary
        if (fileLength == 0)
        {
            // Write the header to the file
            if ((!r4aEsp32NvmWriteFileString(file, wpHeader1, strlen(wpHeader1)))
                || (!r4aEsp32NvmWriteFileString(file, wpHeader2, strlen(wpHeader2))))
            {
                if (display)
                    display->printf("ERROR: Failed to write the header to file %s\r\n", path);
                break;
            }
        }

        // Write the waypoint data
        sprintf(line, wpFormat, latitude, longitude, altitude, horizontalAccuracy,
                horizontalAccuracyStdDev, satellitesInView,
                comment ? comment : "");
        if (display)
            display->printf("Saving waypoint to %s\r\n", path);
        if (!r4aEsp32NvmWriteFileString(file, line, strlen(line)))
        {
            if (display)
                display->printf("ERROR: Failed to write the header to file %s\r\n", path);
            break;
        }

        // Display the waypoint
        sprintf(line, wpFormat, latitude, longitude, altitude, horizontalAccuracy,
                horizontalAccuracyStdDev, satellitesInView,
                comment ? comment : "");
        display->print(wpHeader1);
        display->print(wpHeader2);
        display->print(line);

        // The waypoint was successfully written
    } while (0);

    // Done with the file
    if (file)
        file.close();
}

//*********************************************************************
// Add a point to the waypoint file
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32WpMenuAddPoint(const R4A_MENU_ENTRY * menuEntry,
                            const char * command,
                            Print * display)
{
    static String comment;

    // Locate the comment
    comment = r4aMenuGetParameters(menuEntry, command);

    // Start the data collection
    if (r4aZedF9p)
        r4aZedF9p->computePoint(r4aEsp32WpAddPoint,
                                0,
                                r4aEsp32WpPointsToAverage,
                                comment.c_str(),
                                display);
    else
        display->printf("ERROR: GNSS not initialized!\r\n");
}

//*********************************************************************
// Set the waypoint file name
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32WpMenuFileName(const R4A_MENU_ENTRY * menuEntry,
                            const char * command,
                            Print * display)
{
    String fileName;
    const R4A_ESP32_NVM_PARAMETER * parameter;

    // Locate the file name
    fileName = r4aMenuGetParameters(menuEntry, command);

    // Locate the parameter description
    parameter = r4aEsp32NvmParameterLookup(nvmParameters,
                                           nvmParameterCount,
                                           &r4aEsp32WpFileName,
                                           display);
    // Set the file name
    r4aEsp32NvmParameterSet(parameterFilePath,
                            nvmParameters,
                            nvmParameterCount,
                            parameter,
                            fileName.c_str(),
                            display);
}

//*********************************************************************
// Print the waypoint file contents
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void r4aEsp32WpMenuPrintFile(const R4A_MENU_ENTRY * menuEntry,
                             const char * command,
                             Print * display)
{
    String filePath;

    // Display the file contents
    filePath = String("/") + String(r4aEsp32WpFileName);
    r4aEsp32NvmFileCat(filePath, display);
}

