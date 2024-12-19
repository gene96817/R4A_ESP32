/**********************************************************************
  OV2640.cpp

  Robots-For-All (R4A)
  OV2640 camera support
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// Locals
//****************************************

static R4A_OV2640 * r4aOV2640;

uint8_t r4aOV2640PixelFormat = PIXFORMAT_RGB565;

bool r4aOV2640JpegDisplayTime;  // Set to true to display the JPEG conversion time

//*********************************************************************
// Constructor
// Inputs:
//   i2cBus: R4A_I2C_BUS object address used to access the OV2640 camera
//   i2cAddress: I2C address of the OV2640 camera
//   pins: R4A_OV2640_PINS object containing the ESP32 GPIO pin numbers
//   clockHz: OV2640 clock frequency input
R4A_OV2640::R4A_OV2640(R4A_I2C_BUS * i2cBus,
                       uint8_t i2cAddress,
                       const R4A_OV2640_PINS * pins,
                       uint32_t clockHz)
    : _clockHz{clockHz}, _i2cBus{i2cBus}, _pins{pins},
      _i2cAddress{(uint8_t)(i2cAddress & 0x7f)}
{
}

//*********************************************************************
// Display a group of registers
// Inputs:
//   firstRegister: The register address of the first register to be displayed
//   bytesToRead: The number of registers to display
//   display: Address of Print object for output
void R4A_OV2640::displayRegisters(uint8_t firstRegister,
                                  size_t bytesToRead,
                                  Print * display)
{
    uint8_t data[256];

    // Validate the bytesToRead value
    if (bytesToRead > sizeof(data))
    {
        display->printf("ERROR: bytesToRead > %d!\r\n", sizeof(bytesToRead));
        return;
    }

    // Read the bytes from the camera
    if (_i2cBus->read(_i2cAddress,
                      &firstRegister,
                      sizeof(firstRegister),
                      data,
                      bytesToRead,
                      display) != bytesToRead)
    {
        display->printf("ERROR: Failed to read register 0x%02x\r\n", firstRegister);
        return;
    }

    // Display the bytes
    r4aDumpBuffer(firstRegister, data, bytesToRead, display);
}

//*********************************************************************
// Dump all of the OV2640 registers in hexadecimal
// Inputs:
//   display: Address of Print object for output
void R4A_OV2640::dumpRegisters(Print * display)
{
    size_t bytesToRead;
    uint32_t offset;

    do
    {
        // Display the header
        display->println("OV2640 Registers");
        display->println("----------------");
        display->println();
        display->println("             0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f  0123456789abcdef");
        display->println("            -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --  ----------------");

        // Display register 5
        offset = 5;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0x44
        offset = 0x44;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0x50 - 0x57
        offset = 0x50;
        bytesToRead = 8;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0x5a - 0x57c
        offset = 0x5a;
        bytesToRead = 3;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0x7c - 0x7d
        offset = 0x7c;
        bytesToRead = 2;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0x86 - 0x87
        offset = 0x86;
        bytesToRead = 2;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0x8c
        offset = 0x8c;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0xc0 - 0xc3
        offset = 0xc0;
        bytesToRead = 4;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0xd3
        offset = 0xd3;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0xda
        offset = 0xda;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0xe0
        offset = 0xe0;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0xf0
        offset = 0xf0;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display register 0xf7
        offset = 0xf7;
        bytesToRead = 1;
        displayRegisters(offset, bytesToRead, display);

        // Display the offset header
        display->println("            -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --  ----------------");
        display->println("             0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f  0123456789abcdef");
    } while (0);
}

//*********************************************************************
// Initialize the camera
bool R4A_OV2640::setup(pixformat_t pixelFormat, Print * display)
{
    sensor_t * ov2640Camera;
    esp_err_t status;

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    // Reset and power down are not supported
    config.pin_reset = _pins->pinReset;
    config.pin_pwdn = _pins->pinPowerDown;

    // Clock pin
    config.xclk_freq_hz = 20000000;
    config.pin_xclk = _pins->pinXCLK;

    // Serial IO pins
    config.pin_sccb_sda = _pins->pinI2cData;
    config.pin_sccb_scl = _pins->pinI2cClk;

    // Frame synchronization
    config.pin_vsync = _pins->pinVSYNC;
    config.pin_href = _pins->pinHREF;
    config.pin_pclk = _pins->pinPCLK;

    // Data from the camera
    config.pin_d0 = _pins->pinY2;
    config.pin_d1 = _pins->pinY3;
    config.pin_d2 = _pins->pinY4;
    config.pin_d3 = _pins->pinY5;
    config.pin_d4 = _pins->pinY6;
    config.pin_d5 = _pins->pinY7;
    config.pin_d6 = _pins->pinY8;
    config.pin_d7 = _pins->pinY9;

    // Select output data format
//    config.pixel_format = PIXFORMAT_GRAYSCALE;
//    config.pixel_format = PIXFORMAT_JPEG;
//    config.jpeg_quality = 10;   // 0 - 63, lower numbers are higher quality
//    config.pixel_format = PIXFORMAT_RGB565;
//    config.pixel_format = PIXFORMAT_YUV422;
    config.pixel_format = pixelFormat;

    // Use PSRAM for frame buffer
    config.fb_count = 2;
    config.frame_size = FRAMESIZE_QQVGA;    // 160x120
//    config.frame_size = FRAMESIZE_QCIF;     // 176x144
//    config.frame_size = FRAMESIZE_HQVGA;    // 240x176
//    config.frame_size = FRAMESIZE_240X240;  // 240x240
//    config.frame_size = FRAMESIZE_QVGA;     // 320x240
//    config.frame_size = FRAMESIZE_CIF;      // 400x296
//    config.frame_size = FRAMESIZE_HVGA;     // 480x320
//    config.frame_size = FRAMESIZE_VGA;      // 640x480
//    config.frame_size = FRAMESIZE_SVGA;     // 800x600
//    config.frame_size = FRAMESIZE_XGA;      // 1024x768
//    config.frame_size = FRAMESIZE_HD;       // 1280x720
//    config.frame_size = FRAMESIZE_SXGA;     // 1280x1024
//    config.frame_size = FRAMESIZE_UXGA;     // 1600x1200

    // When to take the picture
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // Initialize the camera
    status = esp_camera_init(&config);
    if (status != ESP_OK)
    {
        // Display the camera initialization error
        display->printf("ERROR: Camera setup failed, returned error 0x%x\r\n", status);
        return false;
    }

    // Get the camera structure
    ov2640Camera = esp_camera_sensor_get();
    if (!ov2640Camera)
    {
        // Display camera structure allocation failure
        display->println("ERROR: Camera structure allocation failure");
        return false;
    }

    // Adjust the image to match what is seen
    ov2640Camera->set_hmirror(ov2640Camera, 1);
    ov2640Camera->set_vflip(ov2640Camera, 1);

    // Adjust the brightness and color saturation
    ov2640Camera->set_brightness(ov2640Camera, 1);
    ov2640Camera->set_saturation(ov2640Camera, -2);
    ov2640Camera->set_agc_gain(ov2640Camera, 30);
    ov2640Camera->set_awb_gain(ov2640Camera, 1);
    ov2640Camera->set_gain_ctrl(ov2640Camera, 1);
    r4aOV2640 = this;
    return true;
}

//*********************************************************************
// Update the camera processing state
// Inputs:
//   display: Address of Print object for debug output, may be nullptr
void R4A_OV2640::update(Print * display)
{
    camera_fb_t * frameBuffer;

    // Get a frame buffer
    frameBuffer = esp_camera_fb_get();
    if (!frameBuffer)
        return;

    // Process the frame buffer
    processFrameBuffer(frameBuffer, display);

    // Return the frame buffer
    esp_camera_fb_return(frameBuffer);
}

//*********************************************************************
// Encode the JPEG image
size_t r4aOV2640SendJpegChunk(void * arg,
                              size_t index,
                              const void* data,
                              size_t len)
{
    R4A_JPEG_CHUNKING_T * chunk = (R4A_JPEG_CHUNKING_T *)arg;
    if(!index){
        chunk->length = 0;
    }
    if(httpd_resp_send_chunk(chunk->req, (const char *)data, len) != ESP_OK){
        return 0;
    }
    chunk->length += len;
    return len;
}

//*********************************************************************
// JPEG image web page handler
esp_err_t r4aOV2640JpegHandler(httpd_req_t *request)
{
    int64_t endTime;
    camera_fb_t * frameBuffer;
    int64_t startTime;
    esp_err_t status;

    do
    {
        startTime = esp_timer_get_time();
        frameBuffer = nullptr;
        status = ESP_FAIL;

        // Allocate the frame buffer
        frameBuffer = esp_camera_fb_get();
        if (!frameBuffer)
        {
            Serial.println("ERROR: Failed to capture the image");
            httpd_resp_send_500(request);
            break;
        }

        // Process the frame buffer
        r4aOV2640->processWebServerFrameBuffer(frameBuffer);

        // Build the response header
        httpd_resp_set_type(request, "image/jpeg");
        httpd_resp_set_hdr(request, "Content-Disposition", "inline; filename=capture.jpg");
        httpd_resp_set_hdr(request, "Access-Control-Allow-Origin", "*");

        // Add the timestamp to the header
        char timestamp[32];
        snprintf(timestamp, sizeof(timestamp), "%lld.%06ld",
                 frameBuffer->timestamp.tv_sec, frameBuffer->timestamp.tv_usec);
        httpd_resp_set_hdr(request, "X-Timestamp", (const char *)timestamp);

        // Send the captured image
        if (frameBuffer->format == PIXFORMAT_JPEG)
        {
            status = httpd_resp_send(request, (const char *)frameBuffer->buf, frameBuffer->len);
            if (status != ESP_OK)
                break;
        }
        else
        {
            // Break the image into multiple chunks
            R4A_JPEG_CHUNKING_T jchunk = {request, 0};
            status = frame2jpg_cb(frameBuffer,
                                  80,
                                  r4aOV2640SendJpegChunk,
                                  &jchunk) ? ESP_OK : ESP_FAIL;
            if (status != ESP_OK)
                break;
            status = httpd_resp_send_chunk(request, NULL, 0);
            if (status != ESP_OK)
                break;
        }
        endTime = esp_timer_get_time();
        if (r4aOV2640JpegDisplayTime)
            Serial.printf("JPG: %lu bytes %lu mSec", (uint32_t)(frameBuffer->len),
                          (uint32_t)((endTime - startTime) / 1000));
        status = ESP_OK;
    } while (0);

    // Return the frame buffer
    if (frameBuffer)
        esp_camera_fb_return(frameBuffer);
    return status;
}

//****************************************
// Constants
//****************************************

// URI handler structure for GET /jpeg
const httpd_uri_t r4aOV2640JpegPage = {
    .uri      = R4A_OV2640_JPEG_WEB_PAGE,
    .method   = HTTP_GET,
    .handler  = r4aOV2640JpegHandler,
    .user_ctx = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};
