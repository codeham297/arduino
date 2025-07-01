#include "ota.h"

WebServer server(80);

// WiFi credentials (externally defined in main.cpp)
extern const char *ssid;
extern const char *pass;

void initOTATask(void *parameters)
{
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // OTA Update Handlers
  server.on("/", HTTP_GET, []()
            {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html",
            "<form method='POST' action='/update' enctype='multipart/form-data'>"
            "<input type='file' name='update'>"
            "<input type='submit' value='Update'>"
            "</form>"); });

  server.on("/update", HTTP_POST, []()
            {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart(); }, []()
            {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        } });

  server.begin();
  Serial.println("OTA Update Server Ready");
  vTaskDelete(NULL); // Delete task after setup
}

void startOTAserver(void *parameters)
{
  // Handle client requests
  while (true)
  {
    server.handleClient();
    delay(1); // Allow other tasks to run
  }
}