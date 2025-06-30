#include <Arduino.h>
#include "blynk.h"

struct UserData
{
  String userName;
  String cardUID;
  float waterUsage;
  float balance;
};

// Sample user database
UserData users[10] = {
    {"Alice", "33AD9C14", 8.0, 1000},
    {"Bob", "4C870802", 15.0, 10000},
    {"Charlie", "DB05219F", 0.0, 98000.0},
    {"David", "5B06F79E", 19.0, 8000},
    {"Eve", "DE6BD09F", 12.0, 72000},
    {"Frank", "9B5FFE9E", 0.0, 100.0},
    {"Grace", "7BDD219F", 4.0, 3000},
    {"Heidi", "CBCF199F", 18.0, 40000},
    {"Ivan", "CB9C099F", 2.0, 75000},
    {"Judy", "0BDE229F", 10.0, 80000}};

void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT); // Example pin setup
  // xTaskCreate(BlynkManagerTask, "BlynkManager", 4096, NULL, 1, NULL);
}
int j = 0;
void loop()
{
  if (j < 10)
  {
    j += 1;
  }
  else
  {
    j = 0; // Reset j to loop through users
  }
  // put your main code here, to run repeatedly:
  digitalWrite(2, HIGH); // Turn on the pin
  delay(10000);          // Wait for a second
  digitalWrite(2, LOW);  // Turn off the pin
  delay(10000);          // Wait for a second
  for (int i = 0; i < 10; i++)
  {
    users[i].waterUsage += 1.0; // Simulate water usage increment
    users[i].balance -= 10.0;   // Simulate balance deduction
  }
  String user_data = formatUserData();

  Serial.println(users[j].userName);
  Serial.println(users[j].waterUsage);
  Serial.println(user_data);
  sendMeterData(users[j].userName, users[j].waterUsage, user_data, users[j].balance);
}

String formatUserData()
{
  String output = "";
  for (int i = 0; i < 10; i++)
  {
    output += users[i].userName + ", " + String(users[i].waterUsage) + ", " + String(users[i].balance) + "\n";
  }
  return output;
}
