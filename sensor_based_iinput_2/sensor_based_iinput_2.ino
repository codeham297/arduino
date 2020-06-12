int led = 13;
int sensor = A0;
int status_;
void setup() {
  pinMode(led, OUTPUT);
  pinMode(sensor, INPUT);

}

void loop() {
  status_ = analogRead(sensor);
  if(status_ <= 300){
    digitalWrite(led, LOW);
  }
  else{
    digitalWrite(led, HIGH);
  }
}
