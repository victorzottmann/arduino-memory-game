#define redLed 2
int count;

void setup() {
  Serial.begin(9600);
  pinMode(redLed, OUTPUT);
  count = 0;
}

void blink() {
  digitalWrite(redLed, HIGH);
  delay(1000);
  digitalWrite(redLed, LOW);
  delay(1000);
  
  count++;
}

void loop() {
  while (count <= 3) {
    blink();
  }
}
