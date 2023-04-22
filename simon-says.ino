#define redLed 2
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW); // set the red LED pin to LOW when the program starts
}

void blink(int ledPin) {
  digitalWrite(ledPin, HIGH);
  count++;
  Serial.println(count);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

void loop() {
  while (count < 3) {
    blink(redLed);
  }
}
