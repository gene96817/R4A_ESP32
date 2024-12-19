/**********************************************************************
  OV2640.ino

  OV2640 camera support routines
**********************************************************************/

#ifdef  USE_OV2640

//****************************************
// Constants
//****************************************

// URI handler structure for GET /jpeg
const httpd_uri_t ov2640JpegPage =
{
    .uri      = "/jpeg",
    .method   = HTTP_GET,
    .handler  = r4aOV2640JpegHandler,
    .user_ctx = &ov2640,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};

//*********************************************************************
// Process the frame buffer
// Inputs:
//   object: Address of a R4A_OV2640 data structure
//   frameBuffer: Buffer containing the raw image data
//   display: Address of Print object for output
// Outputs:
//   Returns true if the processing was successful and false upon error
bool ov2640ProcessFrameBuffer(R4A_OV2640 * object,
                              camera_fb_t * frameBuffer,
                              Print * display)
{
    return true;
}

//*********************************************************************
// Process the web server's frame buffer
// Inputs:
//   object: Address of a R4A_OV2640 data structure
//   frameBuffer: Buffer containing the raw image data
// Outputs:
//   Returns true if the processing was successful and false upon error
bool ov2640ProcessWebServerFrameBuffer(R4A_OV2640 * object,
                                       camera_fb_t * frameBuffer)
{
    return true;
}

#endif  // USE_OV2640
