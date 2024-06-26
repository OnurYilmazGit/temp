#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <esp_task_wdt.h>
#include "FS.h"
#include "LittleFS.h"


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

void displayPicture() {
  const char* filename = "/IMG_0001.bin"; // Make sure this matches the uploaded file name
  // Assuming the image dimensions are known
  const int imgWidth = 200; // Adjust based on your image's actual dimensions
  const int imgHeight = 200; // Adjust based on your image's actual dimensions

  // Open the file from the filesystem
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Calculate the size of the image data
  size_t fileSize = file.size();
  uint8_t* buffer = new uint8_t[fileSize]; // Dynamically allocate memory for the image data

  if (buffer == nullptr) {
    Serial.println("Failed to allocate memory for image");
    file.close();
    return;
  }

  // Read the image data into the buffer
  if (file.read(buffer, fileSize) != fileSize) {
    Serial.println("Failed to read the full image data");
    delete[] buffer;
    file.close();
    return;
  }
  file.close();

  // Initialize the display and clear it
  display.init();
  display.fillScreen(GxEPD_BLACK);

  // Display the image from the buffer
  display.drawBitmap(0, 0, buffer, imgWidth, imgHeight, GxEPD_WHITE);

  // Refresh the display to show the new image
  display.display();

  // Clean up and free the allocated memory
  delete[] buffer;
}

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

  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed, formatting...");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("Failed to format and mount LittleFS");
      return;
    }
  }

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

  server.on("/display", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("Received POST request on /display");
    if (request->hasParam("plain", true)) {
      AsyncWebParameter *p = request->getParam("plain", true);
      Serial.println("Received text: " + p->value());
      displayText(p->value());
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });


  server.on("/cross", HTTP_GET, [](AsyncWebServerRequest *request) {
    displayCross();  // Call the function to display the cross
    request->send(200, "text/plain", "Black and white cross displayed");
  });

  server.on(
    "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "File upload successful.");
    },
    [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
      static File uploadFile;
      if (!index) {
        Serial.println("Upload Start: " + filename);
        uploadFile = LittleFS.open("/" + filename, "w");
      }
      if (uploadFile) {
        uploadFile.write(data, len);
        if (final) {
          Serial.println("Upload Complete: " + filename);
          Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
          uploadFile.close();
          displayPicture();
        }
      }
    });

  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    String output = "Files:\n";
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
      output += String(file.name()) + "\n";
      file = root.openNextFile();
    }
    request->send(200, "text/plain", output);
  });

  // Start the web server
  server.begin();
}
void loop() {
  // put your main code here, to run repeatedly:
  // Avoid running the display logic in loop, as it's only needed once for a static display of text.
}
