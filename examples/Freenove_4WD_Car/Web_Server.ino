/**********************************************************************
  Web_Server.ino

  Web server support routines
**********************************************************************/

//*********************************************************************
// Update the configuration
// Inputs:
//   config: Address of the HTTP config object
void WEB_SERVER::configUpdate(httpd_config_t * config)
{
    // Enable matching multiple web pages
    config->uri_match_fn = httpd_uri_match_wildcard;
}

//*********************************************************************
// Register the error handlers
bool WEB_SERVER::registerErrorHandlers()
{
    esp_err_t error;

    for (int index = 0; index < r4aHttpErrorCount; index++)
    {
        error = httpd_register_err_handler(_webServer,
                                           r4aHttpError[index],
                                           r4aWebServerError);
        if (error != ESP_OK)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Failed to register error handler: %s, error: %d!\r\n",
                                          r4aHttpErrorName[index], error);
            return false;
        }
    }
    return true;
}

//*********************************************************************

// URI handler for getting uploaded files
const httpd_uri_t webServerFileDownloadUri = {
    .uri       = DOWNLOAD_AREA "*",  // Match all URIs of type /path/to/file
    .method    = HTTP_GET,
    .handler   = r4aWebServerFileDownload,
    .user_ctx  = nullptr
};

//*********************************************************************
// Register the URI handlers
bool WEB_SERVER::registerUriHandlers()
{
    esp_err_t error;

    do
    {
        // Add the NVM download page
        error = httpd_register_uri_handler(_webServer, &webServerFileDownloadUri);
        if (error != ESP_OK)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Failed to register NVM download handler, error: %d!\r\n", error);
            break;
        }

#ifdef  USE_OV2640
        // Add the jpeg camera image page
        error = httpd_register_uri_handler(_webServer, &r4aOV2640JpegPage);
        if (error != ESP_OK)
        {
            if (r4aWebServerDebug)
                r4aWebServerDebug->printf("ERROR: Failed to register JPEG handler, error: %d!\r\n", error);
            break;
        }
#endif  // USE_OV2640

        // Successfully registered the handlers
        return true;
    } while (0);

    // Display the error message
    if (r4aWebServerDebug)
        r4aWebServerDebug->printf("ERROR: Failed to register URI handler, error: %d!\r\n", error);
    return false;
}
