#include <GxEPD2_BW.h>
#include "IMG_0001.h" 

// Connections
#define EPD_CS     5
#define EPD_DC     17
#define EPD_RST    16 
#define EPD_BUSY   4 
#define EPD_MOSI   23
#define EPD_CLK    18

// GxEPD2 display class, set the correct width, height parameters for your display
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=*/ EPD_CS, /*DC=*/ EPD_DC, /*RST=*/ EPD_RST, /*BUSY=*/ EPD_BUSY));

void setup() {
  // Initialize the display
  display.init();

  display.fillScreen(GxEPD_BLACK);

  // Draw the bitmap image. Replace "IMG_001" with the actual image name.
  // The parameters are x position, y position, image data, image width, image height, color.
  display.drawBitmap(0, 0, IMAGE_BLACK, 200, 200, GxEPD_WHITE);

  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Avoid running the display logic in loop, as it's only needed once for a static display of text.
}
