/**********************************************************************
  Wifi.cpp

  Robots-For-All (R4A)
  Handle the WiFi events
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// Constants
//****************************************

static const char * r4aEsp32WiFiAuthorizationName[] =
{
    "Open",
    "WEP",
    "WPA_PSK",
    "WPA2_PSK",
    "WPA_WPA2_PSK",
    "WPA2_Enterprise",
    "WPA3_PSK",
    "WPA2_WPA3_PSK",
    "WAPI_PSK",
    "OWE",
    "WPA3_ENT_192",
};
static const int r4aEsp32WiFiAuthorizationNameEntries =
    sizeof(r4aEsp32WiFiAuthorizationName) / sizeof(r4aEsp32WiFiAuthorizationName[0]);

static R4A_WIFI * r4aWiFi;

//*********************************************************************
// Handle the WiFi event
void r4aEsp32WifiEvent(arduino_event_id_t event, arduino_event_info_t info)
{
    r4aWiFi->event(event, info);
}

//*********************************************************************
// Setup WiFi
void R4A_WIFI::begin(const char * hostName)
{
    // Verify the tables
    verifyTables();

    // Save the object
    r4aWiFi = this;

    // Update the host name
    _hostName = hostName;

    // Start the WiFi station
    stationStart();
}

//*********************************************************************
// Get the active channel
//   Returns the channel number or zero (0) if not connected
uint8_t R4A_WIFI::channelGet()
{
    return _channel;
}

//*********************************************************************
// Connect to a remote AP
uint8_t R4A_WIFI::connect(uint8_t apCount)
{
    int ap;
    bool apFound;
    const char * apName;
    int apChannel;
    const char * apPassword;
    const char * apSsid;
    String apSsidString;
    int authIndex;
    int authType;
    int channel;
    Print * debug;
    int type;

    // Verify that an AP was found
    if (apCount == 0)
        return 0;

    // Print the header
    //                                    1                 1         2         3
    //             1234   1234   123456789012345   12345678901234567890123456789012
    debug = (Print *)_debug;
    if (debug)
    {
        debug->printf(" dBm   Chan   Authorization     SSID\r\n");
        debug->printf("----   ----   ---------------   --------------------------------\r\n");
    }

    // Walk the list of APs that were found during the scan
    apFound = false;
    apChannel = 0;
    for (ap = 0; ap < apCount; ap++)
    {
        // The APs are listed in decending signal strength order
        // Check for a requested AP
        apSsidString = WiFi.SSID(ap);
        apSsid = apSsidString.c_str();
        type = WiFi.encryptionType(ap);
        channel = WiFi.channel(ap);
        if (!apFound)
        {
            for (authIndex = 0; authIndex < r4aWifiSsidPasswordEntries; authIndex++)
            {
                // Determine if this authorization matches the AP's SSID
                if (*r4aWifiSsidPassword[authIndex].ssid
                    && strlen(*r4aWifiSsidPassword[authIndex].ssid)
                    && (strcmp(apSsid, *r4aWifiSsidPassword[authIndex].ssid) == 0)
                    && ((type == WIFI_AUTH_OPEN)
                        || (*r4aWifiSsidPassword[authIndex].password
                            && (strlen(*r4aWifiSsidPassword[authIndex].password)))))
                {
                    // A match was found, save it and stop looking
                    apName = *r4aWifiSsidPassword[authIndex].ssid;
                    apPassword = *r4aWifiSsidPassword[authIndex].password;
                    apChannel = channel;
                    authType = type;
                    apFound = true;
                    break;
                }
            }

            // Check for done
            if (apFound && (!_debug))
                break;
        }

        // Display the list of APs
        if (debug)
            debug->printf("%4d   %4d   %s   %s\r\n",
                          WiFi.RSSI(ap),
                          channel,
                          (type < WIFI_AUTH_MAX) ? r4aEsp32WiFiAuthorizationName[type] : "Unknown",
                          apSsid);
    }

    // Connect to the AP
    if (apFound)
    {
        if (debug)
            debug->printf("WiFi connecting to %s on channel %d with %s authorization\r\n",
                          apName,
                          apChannel,
                          (authType < WIFI_AUTH_MAX) ? r4aEsp32WiFiAuthorizationName[authType] : "Unknown");
        _ssid = String(apName);
        _channel = apChannel;
        WiFi.STA.connect(apName, apPassword);
    }

    // Return the channel number
    return apChannel;
}

//*********************************************************************
// Handle the WiFi event
void R4A_WIFI::event(arduino_event_id_t event, arduino_event_info_t info)
{
    int channel;
    Print * debug;
    Print * display;
    static IPAddress localIP;

    // Always output display items
    debug = (Print *)_debug;
    display = (Print *)_display;
    if (!display)
        display = debug;

    // Notify the upper layers that WiFi is no longer available
    if ((event != ARDUINO_EVENT_WIFI_STA_GOT_IP)
        && (event != ARDUINO_EVENT_WIFI_STA_GOT_IP6))
    {
        _channel = 0;
        _connected = false;
        _mdnsAvailable = false;
        _ssid = String("");
    }

    // Handle the event
    switch (event)
    {

    //------------------------------
    // Controller events
    //------------------------------

    case ARDUINO_EVENT_WIFI_OFF:
        if (debug)
            debug->printf("WiFi Event: Off\r\n");
        break;

    case ARDUINO_EVENT_WIFI_READY:
        if (debug)
            debug->printf("WiFi Event: Ready\r\n");
        break;

    //------------------------------
    // WiFi Station State Machine
    //
    //   .--------+<----------+<-----------+<-------------+<----------+<----------+<------------.
    //   v        |           |            |              |           |           |             |
    // STOP --> READY --> STA_START --> SCAN_DONE --> CONNECTED --> GOT_IP --> LOST_IP --> DISCONNECTED
    //            ^                                       ^           ^           |             |
    //            |                                       |           '-----------'             |
    // OFF -------'                                       '-------------------------------------'
    //
    // Handle the WiFi station events
    //------------------------------

    case ARDUINO_EVENT_WIFI_STA_START:
        if (debug)
            debug->printf("WiFi Event: Station start\r\n");
        break;

    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        // uint32_t status:  status of scanning APs: 0 — success, 1 - failure
        // uint8_t  number:  number of scan results
        // uint8_t  scan_id: scan sequence number, used for block scan
        if (info.wifi_scan_done.status == 0)
        {
            if (debug)
                debug->printf("WiFi Event: Scan done\r\n");

            uint8_t apCount = info.wifi_scan_done.number;
            channel = connect(apCount);
        }
        else if (debug)
            debug->printf("WiFi Event: Scan done - failed!\r\n");
        break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        if (debug)
            debug->printf("WiFi Event: Station connected\r\n");
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        localIP = WiFi.STA.localIP();
        if (display)
            display->printf("WiFi Event: Got IP address %s\r\n",
                            localIP.toString().c_str());

        // Start mDNS
        if (_hostName)
        {
            if (MDNS.begin(_hostName))
            {
                if (display)
                    display->printf("mDNS: %s.local connects to %s\r\n",
                                    _hostName, localIP.toString().c_str());
                _mdnsAvailable = true;
            }
            else if (debug)
                debug->printf("ERROR: MDNS responder failed to start!\r\n");
        }
        _connected = true;
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        localIP = WiFi.STA.localIP();
        if (display)
            display->printf("WiFi Event: Got IP6 address\r\n",
                            localIP.toString().c_str());
        _connected = true;
        break;

    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        if (display)
            display->printf("WiFi Event: Lost IP address %d\r\n",
                            localIP.toString().c_str());
        break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        if (debug)
            debug->printf("WiFi Event: Station disconnected\r\n");
        break;

    case ARDUINO_EVENT_WIFI_STA_STOP:
        if (debug)
            debug->printf("WiFi Event: Station stop\r\n");
        break;

    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        if (debug)
            debug->printf("WiFi Event: Station authorization change\r\n");
        break;

    //------------------------------
    // WiFi soft AP (access point) events
    //------------------------------

    case ARDUINO_EVENT_WIFI_AP_START:
        if (debug)
            debug->printf("WiFi Event: AP start\r\n");
        break;

    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        if (debug)
            debug->printf("WiFi Event: AP station connected\r\n");
        break;

    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
        if (debug)
            debug->printf("WiFi Event: AP got IP6 address\r\n");
        break;

    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
        if (debug)
            debug->printf("WiFi Event: AP probe request received\r\n");
        break;

    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        if (debug)
            debug->printf("WiFi Event: AP station IP assigned\r\n");
        break;

    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        if (debug)
            debug->printf("WiFi Event: AP station disconnected\r\n");
        break;

    case ARDUINO_EVENT_WIFI_AP_STOP:
        if (debug)
            debug->printf("WiFi Event: AP stop\r\n");
        break;

    case ARDUINO_EVENT_WIFI_FTM_REPORT:
        if (debug)
            debug->printf("WiFi Event: FTM report\r\n");
        break;
    }
}

//*********************************************************************
// Get the mDNS host name
const char * R4A_WIFI::hostNameGet()
{
    return _hostName;
}

//*********************************************************************
// Set the mDNS host name
bool R4A_WIFI::hostNameSet(const char * hostName)
{
    Print * debug;
    bool success;

    // Verify that a host name was specified
    debug = (Print *)_debug;
    if (!hostName)
    {
        if (debug)
            debug->print("ERROR: No host name specified!\r\n");
        return false;
    }

    // Attempt to set the host name
    success = WiFi.STA.setHostname(hostName);
    if ((!success) && debug)
        debug->print("ERROR: Failed to set the host name!\r\n");
    return success;
}

//*********************************************************************
// Get the connected SSID
//   Returns the address of a zero terminated string of characters or nullptr
const char * R4A_WIFI::ssidGet()
{
    return _ssid.c_str();
}

//*********************************************************************
// Start the Wifi station
bool R4A_WIFI::stationStart()
{
    Print * debug;
    wifi_mode_t mode;
    bool success;

    // Verify that at least one WiFi access point is in the list
    debug = (Print *)_debug;
    if (r4aWifiSsidPasswordEntries == 0)
    {
        if (debug)
            debug->printf("ERROR: No entries in r4aWiFiSsidPasswordEntries\r\n");
        return false;
    }

    // Determine if WiFi is running
    mode = WiFi.getMode();
    if ((mode & (WIFI_MODE_STA | WIFI_MODE_AP)) == 0)
    {
        // Start the WiFi network
        if (debug)
            debug->printf("Initializing the WiFi controller\r\n");

        // Establish the WiFi event handler
        Network.onEvent(r4aEsp32WifiEvent);

        // Start the network
        success = WiFi.begin();

        // Set the host name
        if (success && _hostName)
            success = hostNameSet(_hostName);
    }
    else if ((mode & WIFI_MODE_STA) == 0)
        // Attempt to enable WiFi station
        success = WiFi.mode((wifi_mode_t)(WiFi.getMode() | WIFI_MODE_STA));

    // Start the WiFi scan
    if (success)
    {
        if (debug)
            debug->printf("Starting WiFi scan\r\n");
        if (WiFi.scanNetworks(true) != WIFI_SCAN_RUNNING)
        {
            if (debug)
                debug->printf("ERROR: Failed to start the WiFi scan!\r\n");
        }
    }
    return success;
}

//*********************************************************************
// Stop the WiFi station
bool R4A_WIFI::stationStop()
{
    // Attempt to enable WiFi station
    return WiFi.mode((wifi_mode_t)(WiFi.getMode() & ~WIFI_MODE_STA));
}

//*********************************************************************
// Verify the WiFi tables
void R4A_WIFI::verifyTables()
{
    if (WIFI_AUTH_MAX != r4aEsp32WiFiAuthorizationNameEntries)
    {
        ((Print *)_debug)->printf("ERROR: Fix authorizationNameName list to match wifi_auth_mode_t in esp_wifi_types.h!\r\n");
        r4aReportFatalError("Fix authorizationNameName list to match wifi_auth_mode_t");
    }
}
