#define RED_LED 5
#define GREEN_LED 4
#define YELLOW_LED 3
#define BLUE_LED 2

int count = 0;
int ledList[4];

void setup() {
  Serial.begin(9600);
  startGame();
  initialiseLedPins();      // set the LED pins to OUTPUT mode
  setLedsToLowOnStartUp();  // set the LED pins to LOW when the program starts
}

void initialiseLedPins() {
  for (int i = 0; i < sizeof(ledList); i++) {
    pinMode(ledList[i], OUTPUT);
  }
}

void setLedsToLowOnStartUp() {
  for (int i = 0; i < sizeof(ledList); i++) {
    digitalWrite(ledList[i], LOW);
  }
}

void startGame() {
  ledList[0] = BLUE_LED;
  ledList[1] = GREEN_LED;
  ledList[2] = RED_LED;
  ledList[3] = YELLOW_LED;
}

void blink(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
}

void loop() {
  for (int i = 0; i < sizeof(ledList); i++) {
    blink(ledList[i]);
  }

  while (true) {} // just so it runs the for loop above only once
}
