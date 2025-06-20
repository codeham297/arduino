#include <vibration.h>

// Vibration thresholds
#define LOW_VIBRATION 0.2
#define MEDIUM_VIBRATION 1.0
#define HIGH_VIBRATION 3.0

/* Sensor instance */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

/* Previous values for motion tracking */
float prevX = 0.0, prevY = 0.0, prevZ = 0.0;
float vibration = 0.0; // Global variable to store vibration level

/* Function to initialize the sensor */
void initVibrationSensor()
{

    if (!accel.begin())
    {
        Serial.println("Ooops, no ADXL345 detected ... Check wiring!");
        while (1)
        {
            delay(1000);
        }
    }

    accel.setRange(ADXL345_RANGE_16_G);
    Serial.println("ADXL345 Initialized for Vibration Detection!");
}

/* Function to classify vibration based on motion change */
void classifyVibration(float change)
{
    // if (change < LOW_VIBRATION)
    // {
    //     Serial.println("Vibration Level: LOW ⚪");
    // }
    // else if (change < MEDIUM_VIBRATION)
    // {
    //     Serial.println("Vibration Level: MEDIUM 🟡");
    // }
    // else
    // {
    //     Serial.println("Vibration Level: HIGH 🔴");
    // }
}

/* Task for vibration sensing */
void initVibrationTask(void *pvParameters)
{
    initVibrationSensor(); // Call the initialization function
    while (1)
    {
        sensors_event_t event;
        accel.getEvent(&event);

        // Calculate change in acceleration
        float dx = abs(event.acceleration.x - prevX);
        float dy = abs(event.acceleration.y - prevY);
        float dz = abs(event.acceleration.z - prevZ);
        float totalChange = dx + dy + dz;

        classifyVibration(totalChange);
        // Store previous values
        prevX = event.acceleration.x;
        prevY = event.acceleration.y;
        prevZ = event.acceleration.z;

        vTaskDelay(pdMS_TO_TICKS(500)); // Proper RTOS timing delay
        vibration = totalChange;        // Update global vibration variable
    }
}