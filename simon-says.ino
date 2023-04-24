// pins
#define RED_BTN 11
#define GREEN_BTN 10
#define YELLOW_BTN 9
#define BLUE_BTN 8
#define RED_LED 5
#define GREEN_LED 4
#define YELLOW_LED 3
#define BLUE_LED 2
#define POT A0

// other constants
#define LED_SEQUENCE_SIZE 4
#define UNDEFINED -1

// global variables
int ledPins[LED_SEQUENCE_SIZE];
int btnPins[] = { RED_BTN, GREEN_BTN, YELLOW_BTN, BLUE_BTN };

enum States {
  READY_FOR_NEXT_ROUND,
  USER_IS_PLAYING,
  VICTORY,
  GAME_OVER_FAILURE
};

void setup() {
  Serial.begin(9600);
  startGame();
  setPins();
}

void startGame() {
  /* 
    This truly generates a random number given the potentiometer value
    However, if you disconnect the potentiometer, the number will continue to be random because
    Arduino will try to interpret the voltage of the air, which has a variable electrical charge 
    and which depends on several factors, like pressure, humidity, etc, to generate a random value. 
    If random() was to be called without the existence of randomSeed(), the random number 
    would actually always be the same. 
    In other words, randomSeed() feeds into random() with a random number 
  */
  int potLevel = analogRead(POT);
  randomSeed(potLevel);

  for (int i = 0; i < LED_SEQUENCE_SIZE; i++) {
    ledPins[i] = randomLedPicker();
  }
}

int randomLedPicker() {
  return random(BLUE_LED, RED_LED + 1);  // random from 2 to 5 (if no +1, the range would go between 2 and 4)
}

void setPins() {
  for (int i = 0; i < sizeof(btnPins); i++) {
    pinMode(btnPins[i], INPUT_PULLUP);  // set all the Button pins to INPUT_PULLUP mode
  }

  for (int i = 0; i < sizeof(ledPins); i++) {
    pinMode(ledPins[i], OUTPUT);    // set all the LED pins to OUTPUT mode
    digitalWrite(ledPins[i], LOW);  // set all the LED pins to LOW when the program starts
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

int blink(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
  return ledPin;
}

int verifyUserInput() {
  if (digitalRead(RED_BTN) == LOW) return blink(RED_LED);        // return the number associated with RED_LED when it blinks
  if (digitalRead(GREEN_BTN) == LOW) return blink(GREEN_LED);    // return the number associated with GREEN_LED when it blinks
  if (digitalRead(YELLOW_BTN) == LOW) return blink(YELLOW_LED);  // return the number associated with YELLOW_LED when it blinks
  if (digitalRead(BLUE_BTN) == LOW) return blink(BLUE_LED);      // return the number associated with BLUE_LED when it blinks
  return UNDEFINED;                                              // return a number outside the available pin range
}

void runCurrentLedSequence() {
  for (int i = 0; i < LED_SEQUENCE_SIZE; i++) {
    blink(ledPins[i]);
  }
}

int currentGameState() {
  return READY_FOR_NEXT_ROUND;
}

void loop() {
  runCurrentLedSequence();

  int state = currentGameState();
  switch (state) {
    case READY_FOR_NEXT_ROUND:
      Serial.println("Ready for the next round");
      break;
    case USER_IS_PLAYING:
      Serial.println("The user is playing");
      break;
    case VICTORY:
      Serial.println("Hurray! You won the game!");
      break;
    case GAME_OVER:
      Serial.println("Game over");
      break;
  }

  while (1) {}  // quick hack to stop the for loop above from repeating after it finishes
  // Serial.println(verifyUserInput());
}
