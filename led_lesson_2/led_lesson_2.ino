int led_blue = 13;
int led_green = 12;
int led_yellow = 11;
int led_red = 10;
void setup() {
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_yellow,   OUTPUT);
  pinMode(led_red, OUTPUT);
  // put your setup code here, to run once:

}

void loop() {
  digitalWrite(led_blue, HIGH);
  digitalWrite(led_red, HIGH);
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_green, LOW);
  delay(500);
  digitalWrite(led_yellow, HIGH);
  digitalWrite(led_green, HIGH);
  digitalWrite(led_blue, LOW);
  digitalWrite(led_red, LOW);
  delay(500);
  
  // put your main code here, to run repeatedly:

}
