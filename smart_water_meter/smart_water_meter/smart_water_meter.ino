#define led 2

void setup() {
  pinMode(led, OUTPUT);
  // put your setup code here, to run once:

}

void loop() {
  digitalWrite(led, HIGH);
  delay(100); // wait for a second
  digitalWrite(led, LOW);
  delay(100); // wait for a second
  digitalWrite(led, HIGH);
  delay(100); // wait for a second
  digitalWrite(led, LOW);
  delay(3000); // wait for a second
}
