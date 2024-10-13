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
