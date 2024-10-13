/**********************************************************************
  WebServer.cpp

  Robots-For-All (R4A)
  Web server implementation
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// Locals
//****************************************

static R4A_WEB_SERVER * r4aWebServer;

//*********************************************************************
// Handle the web server errors
static esp_err_t r4aWebServerError (httpd_req_t *req, httpd_err_code_t error)
{
    r4aWebServer->error(req, error, &Serial);
}

//*********************************************************************
// Handle the web server errors
esp_err_t R4A_WEB_SERVER::error (httpd_req_t *req,
                                 httpd_err_code_t error,
                                 Print * display)
{
    const R4A_TAG_NAME_T methodName[] =
    {
        {HTTP_GET, "GET"},
        {HTTP_POST, "POST"},
    };
    const int methodNameEntries = sizeof(methodName) / sizeof(methodName[0]);
    int index;
    const char * method;
    static char line[1000];

    // Get the method name
    method = nullptr;
    for (index = 0; index < methodNameEntries; index++)
        if (req->method == methodName[index].tag)
        {
            method = methodName[index].name;
            break;
        }

    // Send the error to the browser
    sprintf(line, "<html><body>%s: %s<br>\r\n%s\r\n</body>\r\n</html>\r\n",
            (method ? method : "Request"),
            req->uri,
            r4aHttpErrorName[error]);
    httpd_resp_send_err(req, error, line);

    // Display the error locally
    if (display)
    {
        if (!method)
            method = "Unknown";
        display->printf("Request: %s(%d) %s\r\n", method, req->method, req->uri);
        display->printf("%s\r\n", r4aHttpErrorName[error]);
    }

    return ESP_OK;
}

//*********************************************************************
// Start the web server
bool R4A_WEB_SERVER::start(uint16_t port, Print * display)
{
    esp_err_t error;

    // Generate default configuration
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port;

    // Start the httpd server
    do
    {
        error = httpd_start(&_webServer, &config);
        if (error != ESP_OK)
        {
            if (display)
                display->printf("ERROR: Failed to start the web server, error: %d!\r\n",
                                error);
            break;
        }

        // Register URI handlers
        error = httpd_register_uri_handler(_webServer, &r4aOV2640JpegPage);
        if (error != ESP_OK)
        {
            if (display)
                display->printf("ERROR: Failed to register URI handler, error: %d!\r\n", error);
            break;
        }

        // Register the error handlers
        for (int index = 0; index < r4aHttpErrorCount; index++)
        {
            error = httpd_register_err_handler(_webServer,
                                               r4aHttpError[index],
                                               r4aWebServerError);
            if (error != ESP_OK)
            {
                if (display)
                    display->printf("ERROR: Failed to register error handler: %s, error: %d!\r\n",
                                    r4aHttpErrorName[index], error);
                break;
            }
        }

        // Display the web server path
        Serial.printf("Starting web-server: http://%s:%d%s\r\n",
                      WiFi.localIP().toString().c_str(),
                      port, R4A_OV2640_JPEG_WEB_PAGE);

        // Successful server initialization
        r4aWebServer = this;
        return true;
    } while (0);

    // Stop the web server
    stop();

    // The web server failed to start
    return false;
}

//*********************************************************************
// Stop the web server
void R4A_WEB_SERVER::stop(Print * display)
{
    if (_webServer)
    {
        if (display)
            Serial.println("Stopping the web-server");

        // Stop the web server
        httpd_stop(_webServer);
        _webServer = nullptr;

        // Done with the web server
        if (r4aWebServer)
            r4aWebServer = nullptr;
    }
}

//*********************************************************************
// Update the camera processing state
// Inputs:
//   wifiConnected: True when WiFi has an IP address and false otherwise
//   display: Address of Print object for debug output, may be nullptr
void R4A_WEB_SERVER::update(bool wifiConnected, Print * display)
{
    uint32_t currentMsec;
    camera_fb_t * frameBuffer;
    static uint32_t lastImageMsec;

    // Start the web server if necessary
    if ((!_webServer) && wifiConnected)
        start(_port, display);

    // Stop the web server if necessary
    if (_webServer && (!wifiConnected))
        stop(display);
}
