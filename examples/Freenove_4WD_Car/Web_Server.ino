/**********************************************************************
  Web_Server.ino

  Web server support routines
**********************************************************************/

//*********************************************************************
// Register the error handlers
bool WEB_SERVER::registerErrorHandlers(Print * display)
{
    esp_err_t error;

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
            return false;
        }
    }
    return true;
}

//*********************************************************************
// Register the URI handlers
bool WEB_SERVER::registerUriHandlers(Print * display)
{
    esp_err_t error;

    do
    {
        // Add the jpeg camera image page
        error = httpd_register_uri_handler(_webServer, &r4aOV2640JpegPage);
        if (error != ESP_OK)
            break;

        // Successfully registered the handlers
        return true;
    } while (0);

    // Display the error message
    if (display)
        display->printf("ERROR: Failed to register URI handler, error: %d!\r\n", error);
    return false;
}
