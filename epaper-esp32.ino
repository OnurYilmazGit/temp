#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <esp_task_wdt.h>


// Connections
#define EPD_CS 5
#define EPD_DC 17
#define EPD_RST 16
#define EPD_BUSY 4
#define EPD_MOSI 23
#define EPD_CLK 18

// GxEPD2 display class, set the correct width, height parameters for your display
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=*/EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RST, /*BUSY=*/EPD_BUSY));

const char *ssid = "FRITZ!Box 6660 Cable IH";
const char *password = "39188923097050675231";

AsyncWebServer server(80);

void clearDisplay() {
  display.init();
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.display();
}

void displayText(const String &text) {
  display.init();
  
  // Set text properties
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  
  // First clear the display
  display.fillScreen(GxEPD_WHITE);
  
  // Set the position where the text will start
  display.setCursor(10, 25);
  
  // Print the received text instead of "HELLO WORLD"
  display.print(text);
  
  // Update the display with the new data
  display.display();
}


void displayCross() {
  display.init();
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);

  int lineThickness = 6;  // Increase this value for bolder lines
  int w = display.width();
  int h = display.height();

  // Draw the first diagonal line of the "X"
  for (int i = -lineThickness / 2; i < lineThickness / 2; i++) {
    display.drawLine(0 + i, 0, w + i, h, GxEPD_BLACK);  // Diagonal line from top-left to bottom-right
    display.drawLine(0, 0 + i, w, h + i, GxEPD_BLACK);  // Diagonal line from top-left to bottom-right
  }

  // Draw the second diagonal line of the "X"
  for (int i = -lineThickness / 2; i < lineThickness / 2; i++) {
    display.drawLine(w - i, 0, 0 - i, h, GxEPD_BLACK);  // Diagonal line from top-right to bottom-left
    display.drawLine(w, 0 + i, 0, h + i, GxEPD_BLACK);  // Diagonal line from top-right to bottom-left
  }

  display.display();
}


void setup() {
  Serial.begin(115200);
  esp_task_wdt_init(8, true);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Endpoint to clear the screen
  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request) {
    clearDisplay();
    request->send(200, "text/plain", "Screen cleared");
  });

server.on("/display", HTTP_POST, [](AsyncWebServerRequest *request){
  Serial.println("Received POST request on /display");
  if (request->hasParam("plain", true)) {
    AsyncWebParameter* p = request->getParam("plain", true);
    Serial.println("Received text: " + p->value());
    displayText(p->value());
    request->send(200, "text/plain", "OK");
  } else {
    request->send(400, "text/plain", "Bad Request");
  }
});

  // Start the web server
  server.on("/cross", HTTP_GET, [](AsyncWebServerRequest *request) {
    displayCross();  // Call the function to display the cross
    request->send(200, "text/plain", "Black and white cross displayed");
  });
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Avoid running the display logic in loop, as it's only needed once for a static display of text.
}
