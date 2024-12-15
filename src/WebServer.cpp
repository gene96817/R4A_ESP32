/**********************************************************************
  WebServer.cpp

  Robots-For-All (R4A)
  Web server implementation
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// Globals
//****************************************

Print * r4aWebServerDebug;
const char * r4aWebServerDownloadArea;
const char * r4aWebServerNvmArea;

//****************************************
// Locals
//****************************************

static R4A_WEB_SERVER * r4aWebServer;

//*********************************************************************
// Check for extension
bool r4aWebServerCheckExtension(const char * path, const char * extension)
{
    int extensionLength;
    int pathLength;

    // Get the lengths
    pathLength = strlen(path);
    extensionLength = strlen(extension);

    // Check for extension
    return ((pathLength >= extensionLength)
        && (strcmp(&path[pathLength - extensionLength], extension) == 0));
}

//*********************************************************************
// Handle the web server errors
esp_err_t r4aWebServerError (httpd_req_t *req, httpd_err_code_t error)
{
    return r4aWebServer->error(req, error);
}

//*********************************************************************
// Download a file from the robot to the browser
//   request: Address of a HTTP request object
// Outputs:
//   Returns the file download status
esp_err_t r4aWebServerFileDownload(httpd_req_t *request)
{
    return r4aWebServer->fileDownload(request);
}

//*********************************************************************
// Handle the web server errors
esp_err_t R4A_WEB_SERVER::error (httpd_req_t *req, httpd_err_code_t error)
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
    if (r4aWebServerDebug)
    {
        if (!method)
            method = "Unknown";
        r4aWebServerDebug->printf("Request: %s(%d) %s\r\n",
                                  method, req->method, req->uri);
        r4aWebServerDebug->printf("%s\r\n", r4aHttpErrorName[error]);
    }

    return ESP_OK;
}

//*********************************************************************
// Download a file from the robot to the browser
//   request: Address of a HTTP request object
// Outputs:
//   Returns the file download status
esp_err_t R4A_WEB_SERVER::fileDownload(httpd_req_t *request)
{
    uint8_t * buffer;
    const size_t bufferLength = 8192;
    size_t bytesRead;
    const char * dataType;
    File file;
    const char * path;
    esp_err_t status;

    do
    {
        buffer = nullptr;

        // Get the file name
        path = request->uri;

        // Remove the download prefix
        if (strncmp(r4aWebServerNvmArea, path, strlen(r4aWebServerNvmArea)) != 0)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Not a file download request\r\n");
            httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Not a file download request");
            break;
        }
        path = &path[strlen(r4aWebServerNvmArea) - 1];

        // Determine if the file exists
        if (LittleFS.exists(path) == false)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: File does not exist!\r\n");
            httpd_resp_send_err(request, HTTPD_404_NOT_FOUND, "File does not exist");
            break;
        }

        // Determine the data type
        dataType = nullptr;
        if (r4aWebServerCheckExtension(path, ".txt")
            || r4aWebServerCheckExtension(path, ".log"))
            dataType = "text/plain";
        if (!dataType)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Unknowo file type!\r\n");
            httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Unknown file type");
            break;
        }

        // Allocate the buffer
        buffer = (uint8_t *)malloc(bufferLength);
        if (!buffer)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Failed to allocate the data buffer\r\n");
            httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to allocate data buffer");
            break;
        }

        // Open the file
        file = LittleFS.open(path, FILE_READ);
        if (!file)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Failed to open file %s\r\n", path);
            httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to open existing file");
            break;
        }

        // Verify that the file is not a directory
        if (file.isDirectory())
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: File %s is a directory!\r\n", path);
            httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Unable to download a directory");
            break;
        }

        // Send the file contents to the browser
        do
        {
            // Read data from the file
            bytesRead = file.read(buffer, bufferLength);

            // Send a partial response
            if (bytesRead > 0)
                status = httpd_resp_send_chunk(request, (char *)buffer, bytesRead);
            else
                status = httpd_resp_send_chunk(request, NULL, 0);

            // Process the error
            if (status != ESP_OK)
            {
                if (r4aWebServerDebug)
                    r4aWebServerDebug->printf("ERROR: Failed to send %d data bytes to browser\r\n", bytesRead);
                httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send data to browser");
                break;
            }
        } while (bytesRead > 0);

        // Free the data buffer
        free(buffer);

        // Close the file
        file.close();
        return ESP_OK;
    } while (0);

    // Close the file if necessary
    if (file)
        file.close();

    // Free the data buffer
    if (buffer)
        free(buffer);

    // Failed to access the requested page
    return ESP_FAIL;
}

//*********************************************************************
// Start the web server
bool R4A_WEB_SERVER::start(uint16_t port)
{
    esp_err_t error;

    // Generate default configuration
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port;

    // Start the httpd server
    do
    {
        // Update the configuration
        configUpdate(&config);

        // Start the web server
        error = httpd_start(&_webServer, &config);
        if (error != ESP_OK)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Failed to start the web server, error: %d!\r\n",
                                          error);
            break;
        }

        // Register URI handlers
        if (!registerUriHandlers())
            break;

        // Register the error handlers
        if (!registerErrorHandlers())
            break;

        // Display the web server path
        Serial.printf("Starting web-server: http://%s:%d\r\n",
                      WiFi.localIP().toString().c_str(),
                      port);

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
void R4A_WEB_SERVER::stop()
{
    if (_webServer)
    {
        if (r4aWebServerDebug)
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
void R4A_WEB_SERVER::update(bool wifiConnected)
{
    // Start the web server if necessary
    if ((!_webServer) && wifiConnected)
        start(_port);

    // Stop the web server if necessary
    if (_webServer && (!wifiConnected))
        stop();
}
