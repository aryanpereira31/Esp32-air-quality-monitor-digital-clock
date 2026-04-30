//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc.
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!
//   Note that some sketches are designed for a particular TFT pixel width/height

// User defined information reported by "Read_User_Setup" test & diagnostics example
#define USER_SETUP_INFO "User_Setup"

// Define to disable all #warnings in library (can be put in User_Setup_Select.h)
//#define DISABLE_ALL_LIBRARY_WARNINGS

// ##################################################################################
//
// Section 1. Call up the right driver file and any options for it
//
// ##################################################################################
#define ST7789_DRIVER      // Full screen 240x320

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// ST7789 displays often need these for correct colors
#define TFT_RGB_ORDER TFT_BGR  
#define TFT_INVERSION_ON

// ###### ESP32 Pins ######
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC   16
#define TFT_RST  17

// ###### Fonts ######
#define LOAD_GLCD
#define LOAD_FONT2
#define SMOOTH_FONT

// ###### Speed ######
#define SPI_FREQUENCY  40000000 // 40MHz is stable for ST7789