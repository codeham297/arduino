#include <SoftwareSerial.h>

// Define pins
#define GSM_RX 5
#define GSM_TX 18
#define BUZZER 26
#define VIBRATION_SENSOR 14
#define SPEAKER 26
#define LED_DANGER 33
#define LED_SAFE 32
#define CAMERA_BTN 34

// Constants
const int delay_time = 50;      // Delay in milliseconds
const int check_interval = 100; // Sensor check interval in milliseconds

// Global variables
bool vibration_message_sent = false; // Tracks if vibration message has been sent
bool elephant_message_sent = false;  // Tracks if elephant message has been sent
unsigned long last_check_time = 0;   // Keeps track of the last sensor check

SoftwareSerial Sim(GSM_RX, GSM_TX); // SoftwareSerial for GSM communication

String phoneNumber = "+1234567890"; // Replace with the actual phone number
String vibration_message = "Vibration Detected!";
String elephant_message = "Elephant Detected!";
String system_status = "System Initialized"; // Initial system status message

void setup()
{
  // Serial.begin(9600);
  // Initialize GSM communication
  Sim.begin(19200);

  // Send GSM setup commands
  sendGSMCommand("AT+CMGF=1");         // Set SMS text mode
  sendGSMCommand("AT+CNMI=2,2,0,0,0"); // Enable live SMS notifications

  // Configure pins
  pinMode(BUZZER, OUTPUT);
  pinMode(VIBRATION_SENSOR, INPUT);
  pinMode(SPEAKER, OUTPUT);
  pinMode(LED_DANGER, OUTPUT);
  pinMode(LED_SAFE, OUTPUT);
  pinMode(CAMERA_BTN, INPUT);

  // Send system initialization message
  SendMessage(phoneNumber, system_status);
}

void loop()
{
  // Check sensors at regular intervals
  if (millis() - last_check_time >= check_interval)
  {
    last_check_time = millis();
    checkSensors(); // Monitor sensors and handle alerts
  }

  ReceiveMessage(); // Continuously check for incoming messages
}

// Function to monitor sensors and trigger actions
void checkSensors()
{
  bool vibration_detected = digitalRead(VIBRATION_SENSOR) == HIGH;
  bool camera_triggered = digitalRead(CAMERA_BTN) == HIGH;

  if (vibration_detected && !vibration_message_sent)
  {
    SendMessage(phoneNumber, vibration_message);
    vibration_message_sent = true; // Prevent repeated messages for vibration
  }
  else if (!vibration_detected)
  {
    vibration_message_sent = false; // Reset vibration message status
  }

  if (vibration_detected && camera_triggered && !elephant_message_sent)
  {
    activateAlert();
    SendMessage(phoneNumber, elephant_message);
    elephant_message_sent = true; // Prevent repeated messages for elephant detection
  }
  else if (!camera_triggered)
  {
    elephant_message_sent = false; // Reset elephant message status
    deactivateAlert();             // Deactivate alert if camera is not triggered
  }

  // Deactivate alert if no detections are active
  if (!digitalRead(VIBRATION_SENSOR) && ! digitalRead(CAMERA_BTN))
  {
    deactivateAlert();
  }
}

// Function to activate alert systems
void activateAlert()
{
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED_DANGER, HIGH);
  digitalWrite(LED_SAFE, LOW);
  digitalWrite(SPEAKER, HIGH);
}

// Function to deactivate alert systems
void deactivateAlert()
{
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_DANGER, LOW);
  digitalWrite(LED_SAFE, HIGH);
  digitalWrite(SPEAKER, LOW);
}

// Function to send a message
void SendMessage(String phoneNumber, String message)
{
  sendGSMCommand("AT+CMGS=\"" + phoneNumber + "\"");
  delay(delay_time);
  Sim.println(message); // Send the message text
  delay(delay_time);
  Sim.write(26); // CTRL+Z to end the SMS
  delay(delay_time * 2);
}

// Function to receive messages from the GSM module
void ReceiveMessage()
{
  if (Sim.available() > 0)
  {
    while (Sim.available())
    {
      char c = Sim.read();
      // Optional: Process the received character if needed
    }
  }
}

// Helper function to send generic GSM commands
void sendGSMCommand(String command)
{
  Sim.println(command);
  delay(delay_time); // Allow time for GSM processing
}