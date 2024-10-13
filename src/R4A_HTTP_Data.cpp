/**********************************************************************
  R4A_HTTP_Data.cpp

  Robots-For-All (R4A)
  Provide the HTTP data
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// HTTP API
//****************************************

const httpd_err_code_t r4aHttpError[] =
{
    HTTPD_500_INTERNAL_SERVER_ERROR,
    HTTPD_501_METHOD_NOT_IMPLEMENTED,
    HTTPD_505_VERSION_NOT_SUPPORTED,
    HTTPD_400_BAD_REQUEST,
    HTTPD_401_UNAUTHORIZED,
    HTTPD_403_FORBIDDEN,
    HTTPD_404_NOT_FOUND,
    HTTPD_405_METHOD_NOT_ALLOWED,
    HTTPD_408_REQ_TIMEOUT,
    HTTPD_411_LENGTH_REQUIRED,
    HTTPD_414_URI_TOO_LONG,
    HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE,
};
const int r4aHttpErrorCount = sizeof(r4aHttpError) / sizeof(r4aHttpError[0]);

const char * r4aHttpErrorName[] =
{
    "HTTPD_500_INTERNAL_SERVER_ERROR",
    "HTTPD_501_METHOD_NOT_IMPLEMENTED",
    "HTTPD_505_VERSION_NOT_SUPPORTED",
    "HTTPD_400_BAD_REQUEST",
    "HTTPD_401_UNAUTHORIZED",
    "HTTPD_403_FORBIDDEN",
    "HTTPD_404_NOT_FOUND",
    "HTTPD_405_METHOD_NOT_ALLOWED",
    "HTTPD_408_REQ_TIMEOUT",
    "HTTPD_411_LENGTH_REQUIRED",
    "HTTPD_414_URI_TOO_LONG",
    "HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE",
};
