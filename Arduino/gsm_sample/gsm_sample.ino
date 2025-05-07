#include <SoftwareSerial.h>

// Define RX and TX pins for SoftwareSerial
SoftwareSerial Sim(10, 11);

void setup() {
  // Start serial communication with GSM module and debugging
  Sim.begin(9600); // GSM module baud rate
  Serial.begin(9600); // Debugging baud rate
  
  // Set SMS text mode
  Sim.println("AT+CMGF=1"); // Set to SMS mode
  delay(1000);
  
  // Enable message notifications
  Sim.println("AT+CNMI=2,2,0,0,0"); // Live SMS notifications
  delay(1000);
}

void loop() {
  // Example usage of SendMessage function
  String phoneNumber = "+1234567890"; // Replace with actual recipient's number
  String message = "Hello, this is a test message!"; // Replace with your message
  SendMessage(phoneNumber, message);

  // Call ReceiveMessage to check for incoming messages
  ReceiveMessage();
  delay(10000); // Wait before the next iteration
}

// Function to send a message
void SendMessage(String phoneNumber, String message) {
  Sim.println("AT+CMGF=1"); // Set GSM module to SMS mode
  delay(1000);

  Sim.print("AT+CMGS=\"");
  Sim.print(phoneNumber); // Add recipient's phone number
  Sim.println("\"");
  delay(1000);

  Sim.println(message); // Add message content
  delay(1000);

  Sim.write(26); // Send Ctrl+Z to end the message
  delay(3000);

  Serial.println("Message Sent: " + message); // Print to serial monitor for debugging
}

// Function to receive messages
void ReceiveMessage() {
  if (Sim.available() > 0) { // Check if data is available from the GSM module
    while (Sim.available()) {
      char c = Sim.read(); // Read each character from the serial buffer
      Serial.print(c);     // Print the incoming message to the serial monitor
    }
    Serial.println(); // Add a newline after the message
  }
}