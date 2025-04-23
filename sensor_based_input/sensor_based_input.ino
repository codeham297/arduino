int led = 13;
int sensor = 4;
int status_;
void setup() {
  pinMode(led, OUTPUT);
  pinMode(sensor, INPUT);

}

void loop() {
  status_ = digitalRead(sensor);
  if(status_ == HIGH){
    digitalWrite(led, HIGH);
  }
  else{
    digitalWrite(led, LOW);
  }
}
