// Blynk Credentials
#define BLYNK_TEMPLATE_ID "TMPL2fA5FeDds"
#define BLYNK_TEMPLATE_NAME "METER"
#define BLYNK_AUTH_TOKEN "iGxpZ0h7AciY9yFXXB4UmwE22eVocKIO"

#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "ABDULHAMEED";
char pass[] = "04012024ABDUL";

// GSM Module Pins
#define GSM_RX 5
#define GSM_TX 18

// Device Indicators
#define BUZZER 26
#define VIBRATION_SENSOR 14
#define SPEAKER 26
#define LED_DANGER 33
#define LED_SAFE 32
#define CAMERA_BTN 34
#define INDICATOR_LED 2 // Blynk connection indicator

const unsigned long check_interval = 100;
unsigned long last_check_time = 0;
unsigned long last_blynk_check_time = 0;
unsigned long last_gsm_check_time = 0;

bool vibration_message_sent = false;
bool elephant_message_sent = false;

SoftwareSerial Sim(GSM_RX, GSM_TX);

String phoneNumber = "+255756920253";
String vibration_message = "Vibration Detected!";
String elephant_message = "ELEPHANT DETECTED!";
String system_status = "System Initialized";

void setup()
{
  Serial.begin(9600);
  Sim.begin(19200);

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  sendGSMCommand("AT+CMGF=1");
  sendGSMCommand("AT+CNMI=2,2,0,0,0");

  pinMode(BUZZER, OUTPUT);
  pinMode(VIBRATION_SENSOR, INPUT);
  pinMode(SPEAKER, OUTPUT);
  pinMode(LED_DANGER, OUTPUT);
  pinMode(LED_SAFE, OUTPUT);
  pinMode(CAMERA_BTN, INPUT);
  pinMode(INDICATOR_LED, OUTPUT);

  digitalWrite(INDICATOR_LED, LOW);

  SendMessage(phoneNumber, system_status);
}

void loop()
{
  Blynk.run();

  unsigned long current_time = millis();

  if (current_time - last_check_time >= check_interval)
  {
    last_check_time = current_time;
    checkSensors();
  }

  if (current_time - last_blynk_check_time >= 5000)
  {
    last_blynk_check_time = current_time;
    checkBlynkConnection();
  }

  if (current_time - last_gsm_check_time >= 10000)
  {
    last_gsm_check_time = current_time;
    ReceiveMessage();
  }
}

// Function to monitor sensors & send data to GSM + Blynk
void checkSensors() {
  bool vibration_detected = digitalRead(VIBRATION_SENSOR) == HIGH;
  bool camera_triggered = digitalRead(CAMERA_BTN) == HIGH;

  if (vibration_detected && camera_triggered && !elephant_message_sent) {
    activateAlert();
    sendData(elephant_message);
    elephant_message_sent = true;
  } else if (!camera_triggered) {
    elephant_message_sent = false;
    sendData("No elephant");
    deactivateAlert();
  }
}

// Function to send data via GSM & Blynk
void sendData(String message)
{
  SendMessage(phoneNumber, message);
  Serial.println("Sending data to Blynk...");
  Blynk.virtualWrite(V1, message);
}

// Functions to activate/deactivate alert systems
void activateAlert()
{
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED_DANGER, HIGH);
  digitalWrite(LED_SAFE, LOW);
  digitalWrite(SPEAKER, HIGH);
}

void deactivateAlert()
{
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_DANGER, LOW);
  digitalWrite(LED_SAFE, HIGH);
  digitalWrite(SPEAKER, LOW);
}

// Function to send an SMS via GSM
void SendMessage(String phoneNumber, String message)
{
  sendGSMCommand("AT+CMGS=\"" + phoneNumber + "\"");
  Sim.println(message);
  Sim.write(26);
}

// Function to receive incoming SMS
void ReceiveMessage()
{
  while (Sim.available())
  {
    Serial.write(Sim.read());
  }
}

// Function to send generic GSM commands
void sendGSMCommand(String command)
{
  Sim.println(command);
}

// Function to check GSM network status
bool checkNetwork()
{
  sendGSMCommand("AT+CREG?");
  while (Sim.available())
  {
    String response = Sim.readString();
    Serial.println(response);
    return (response.indexOf("+CREG: 0,1") >= 0 || response.indexOf("+CREG: 0,5") >= 0);
  }
  return false;
}

// Function to check & handle Blynk connection status
void checkBlynkConnection()
{
  static bool connected_message_sent = false;
  static bool disconnected_message_sent = false;

  if (Blynk.connected())
  {
    digitalWrite(INDICATOR_LED, HIGH);
    if (!connected_message_sent)
    {
      Serial.println("Blynk is connected.");
      connected_message_sent = true;
      disconnected_message_sent = false;
    }
  }
  else
  {
    digitalWrite(INDICATOR_LED, LOW);
    if (!disconnected_message_sent)
    {
      Serial.println("Blynk is not connected. Attempting to reconnect...");
      disconnected_message_sent = true;
      connected_message_sent = false;
      Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    }
  }
}