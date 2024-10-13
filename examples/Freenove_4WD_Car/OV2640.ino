/**********************************************************************
  OV2640.ino

  OV2640 camera support routines
**********************************************************************/

//*********************************************************************
// Process the frame buffer
// Inputs:
//   frameBuffer: Buffer containing the raw image data
//   display: Address of Print object for output
// Outputs:
//   Returns true if the processing was successful and false upon error
bool OV2640::processFrameBuffer(camera_fb_t * frameBuffer,
                                Print * display)
{
    return true;
}

//*********************************************************************
// Process the web server's frame buffer
// Inputs:
//   frameBuffer: Buffer containing the raw image data
// Outputs:
//   Returns true if the processing was successful and false upon error
bool OV2640::processWebServerFrameBuffer(camera_fb_t * frameBuffer)
{
    return true;
}
