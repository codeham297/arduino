#include "waterflow.h"
#include "blynk.h"
#include "globals.h"

void waterFlowTask(void *pvParameters)
{
    // One-time initialization
    pinMode(FLOW_SENSOR_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), countPulse, RISING);
    unsigned long lastPulseCount = 0;

    // Continuous operation
    while (true)
    {
        if (current_user_uid != "00000000")
        {
            unsigned long newPulses = pulseCount - lastPulseCount;
            float deltaWater = newPulses / (float)PULSES_PER_LITER;
            totalWaterUsed = pulseCount / (float)PULSES_PER_LITER;

            if (newPulses > 0)
            {
                Serial.println("Water Used: " + String(deltaWater) + " L (Total: " + String(totalWaterUsed) + " L)");

                for (int i = 0; i < 10; i++)
                {
                    if (users[i].cardUID == current_user_uid)
                    {
                        users[i].balance -= deltaWater;
                        users[i].waterUsage += deltaWater;
                        sendMeterData(users[i].userName, deltaWater, users[i].waterUsage, users[i].balance);

                        if (users[i].balance <= 0.132)
                        {
                            Serial.println("Balance depleted for " + users[i].userName);
                            digitalWrite(SOLENOID_VALVE, LOW);
                            digitalWrite(YELLOW_LED, LOW);
                            current_user_uid = "00000000";
                            pulseCount = 0;
                            totalWaterUsed = 0.0;
                            lastPulseCount = 0;
                        }
                        break;
                    }
                }
            }
            lastPulseCount = pulseCount;
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}