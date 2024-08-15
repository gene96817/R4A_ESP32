/**********************************************************************
  R4A_ESP32_GPIO.h

  Declare the GPIO constants and data structures
**********************************************************************/

#ifndef __R4A_ESP32_GPIO_H__
#define __R4A_ESP32_GPIO_H__

#define R4A_GPIO_MAX_PORTS              40

typedef struct _R4A_GPIO_REGS
{
    uint32_t reserved_0;                //     0
    uint32_t R4A_GPIO_OUT_REG;          //     4
    uint32_t R4A_GPIO_OUT_W1TS_REG;     //     8
    uint32_t R4A_GPIO_OUT_W1TC_REG;     // 0x00c
    uint32_t R4A_GPIO_OUT1_REG;         // 0x010
    uint32_t R4A_GPIO_OUT1_W1TS_REG;    // 0x014
    uint32_t R4A_GPIO_OUT1_W1TC_REG;    // 0x018
    uint32_t reserved_01c;              // 0x01c
    uint32_t R4A_GPIO_ENABLE_REG;       // 0x020
    uint32_t R4A_GPIO_ENABLE_W1TS_REG;  // 0x024
    uint32_t R4A_GPIO_ENABLE_W1TC_REG;  // 0x028
    uint32_t R4A_GPIO_ENABLE1_REG;      // 0x02c
    uint32_t R4A_GPIO_ENABLE1_W1TS_REG; // 0x030
    uint32_t R4A_GPIO_ENABLE1_W1TC_REG; // 0x034
    uint32_t R4A_GPIO_STRAP_REG;        // 0x038
    uint32_t R4A_GPIO_IN_REG;           // 0x03c
    uint32_t R4A_GPIO_IN1_REG;          // 0x040
    uint32_t R4A_GPIO_STATUS_REG;       // 0x044
    uint32_t R4A_GPIO_STATUS_W1TS_REG;  // 0x048
    uint32_t R4A_GPIO_STATUS_W1TC_REG;  // 0x04c
    uint32_t R4A_GPIO_STATUS1_REG;      // 0x050
    uint32_t R4A_GPIO_STATUS1_W1TS_REG; // 0x054
    uint32_t R4A_GPIO_STATUS1_W1TC_REG; // 0x058
    uint32_t reserved_05c;              // 0x05c
    uint32_t R4A_GPIO_ACPU_INT_REG;     // 0x060
    uint32_t R4A_GPIO_ACPUNMI_INT_REG;  // 0x064
    uint32_t R4A_GPIO_PCPU_INT_REG;     // 0x068
    uint32_t R4A_GPIO_PCPU_NMI_INT_REG; // 0x06c
    uint32_t reserved_070;              // 0x070
    uint32_t R4A_GPIO_ACPU_INT1_REG;    // 0x074
    uint32_t R4A_GPIO_ACPU_NMI_INT1_REG;// 0x078
    uint32_t R4A_GPIO_PCPU_INT1_REG;    // 0x07c
    uint32_t R4A_GPIO_PCPU_NMI_INT1_REG;// 0x080
    uint32_t reserved_84;               // 0x084
    uint32_t R4A_GPIO_PIN_REG[R4A_GPIO_MAX_PORTS]; // 0x088 - 0x12f
    uint32_t reserved_128;              // 0x128
    uint32_t reserved_12c;              // 0x12c
    uint32_t R4A_GPIO_FUNC_IN_SEL_CFG_REG[256]; //0x130 - 0x52f
    uint32_t R4A_GPIO_FUNC_OUT_SEL_CFG_REG[R4A_GPIO_MAX_PORTS]; //0x530 - 0x5cf
} R4A_GPIO_REGS;
R4A_GPIO_REGS * r4aGpioRegs = (R4A_GPIO_REGS *)0x3ff44000;

#define GPIO_FUNC_SIG_IN_SEL        0x00000080  // Force output to port
#define GPIO_FUNC_IN_INV_SEL        0x00000040  // Invert the signal
#define GPIO_FUNC_IN_SEL            0x0000002f  // Port number or 0x30 low or 0x38 high

#define GPIO_FUNC_OEN_INV_SEL       0x00000800  // Invert the output
#define GPIO_FUNC_OEN_SEL           0x00000400  // 1: Use GPIO_ENABLE_REG to enable output, 0: Use peripheral direction
#define GPIO_FUNC_OUT_INV_SEL       0x00000200  // Invert the output
#define GPIO_FUNC_OUT_SEL           0x000001ff  // Index into the GPIO matrix

// GPIO output
//
//               GPIO_FUNC_OUT_SEL          MCU SEL
//                       |                     |
//                       v                     v
//      Signals --> GPIO_MATRIX -----------> IO_MUX ------> I/O pad
//    0x100 = GPIO out           GPIOx_out
//

typedef struct _R4A_IO_MUX_REGS
{
    uint32_t R4A_IO_MUX_PIN_CTRL;       //    0,  0
    uint32_t R4A_IO_MUX_GPIO36_REG;     //    4,  1
    uint32_t R4A_IO_MUX_GPIO37_REG;     //    8,  2
    uint32_t R4A_IO_MUX_GPIO38_REG;     // 0x0c,  3
    uint32_t R4A_IO_MUX_GPIO39_REG;     // 0x10,  4
    uint32_t R4A_IO_MUX_GPIO34_REG;     // 0x14,  5
    uint32_t R4A_IO_MUX_GPIO35_REG;     // 0x18,  6
    uint32_t R4A_IO_MUX_GPIO32_REG;     // 0x1c,  7
    uint32_t R4A_IO_MUX_GPIO33_REG;     // 0x20,  8
    uint32_t R4A_IO_MUX_GPIO25_REG;     // 0x24,  9
    uint32_t R4A_IO_MUX_GPIO26_REG;     // 0x28, 10
    uint32_t R4A_IO_MUX_GPIO27_REG;     // 0x2c, 11
    uint32_t R4A_IO_MUX_MTMS_REG;       // 0x30, 12
    uint32_t R4A_IO_MUX_MTDI_REG;       // 0x34, 13
    uint32_t R4A_IO_MUX_MTCK_REG;       // 0x38, 14
    uint32_t R4A_IO_MUX_MTDO_REG;       // 0x3c, 15
    uint32_t R4A_IO_MUX_GPIO2_REG;      // 0x40, 16
    uint32_t R4A_IO_MUX_GPIO0_REG;      // 0x44, 17
    uint32_t R4A_IO_MUX_GPIO4_REG;      // 0x48, 18
    uint32_t R4A_IO_MUX_GPIO16_REG;     // 0x4c, 19
    uint32_t R4A_IO_MUX_GPIO17_REG;     // 0x50, 20
    uint32_t R4A_IO_MUX_SD_DATA2_REG;   // 0x54, 21
    uint32_t R4A_IO_MUX_SD_DATA3_REG;   // 0x58, 22
    uint32_t R4A_IO_MUX_SD_CMD_REG;     // 0x5c, 23
    uint32_t R4A_IO_MUX_SD_CLK_REG;     // 0x60, 24
    uint32_t R4A_IO_MUX_SD_DATA0_REG;   // 0x64, 25
    uint32_t R4A_IO_MUX_SD_DATA1_REG;   // 0x68, 26
    uint32_t R4A_IO_MUX_GPIO5_REG;      // 0x6c, 27
    uint32_t R4A_IO_MUX_GPIO18_REG;     // 0x70, 28
    uint32_t R4A_IO_MUX_GPIO19_REG;     // 0x74, 29
    uint32_t R4A_IO_MUX_GPIO20_REG;     // 0x78, 30
    uint32_t R4A_IO_MUX_GPIO21_REG;     // 0x7c, 31
    uint32_t R4A_IO_MUX_GPIO22_REG;     // 0x80, 32
    uint32_t R4A_IO_MUX_U0RXD_REG;      // 0x84, 33
    uint32_t R4A_IO_MUX_U0TXD_REG;      // 0x88, 34
    uint32_t R4A_IO_MUX_GPIO23_REG;     // 0x8c, 35
    uint32_t R4A_IO_MUX_GPIO24_REG;     // 0x90, 36
} R4A_IO_MUX_REGS;
R4A_IO_MUX_REGS * r4aIoMux = (R4A_IO_MUX_REGS *)0x3ff49000;

#define IO_MUX_MCU_SEL              0x00007000  // IO mux for pin
#define IO_MUX_FUN_DRV              0x00000c00  // Drive strength
#define IO_MUX_IN_IE                0x00000200  // Enable input
#define IO_MUX_FUN_WPU              0x00000100  // Enble pull up
#define IO_MUX_FUN_WPD              0x00000080  // Enable pull down
#define IO_MUX_MCU_DRV              0x00000060  // Drive strength during sleep mode
#define IO_MUX_MCU_IE               0x00000010  // Input enable during sleep mode
#define IO_MUX_MCU_WPU              0x00000008  // Enable pull up during sleep mode
#define IO_MUX_MCU_WPD              0x00000004  // Enable pull down during sleep mode
#define IO_MUX_SLP_SEL              0x00000002  // Place pad in sleep mode
#define IO_MUX_MCU_OE               0x00000001  // Output enable during sleep mode

int r4aGpioPortToIoMuxIndex[R4A_GPIO_MAX_PORTS] =
{ // 0   1   2   3   4   5   6   7   8   9
    17, 34, 16, 33, 18, 27, 24, 25, 26, 21,     //  0
    22, 23, 13, 14, 12, 15, 19, 20, 28, 29,     // 10
    30, 31, 32, 35, 36,  9, 10, 11, -1, -1,     // 20
    -1, -1,  7,  8,  5,  6,  1,  2,  3,  4,     // 30
};

const char * const r4aIoMuxNames[R4A_GPIO_MAX_PORTS][8] =
{ //    0            1         2          3            4             5           6     7
    "GPIO0",    "CLK_OUT1", "GPIO0",  "-",          "-",        "EMAC_TX_CLK",  "-",  "-", //  0
    "U0TXD",    "CLK_OUT3", "GPIO1",  "-",          "-",        "EMAC_RXD2",    "-",  "-", //  1
    "GPIO2",    "HSPIWP",   "GPIO2",  "HS2_DATA0",  "SD_DATA0", "-",            "-",  "-", //  2
    "U0RXD",    "CLK_OUT2", "GPIO3",  "-",          "-",        "-",            "-",  "-", //  3
    "GPIO4",    "HSPIHD",   "GPIO4",  "HS2_DATA1",  "SD_DATA1", "EMAC_TX_ER",   "-",  "-", //  4
    "GPIO5",    "VSPICS0",  "GPIO5",  "HS1_DATA6",  "-",        "EMAC_RX_CLK",  "-",  "-", //  5
    "SD_CLK",   "SPICLK",   "GPIO6",  "HS1_CLK",    "U1CTS",    "-",            "-",  "-", //  6
    "SD_DATA0", "SPIQ",     "GPIO7",  "HS1_DATA0",  "U2RTS",    "-",            "-",  "-", //  7
    "SD_DATA1", "SPID",     "GPIO8",  "HS1_DATA1",  "U2CTS",    "-",            "-",  "-", //  8
    "SD_DATA2", "SPIHD",    "GPIO9",  "HS1_DATA2",  "U1RXD",    "-",            "-",  "-", //  9
    "SD_DATA3", "SPIWP",    "GPIO10", "HS1_DATA3",  "U1TXD",    "-",            "-",  "-", //  10
    "SD_CMD",   "SPICS0",   "GPIO11", "HS1_CMD",    "U1RTS",    "-",            "-",  "-", //  11
    "MTDI",     "HSPIQ",    "GPIO12", "HS2_DATA2",  "SD_DATA2", "EMAC_TXD3",    "-",  "-", //  12
    "MTCK",     "HSPID",    "GPIO13", "HS2_DATA3",  "SD_DATA3", "EMAC_RX_ER",   "-",  "-", //  13
    "MTMS",     "HSPICLK",  "GPIO14", "HS2_CLK",    "SD_CLK",   "EMAC_TXD2",    "-",  "-", //  14
    "MTDO",     "HSPICS0",  "GPIO15", "HS2_CMD",    "SD_CMD",   "EMAC_RXD3",    "-",  "-", //  15
    "GPIO16",   "-",        "GPIO16", "HS1_DATA4",  "U2RXD",    "EMAC_CLK_OUT", "-",  "-", //  16
    "GPIO17",   "-",        "GPIO17", "HS1_DATA5",  "U2TXD",    "EMAC_CLK_180", "-",  "-", //  17
    "GPIO18",   "VSPICLK",  "GPIO18", "HS1_DATA7",  "-",        "-",            "-",  "-", //  18
    "GPIO19",   "VSPIQ",    "GPIO19", "U0CTS",      "-",        "EMAC_TXD0",    "-",  "-", //  19
    "-",        "-",        "-",      "-",          "-",        "-",            "-",  "-", //  20
    "GPIO21",   "VSPIHD",   "GPIO21", "-",          "-",        "EMAC_TX_EN",   "-",  "-", //  21
    "GPIO22",   "VSPIWP",   "GPIO22", "U0RTS",      "-",        "EMAC_TXD1",    "-",  "-", //  22
    "GPIO23",   "VSPID",    "GPIO23", "HS1_STROBE", "-",        "-",            "-",  "-", //  23
    "-",        "-",        "-",      "-",          "-",        "-",            "-",  "-", //  24
    "GPIO25",   "-",        "GPIO25", "-",          "-",        "EMAC_RXD0",    "-",  "-", //  25
    "GPIO26",   "-",        "GPIO26", "-",          "-",        "EMAC_RXD1",    "-",  "-", //  26
    "GPIO27",   "-",        "GPIO27", "-",          "-",        "EMAC_RX_DV",   "-",  "-", //  27
    "-",        "-",        "-",      "-",          "-",        "-",            "-",  "-", //  28
    "-",        "-",        "-",      "-",          "-",        "-",            "-",  "-", //  29
    "-",        "-",        "-",      "-",          "-",        "-",            "-",  "-", //  30
    "-",        "-",        "-",      "-",          "-",        "-",            "-",  "-", //  31
    "GPIO32",   "-",        "GPIO32", "-",          "-",        "-",            "-",  "-", //  32
    "GPIO33",   "-",        "GPIO33", "-",          "-",        "-",            "-",  "-", //  33
    "GPIO34",   "-",        "GPIO34", "-",          "-",        "-",            "-",  "-", //  34
    "GPIO35",   "-",        "GPIO35", "-",          "-",        "-",            "-",  "-", //  35
    "GPIO36",   "-",        "GPIO36", "-",          "-",        "-",            "-",  "-", //  36
    "GPIO37",   "-",        "GPIO37", "-",          "-",        "-",            "-",  "-", //  37
    "GPIO38",   "-",        "GPIO38", "-",          "-",        "-",            "-",  "-", //  38
    "GPIO39",   "-",        "GPIO39", "-",          "-",        "-",            "-",  "-", //  39
};

const uint8_t r4aIoMuxIsGpio[R4A_GPIO_MAX_PORTS] =
{
    0b00000101, //  0
    0b00000101, //  1
    0b00000101, //  2
    0b00000101, //  3
    0b00000101, //  4
    0b00000101, //  5
    0b00000100, //  6
    0b00000100, //  7
    0b00000100, //  8
    0b00000100, //  9
    0b00000100, // 10
    0b00000100, // 11
    0b00000100, // 12
    0b00000100, // 13
    0b00000100, // 14
    0b00000100, // 15
    0b00000101, // 16
    0b00000101, // 17
    0b00000101, // 18
    0b00000101, // 19
    0b00000000, // 20
    0b00000101, // 21
    0b00000101, // 22
    0b00000101, // 23
    0b00000000, // 24
    0b00000101, // 25
    0b00000101, // 26
    0b00000101, // 27
    0b00000000, // 28
    0b00000000, // 29
    0b00000000, // 30
    0b00000000, // 31
    0b00000101, // 32
    0b00000101, // 33
    0b00000101, // 34
    0b00000101, // 35
    0b00000101, // 36
    0b00000101, // 37
    0b00000101, // 38
    0b00000101, // 39
};

typedef struct _R4A_GPIO_MATRIX
{
    const char * r4aInput;
    const char * r4aOutput;
} R4A_GPIO_MATRIX;

const R4A_GPIO_MATRIX r4aGpioMatrixNames[256] =
{ //  Input                    Output
    {"SPICLK_in",             "SPICLK_out"},        //   0
    {"SPIQ_in",               "SPIQ_out"},          //   1
    {"SPID_in",               "SPID_out"},          //   2
    {"SPIHD_in",              "SPIHD_out"},         //   3
    {"SPIWP_in",              "SPIWP_out"},         //   4
    {"SPICS0_in",             "SPICS0_out"},        //   5
    {"SPICS1_in",             "SPICS1_out"},        //   6
    {"SPICS2_in",             "SPICS2_out"},        //   7
    {"HSPICLK_in",            "HSPICLK_out"},       //   8
    {"HSPIQ_in",              "HSPIQ_out"},         //   9
    {"HSPID_in",              "HSPID_out"},         //  10
    {"HSPICS0_in",            "HSPICS0_out"},       //  11
    {"HSPIHD_in",             "HSPIHD_out"},        //  12
    {"HSPIWP_in",             "HSPIWP_out"},        //  13
    {"U0RXD_in",              "U0RXD_out"},         //  14
    {"U0CTS_in",              "U0CTS_out"},         //  15
    {"U0DSR_in",              "U0DSR_out"},         //  16
    {"U1RXD_in",              "U1RXD_out"},         //  17
    {"U1CTS_in",              "U1CTS_out"},         //  18
    {"-",                     "-"},                 //  19
    {"-",                     "-"},                 //  20
    {"-",                     "-"},                 //  21
    {"-",                     "-"},                 //  22
    {"I2S0O_BCK_in",          "I2S0O_BCK_out"},     //  23
    {"I2S10_BCK_in",          "I2S10_BCK_out"},     //  24
    {"I2S0O_WS_in",           "I2S0O_WS_out"},      //  25
    {"I2S1O_WS_in",           "I2S1O_WS_out"},      //  26
    {"I2S0I_BCK_in",          "I2S0I_BCK_out"},     //  27
    {"I2S0I_WS_in",           "I2S0I_WS_out"},      //  28
    {"I2CEXT0_SCL_in",        "I2CEXT0_SCL_out"},   //  29
    {"I2CEXT0_SDA_in",        "I2CEXT0_SDA_out"},   //  30
    {"pwm0_sync0_in",         "sdio_tohost_int_out"},   //  31
    {"pwm0_sync1_in",         "pwm0_out0a"},        //  32
    {"pwm0_sync2_in",         "pwm0_out0b"},        //  33
    {"pwm0_f0_in",            "pwm0_out1a"},        //  34
    {"pwm0_f1_in",            "pwm0_out1b"},        //  35
    {"pwm0_f2_in",            "pwm0_out2a"},        //  36
    {"-",                     "pwm0_out2b"},        //  37
    {"-",                     "-"},                 //  38
    {"pcnt_sig_ch0_in0",      "-"},                 //  39
    {"pcnt_sig_ch1_in0",      "-"},                 //  40
    {"pcnt_ctrl_ch0_in0",     "-"},                 //  41
    {"pcnt_ctrl_ch1_in0",     "-"},                 //  42
    {"pcnt_sig_ch0_in1-",     "-"},                 //  43
    {"pcnt_sig_ch1_in1",      "-"},                 //  44
    {"pcnt_ctrl_ch0_in1",     "-"},                 //  45
    {"pcnt_ctrl_ch1_in1",     "-"},                 //  46
    {"pcnt_sig_ch0_in2",      "-"},                 //  47
    {"pcnt_sig_ch1_in2",      "-"},                 //  48
    {"pcnt_ctrl_ch0_in2",     "-"},                 //  49
    {"pcnt_ctrl_ch1_in2",     "-"},                 //  50
    {"pcnt_sig_ch0_in3",      "-"},                 //  51
    {"pcnt_sig_ch1_in3",      "-"},                 //  52
    {"pcnt_ctrl_ch0_in3",     "-"},                 //  53
    {"pcnt_ctrl_ch1_in3",     "-"},                 //  54
    {"pcnt_sig_ch0_in4",      "-"},                 //  55
    {"pcnt_sig_ch1_in4",      "-"},                 //  56
    {"pcnt_ctrl_ch0_in4",     "-"},                 //  57
    {"pcnt_ctrl_ch1_in4",     "-"},                 //  58
    {"-",                     "-"},                 //  59
    {"-",                     "-"},                 //  60
    {"HSPICS1_in",            "HSPICS1_out"},       //  61
    {"HSPICS2_in",            "HSPICS2_out"},       //  62
    {"VSPICLK_in",            "VSPICLK_out"},       //  63
    {"VSPIQ_in",              "VSPIQ_out"},         //  64
    {"VSPID_in",              "VSPID_out"},         //  65
    {"VSPIHD_in",             "VSPIHD_out"},        //  66
    {"VSPIWP_in",             "VSPIWP_out"},        //  67
    {"VSPICS0_in",            "VSPICS0_out"},       //  68
    {"VSPICS1_in",            "VSPICS1_out"},       //  69
    {"VSPICS2_in",            "VSPICS2_out"},       //  70
    {"pcnt_sig_ch0_in5",      "ledc_hs_sig_out0"},  //  71
    {"pcnt_sig_ch1_in5",      "ledc_hs_sig_out1"},  //  72
    {"pcnt_ctrl_ch0_in5",     "ledc_hs_sig_out2"},  //  73
    {"pcnt_ctrl_ch1_in5",     "ledc_hs_sig_out3"},  //  74
    {"pcnt_sig_ch0_in6",      "ledc_hs_sig_out4"},  //  75
    {"pcnt_sig_ch1_in6",      "ledc_hs_sig_out5"},  //  76
    {"pcnt_ctrl_ch0_in6",     "ledc_hs_sig_out6"},  //  77
    {"pcnt_ctrl_ch1_in6",     "ledc_hs_sig_out7"},  //  78
    {"pcnt_sig_ch0_in7",      "ledc_ls_sig_out0"},  //  79
    {"pcnt_sig_ch1_in7",      "ledc_ls_sig_out1"},  //  80
    {"pcnt_ctrl_ch0_in7",     "ledc_ls_sig_out2"},  //  81
    {"pcnt_ctrl_ch1_in7",     "ledc_ls_sig_out3"},  //  82
    {"rmt_sig_in0",           "ledc_ls_sig_out4"},  //  83
    {"rmt_sig_in1",           "ledc_ls_sig_out5"},  //  84
    {"rmt_sig_in2",           "ledc_ls_sig_out6"},  //  85
    {"rmt_sig_in3",           "ledc_ls_sig_out7"},  //  86
    {"rmt_sig_in4",           "rmt_sig_out0"},      //  87
    {"rmt_sig_in5",           "rmt_sig_out1"},      //  88
    {"rmt_sig_in6",           "rmt_sig_out2"},      //  89
    {"rmt_sig_in7",           "rmt_sig_out3"},      //  90
    {"-",                     "rmt_sig_out4"},      //  91
    {"-",                     "rmt_sig_out5"},      //  92
    {"-",                     "rmt_sig_out6"},      //  93
    {"-",                     "rmt_sig_out7"},      //  94
    {"I2CEXT1_SCL_in",        "I2CEXT1_SCL_out"},   //  95
    {"I2CEXT1_SDA_in",        "I2CEXT1_SDA_out"},   //  96
    {"host_card_detect_n_1",  "host_ccmd_od_pullup_en_n"},    //  97
    {"host_card_detect_n_2",  "host_rst_n_1"},      //  98
    {"host_card_write_prt_1", "host_rst_n_2"},      //  99
    {"host_card_write_prt_2", "gpio_sd0_out"},      // 100
    {"host_card_int_n_1",     "gpio_sd1_out"},      // 101
    {"host_card_int_n_2",     "gpio_sd2_out"},      // 102
    {"pwm1_sync0_in",         "gpio_sd3_out"},      // 103
    {"pwm1_sync1_in",         "gpio_sd4_out"},      // 104
    {"pwm1_sync2_in",         "gpio_sd5_out"},      // 105
    {"pwm1_f0_in",            "gpio_sd6_out"},      // 106
    {"pwm1_f1_in",            "gpio_sd7_out"},      // 107
    {"pwm1_f2_in",            "pwm1_out0a"},        // 108
    {"pwm0_cap0_in",          "pwm1_out0b"},        // 109
    {"pwm0_cap1_in",          "pwm1_out1a"},        // 110
    {"pwm0_cap2_in",          "pwm1_out1b"},        // 111
    {"pwm1_cap0_in",          "pwm1_out2a"},        // 112
    {"pwm1_cap1_in",          "pwm1_out2b"},        // 113
    {"pwm1_cap2_in",          "pwm2_out1h"},        // 114
    {"-pwm2_flta",            "pwm2_out1h"},        // 115
    {"pwm2_fltb",             "pwm2_out2h"},        // 116
    {"pwm2_cap1_in",          "pwm2_outl"},         // 117
    {"pwm2_cap2_in",          "pwm2_out3h"},        // 118
    {"pwm2_cap3_in",          "pwm2_out3l"},        // 119
    {"pwm3_flta",             "pwm2_out4h"},        // 120
    {"pwm3_fltb",             "pwm2_out4l"},        // 121
    {"pwm3_cap1_in",          "-"},                 // 122
    {"pwm3_cap2_in",          "-"},                 // 123
    {"pwm3_cap3_in",          "-"},                 // 124
    {"-",                     "-"},                 // 125
    {"-",                     "-"},                 // 126
    {"-",                     "-"},                 // 127
    {"-",                     "-"},                 // 128
    {"-",                     "-"},                 // 129
    {"-",                     "-"},                 // 130
    {"-",                     "-"},                 // 131
    {"-",                     "-"},                 // 132
    {"-",                     "-"},                 // 133
    {"-",                     "-"},                 // 134
    {"-",                     "-"},                 // 135
    {"-",                     "-"},                 // 136
    {"-",                     "-"},                 // 137
    {"-",                     "-"},                 // 138
    {"-",                     "-"},                 // 139
    {"I2S0I_DATA_in0",        "I2S0I_DATA_out0"},   // 140
    {"I2S0I_DATA_in1",        "I2S0I_DATA_out1"},   // 141
    {"I2S0I_DATA_in2",        "I2S0I_DATA_out2"},   // 142
    {"I2S0I_DATA_in3",        "I2S0I_DATA_out3"},   // 143
    {"I2S0I_DATA_in4",        "I2S0I_DATA_out4"},   // 144
    {"I2S0I_DATA_in5",        "I2S0I_DATA_out5"},   // 145
    {"I2S0I_DATA_in6",        "I2S0I_DATA_out6"},   // 146
    {"I2S0I_DATA_in7",        "I2S0I_DATA_out7"},   // 147
    {"I2S0I_DATA_in8",        "I2S0I_DATA_out8"},   // 148
    {"I2S0I_DATA_in9",        "I2S0I_DATA_out9"},   // 149
    {"I2S0I_DATA_in10",       "I2S0I_DATA_out10"},  // 150
    {"I2S0I_DATA_in11",       "I2S0I_DATA_out11"},  // 151
    {"I2S0I_DATA_in12",       "I2S0I_DATA_out12"},  // 152
    {"I2S0I_DATA_in13",       "I2S0I_DATA_out13"},  // 153
    {"I2S0I_DATA_in14",       "I2S0I_DATA_out14"},  // 154
    {"I2S0I_DATA_in15",       "I2S0I_DATA_out15"},  // 155
    {"-",                     "I2S0I_DATA_out16"},  // 156
    {"-",                     "I2S0I_DATA_out17"},  // 157
    {"-",                     "I2S0I_DATA_out18"},  // 158
    {"-",                     "I2S0I_DATA_out19"},  // 159
    {"-",                     "I2S0I_DATA_out20"},  // 160
    {"-",                     "I2S0I_DATA_out21"},  // 161
    {"-",                     "I2S0I_DATA_out22"},  // 162
    {"-",                     "I2S0I_DATA_out13"},  // 163
    {"I2S1I_BCK_in",          "I2S1I_BCK_out"},     // 164
    {"I2S1I_WS_in",           "I2S1I_WS_outt"},     // 165
    {"I2S1I_DATA_in0",        "I2S1I_DATA_out0"},   // 166
    {"I2S1I_DATA_in1",        "I2S1I_DATA_out1"},   // 167
    {"I2S1I_DATA_in2",        "I2S1I_DATA_out2"},   // 168
    {"I2S1I_DATA_in3",        "I2S1I_DATA_out3"},   // 169
    {"I2S1I_DATA_in4",        "I2S1I_DATA_out4"},   // 170
    {"I2S1I_DATA_in5",        "I2S1I_DATA_out5"},   // 171
    {"I2S1I_DATA_in6",        "I2S1I_DATA_out6"},   // 172
    {"I2S1I_DATA_in7",        "I2S1I_DATA_out7"},   // 173
    {"I2S1I_DATA_in8",        "I2S1I_DATA_out8"},   // 174
    {"I2S1I_DATA_in9",        "I2S1I_DATA_out9"},   // 175
    {"I2S1I_DATA_in10",       "I2S1I_DATA_out10"},  // 176
    {"I2S1I_DATA_in11",       "I2S1I_DATA_out11"},  // 177
    {"I2S1I_DATA_in12",       "I2S1I_DATA_out12"},  // 178
    {"I2S1I_DATA_in13",       "I2S1I_DATA_out13"},  // 179
    {"I2S1I_DATA_in14",       "I2S1I_DATA_out14"},  // 180
    {"I2S1I_DATA_in15",       "I2S1I_DATA_out15"},  // 181
    {"-",                     "I2S1I_DATA_out16"},  // 182
    {"-",                     "I2S1I_DATA_out17"},  // 183
    {"-",                     "I2S1I_DATA_out18"},  // 184
    {"-",                     "I2S1I_DATA_out19"},  // 185
    {"-",                     "I2S1I_DATA_out20"},  // 186
    {"-",                     "I2S1I_DATA_out21"},  // 187
    {"-",                     "I2S1I_DATA_out22"},  // 188
    {"-",                     "I2S1I_DATA_out23"},  // 189
    {"I2SOI_H_SYNC",          "pwm3_out1h"},        // 190
    {"I2SOI_v_SYNC",          "pwm3_out1l"},        // 191
    {"I2SOI_H_ENABLE",        "pwm3_out2h"},        // 192
    {"I2S1I_H_SYNC",          "pwm3_out2l"},        // 193
    {"I2S1I_V_SYNC",          "pwm3_out3h"},        // 194
    {"I2S1I_H_ENABLE",        "pwm3_out3l"},        // 195
    {"-",                     "pwm3_out4h"},        // 196
    {"-",                     "pwm3_out4l"},        // 197
    {"U2RXD_in",              "U2TXD_out"},         // 198
    {"U2CTS_in",              "U2RTS_out"},         // 199
    {"emac_mdc_i",            "emac_mdc_o"},        // 200
    {"emac_mdi_i",            "emac_mdo_o"},        // 201
    {"emac_crs_i",            "emac_crs_o"},        // 202
    {"emac_col_i",            "emac_col_o"},        // 203
    {"pcmfsync_in",           "bt_audio0_irq"},     // 204
    {"pcmclk_in",             "bt_audio1_irq"},     // 205
    {"pcmdin",                "bt_audio2_irq"},     // 206
    {"-",                     "ble_audio0_irq"},    // 207
    {"-",                     "ble_audio1_irq"},    // 208
    {"-",                     "ble_audio2_irq"},    // 209
    {"-",                     "pcmfsync_out"},      // 210
    {"-",                     "pcmclk_out"},        // 211
    {"-",                     "pcmdout"},           // 212
    {"-",                     "ble_audio_sync0_p"}, // 213
    {"-",                     "ble_audio_sync1_p"}, // 214
    {"-",                     "ble_audio_sync2_p"}, // 215
    {"-",                     "-"},                 // 216
    {"-",                     "-"},                 // 217
    {"-",                     "-"},                 // 218
    {"-",                     "-"},                 // 219
    {"-",                     "-"},                 // 220
    {"-",                     "-"},                 // 221
    {"-",                     "-"},                 // 222
    {"-",                     "-"},                 // 223
    {"-",                     "sig_in_func224"},    // 224
    {"-",                     "sig_in_func225"},    // 225
    {"-",                     "sig_in_func226"},    // 226
    {"-",                     "sig_in_func227"},    // 227
    {"-",                     "sig_in_func228"},    // 228
    {"-",                     "-"},                 // 229
    {"-",                     "-"},                 // 230
    {"-",                     "-"},                 // 231
    {"-",                     "-"},                 // 232
    {"-",                     "-"},                 // 233
    {"-",                     "-"},                 // 234
    {"-",                     "-"},                 // 235
    {"-",                     "-"},                 // 236
    {"-",                     "-"},                 // 237
    {"-",                     "-"},                 // 238
    {"-",                     "-"},                 // 239
    {"-",                     "-"},                 // 240
    {"-",                     "-"},                 // 241
    {"-",                     "-"},                 // 242
    {"-",                     "-"},                 // 243
    {"-",                     "-"},                 // 244
    {"-",                     "-"},                 // 245
    {"-",                     "-"},                 // 246
    {"-",                     "-"},                 // 247
    {"-",                     "-"},                 // 248
    {"-",                     "-"},                 // 249
    {"-",                     "-"},                 // 250
    {"-",                     "-"},                 // 251
    {"-",                     "-"},                 // 252
    {"-",                     "-"},                 // 253
    {"-",                     "-"},                 // 254
    {"-",                     "-"},                 // 255
};

typedef struct _R4A_RTCIO_REGS
{
    uint32_t R4A_RTCIO_RTC_GPIO_OUT_REG;        //    0
    uint32_t R4A_RTCIO_RTC_GPIO_OUT_W1TS_REG;   //    4
    uint32_t R4A_RTCIO_RTC_GPIO_OUT_W1TC_REG;   //    8
    uint32_t R4A_RTCIO_RTC_GPIO_ENABLE_REG;     // 0x0c
    uint32_t R4A_RTCIO_RTC_GPIO_ENABLE_W1TS_REG;// 0x10
    uint32_t R4A_RTCIO_RTC_GPIO_ENABLE_W1TC_REG;// 0x14
    uint32_t R4A_RTCIO_RTC_GPIO_STATUS_REG;     // 0x18
    uint32_t R4A_RTCIO_RTC_GPIO_STATUS_W1TS_REG;// 0x1c
    uint32_t R4A_RTCIO_RTC_GPIO_STATUS_W1TC_REG;// 0x20
    uint32_t R4A_RTCIO_RTC_GPIO_IN_REG;         // 0x24
    uint32_t R4A_RTCIO_RTC_GPIO_PIN_REG[18];    // 0x28 - 0x6f
    uint32_t reserved_70;                       // 0x70
    uint32_t R4A_RTCIO_DIG_PAD_HOLD_REG;        // 0x74
    uint32_t R4A_RTCIO_HALL_SENS_REG;           // 0x78
    uint32_t R4A_RTCIO_SENSOR_PADS_REG;         // 0x7c
    uint32_t R4A_RTCIO_ADC_PAD_REG;             // 0x80
    uint32_t R4A_RTCIO_PAD_DAC1_REG;            // 0x84
    uint32_t R4A_RTCIO_PAD_DAC2_REG;            // 0x88
    uint32_t R4A_RTCIO_XTAL_32K_PAD_REG;        // 0x8c
    uint32_t R4A_RTCIO_TOUCH_CFG_REG;           // 0x90
    uint32_t R4A_RTCIO_TOUCH_PAD_REG[10];       // 0x94 - 0xbb
    uint32_t R4A_RTCIO_EXT_WAKEUP0_REG;         // 0xbc
    uint32_t R4A_RTCIO_XTL_EXT_CTR_REG;         // 0xc0
    uint32_t R4A_RTCIO_SAR_I2C_IO_REG;          // 0xc4
} R4A_RTCIO_REGS;
R4A_RTCIO_REGS * r4aRtcioRegs = (R4A_RTCIO_REGS *)0x3ff48400;

//****************************************
// API
//****************************************

void r4aEsp32GpioDisplayRegisters(Print * display = &Serial);
void r4aEsp32GpioDisplayIoMuxRegisters(int portNumber, uint32_t regValue, Print * display = &Serial);
void r4aEsp32GpioDisplayIoMuxRegisters(Print * display = &Serial);

#endif  // __R4A_ESP32_GPIO_H__
