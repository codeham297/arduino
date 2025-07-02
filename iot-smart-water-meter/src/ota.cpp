#include "ota.h"
#include "my_wifi.h"

WebServer server(80);

void initOTATask(void *parameters)
{
    Serial.begin(115200);

    if (!isWiFiConnected())
    {
        Serial.println("WiFi not connected, OTA cannot start");
        vTaskDelete(NULL);
        return;
    }

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
    vTaskDelete(NULL);
}

void startOTAserver(void *parameters)
{
    while (true)
    {
        if (isWiFiConnected())
        {
            server.handleClient();
        }
        delay(1);
    }
}