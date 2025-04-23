#include <SoftwareSerial.h>

// Define pins
#define GSM_RX 0
#define GSM_TX 1
#define BUZZER 2
#define VIBRATION_SENSOR 3
#define SPEAKER 4
#define LED_DANGER 5
#define LED_SAFE 7
#define CAMERA_BTN 6

// Constants
const int delay_time = 50; // Delay in milliseconds
const int check_interval = 100; // Sensor check interval in milliseconds

// Global variables
int message_status = 0; // Tracks whether a message has already been sent
unsigned long last_check_time = 0; // Keeps track of the last sensor check

SoftwareSerial Sim(GSM_RX, GSM_TX); // SoftwareSerial for GSM communication

void setup() {
  // Initialize GSM communication
  Sim.begin(9600);
  
  // Send GSM setup commands
  sendGSMCommand("AT+CMGF=1"); // Set SMS text mode
  sendGSMCommand("AT+CNMI=2,2,0,0,0"); // Enable live SMS notifications
  
  // Configure pins
  pinMode(BUZZER, OUTPUT);
  pinMode(VIBRATION_SENSOR, INPUT);
  pinMode(SPEAKER, OUTPUT);
  pinMode(LED_DANGER, OUTPUT);
  pinMode(LED_SAFE, OUTPUT);
  pinMode(CAMERA_BTN, INPUT);
  
  // Send system initialization message
  SendMessage("+1234567890", "System Initialized");
}

void loop() {
  // Check sensors at regular intervals
  if (millis() - last_check_time >= check_interval) {
    last_check_time = millis();
    checkSensors(); // Monitor sensors and handle alerts
  }
  
  ReceiveMessage(); // Continuously check for incoming messages
}

// Function to monitor sensors and trigger actions
void checkSensors() {
  bool vibration_detected = digitalRead(VIBRATION_SENSOR) == HIGH;
  bool camera_triggered = digitalRead(CAMERA_BTN) == HIGH;

  if (vibration_detected && camera_triggered && message_status == 0) {
    activateAlert();
    SendMessage("+1234567890", "Elephant Detected!");
    message_status = 1; // Prevent repeated messages
  } else if (!vibration_detected || !camera_triggered) {
    deactivateAlert();
    message_status = 0; // Reset message status when no detection
  }
}

// Function to activate alert systems
void activateAlert() {
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED_DANGER, HIGH);
  digitalWrite(LED_SAFE, LOW);
  digitalWrite(SPEAKER, HIGH);
}

// Function to deactivate alert systems
void deactivateAlert() {
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_DANGER, LOW);
  digitalWrite(LED_SAFE, HIGH);
  digitalWrite(SPEAKER, LOW);
}

// Function to send a message
void SendMessage(String phoneNumber, String message) {
  sendGSMCommand("AT+CMGS=\"" + phoneNumber + "\"");
  delay(delay_time);
  Sim.println(message); // Send the message text
  delay(delay_time);
  Sim.write(26); // CTRL+Z to end the SMS
  delay(delay_time * 2);
}

// Function to receive messages from the GSM module
void ReceiveMessage() {
  if (Sim.available() > 0) {
    while (Sim.available()) {
      char c = Sim.read();
    }
  }
}

// Helper function to send generic GSM commands
void sendGSMCommand(String command) {
  Sim.println(command);
  delay(delay_time); // Allow time for GSM processing
}