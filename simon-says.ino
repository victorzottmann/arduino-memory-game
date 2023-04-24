#define RED_BTN 11
#define GREEN_BTN 10
#define YELLOW_BTN 9
#define BLUE_BTN 8
#define RED_LED 5
#define GREEN_LED 4
#define YELLOW_LED 3
#define BLUE_LED 2
#define UNDEFINED -1

int ledPins[4];
int btnPins[] = { RED_BTN, GREEN_BTN, YELLOW_BTN, BLUE_BTN };

void setup() {
  Serial.begin(9600);
  startGame();
  setPins();
}

void startGame() {
  ledPins[0] = BLUE_LED;
  ledPins[1] = GREEN_LED;
  ledPins[2] = RED_LED;
  ledPins[3] = YELLOW_LED;
}

void setPins() {
  for (int i = 0; i < sizeof(btnPins); i++) {
    pinMode(btnPins[i], INPUT_PULLUP);  // set the Button pins to INPUT_PULLUP mode
  }

  for (int i = 0; i < sizeof(ledPins); i++) {
    pinMode(ledPins[i], OUTPUT);   // set the LED pins to OUTPUT mode
    digitalWrite(ledPins[i], LOW); // set the LED pins to LOW when the program starts
  }
}

void triggerBtnAndLed(int btnPin, int ledPin) {
  int btnState = digitalRead(btnPin);

  if (btnState == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}

int verifyUserInput() {
  if (digitalRead(RED_BTN) == LOW) {
    return RED_LED;                               // return the number associated with RED_LED
  } else if (digitalRead(GREEN_BTN) == LOW) {
    return GREEN_LED;                             // return the number associated with GREEN_LED
  } else if (digitalRead(YELLOW_BTN) == LOW) {
    return YELLOW_LED;                            // return the number associated with YELLOW_LED
  } else if (digitalRead(BLUE_BTN) == LOW) {
    return BLUE_LED;                              // return the number associated with BLUE_LED
  } else {
    return UNDEFINED;                             // return a number outside the available pin range
  }
}

void loop() {
  triggerBtnAndLed(RED_BTN, RED_LED);
  triggerBtnAndLed(GREEN_BTN, GREEN_LED);
  triggerBtnAndLed(YELLOW_BTN, YELLOW_LED);
  triggerBtnAndLed(BLUE_BTN, BLUE_LED);
  Serial.println(verifyUserInput());
}
