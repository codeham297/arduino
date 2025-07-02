#ifndef OTA_H
#define OTA_H

#include <WebServer.h>
#include <Update.h>

// Declare the task functions
void initOTATask(void *parameters);
void startOTAserver(void *parameters);

#endif