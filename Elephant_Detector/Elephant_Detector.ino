#include <SoftwareSerial.h>

int gsm_receiver = 0;
int gsm_transmitter = 1;
SoftwareSerial Sim(gsm_receiver, gsm_transmitter); // Define RX and TX pins for SoftwareSerial

int buzzer = 2;
int vibration_sensor = 3;
int speaker = 4;
int led_danger = 5;
int led_safe = 7;
int camera_btn = 6;
int delay_time = 1;//Thats a delay of 1ms
int message_status = 0;
void setup(){
  message_status = 0; // Initialize message status
  Sim.begin(9600); // GSM module baud rate
  Serial.begin(9600); // Debugging baud rate
  Sim.println("AT+CMGF=1"); // Set to SMS mode
  delay(delay_time); //Change the delay in live mode
  Sim.println("AT+CNMI=2,2,0,0,0"); // Live SMS notifications
  delay(delay_time); //Change the delay in live mode;

  pinMode(buzzer, OUTPUT);
  pinMode(vibration_sensor, INPUT);
  pinMode(speaker, OUTPUT);
  pinMode(led_danger, OUTPUT);
  pinMode(led_safe, OUTPUT);
  pinMode(camera_btn, INPUT);
  SendMessage("+1234567890", "System Initialized"); // Send message when system is initialized
}

void loop(){
  String phoneNumber = "+1234567890";
  if(digitalRead(vibration_sensor) == 1){
  if(digitalRead(camera_btn) == 1){
    if(!message_status){
    digitalWrite(buzzer, 1);
    digitalWrite(led_danger, 1);
    digitalWrite(led_safe, 0);
    digitalWrite(speaker, 1);
    SendMessage(phoneNumber, "Elephant Detected!"); // Send message when elephant is detected
    ReceiveMessage(); //Check for incoming messages
    message_status = 1;
    }

    delay(delay_time); // Wait before the next iteration

  }
  else{
    digitalWrite(buzzer, 0);
    digitalWrite(led_safe, 1);
    digitalWrite(speaker, 0);
    digitalWrite(led_danger, 0);
    message_status = 0; // Reset message status when no elephant is detected
  }
  }
  else{
    digitalWrite(buzzer, 0);
    digitalWrite(led_safe, 1);
    digitalWrite(speaker, 0);
    digitalWrite(led_danger, 0);
  }
}


void SendMessage(String phoneNumber, String message) {
  Sim.println("AT+CMGF=1"); // Set GSM module to SMS mode
  delay(delay_time);

  Sim.print("AT+CMGS=\"");
  Sim.print(phoneNumber); // Add recipient's phone number
  Sim.println("\"");
  delay(delay_time);

  Sim.println(message); // Add message content
  delay(delay_time);

  Sim.write(26); // Send Ctrl+Z to end the message
  delay(3*delay_time);

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