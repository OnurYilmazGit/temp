#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h> // for color displays
#include <Fonts/FreeMonoBold9pt7b.h>

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
  
  // Set text properties
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  
  // First clear the display
  display.fillScreen(GxEPD_WHITE);
  // Write hello world
  display.setCursor(10, 25); // Set the position where the text will start
  display.print("HELLO WORLD");
  // Update the display with the new data
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Avoid running the display logic in loop, as it's only needed once for a static display of text.
}
