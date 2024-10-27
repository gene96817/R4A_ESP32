# R4A_ESP32

[Robots-For-All](https://robots-for-all.org/index.html): ESP32 support

The [R4A_ESP32](https://github.com/LeeLeahy2/R4A_ESP32) library contains
ESP32 specific support code including:

- ESP32 support routines
- Freenove 4WD Car support
- GPIO register dump
- I2C driver
- Lock routine
- NVM (Non-Volatile-Memory) and parameter support
- OV2640 camera support
- Timer register dump
- Waypoint support
- Web server support
- WiFi support

Examples include:

- [Freenove 4WD Car](https://www.amazon.com/dp/B08X6PTQFM?ref=ppx_yo2ov_dt_b_fed_asin_title) - Example showing the library support.  Features include:
  - Basic light following
  - Basic line following
  - Shell for:
    - Advanced line following
    - Camera line following
    - Waypoint following with optional GNSS receiver
  - GNSS logging support with optional GNSS receiver
  - Motor support
  - OV2640 camera support
  - Servo support
  - Serial menu support
  - WS2812 support
  - Waypoint support with optional GNSS receiver
  - Wifi support
    - mDNS
    - NTP
    - NTRIP (GNSS corrections) with optional GNSS receiver
    - Telnet menu support
    - Web server
