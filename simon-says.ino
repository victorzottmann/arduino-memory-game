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
int currentRound = 0;
int ledsAnswered = 0;

enum States {
  READY_FOR_NEXT_ROUND,
  USER_IS_PLAYING,
  VICTORY,
  GAME_OVER
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

// keeping this function here for reference
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
  delay(1000);
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

void blinkLedsForCurrentRound() {
  /* 
    Changed from LED_SEQUENCE_SIZE to check if the blink sequence would adapt accordingly.
    For example, if in round 1, blink once; if in round 2, blink twice, and so on.
    Remember not to leave it this way as it was just for testing
  */
  for (int i = 0; i < currentRound; i++) {
    blink(ledPins[i]);
  }
}

int currentGameState() {
  // Leave it like this for now. Need to think about how to change the LED_SEQUENCE_SIZE based on
  // the level of difficulty as well.
  if (currentRound <= LED_SEQUENCE_SIZE) {
    if (ledsAnswered == currentRound) {
      return READY_FOR_NEXT_ROUND;
    } else {
      return USER_IS_PLAYING;
    }
  } else {
    return VICTORY;
  }
}

void prepareNextRound() {
  currentRound++;
  /* 
    ledsAnswered needs to be reset in every round to count how many
    leds the user answered in a given round. 
    For example, if 3 out of 4 were answered then it's game over.
  */
  ledsAnswered = 0;

  if (currentRound < LED_SEQUENCE_SIZE) {
    blinkLedsForCurrentRound();
  }
}

void processUserInput() {
  ledsAnswered++;
}

void victoryBlinkSequence() {
  while (gameOverVictoryCount < 3) {
    digitalWrite(RED_LED, HIGH);
    delay(100);
    digitalWrite(GREEN_LED, HIGH);
    delay(100);
    digitalWrite(YELLOW_LED, HIGH);
    delay(100);
    digitalWrite(BLUE_LED, HIGH);
    delay(100);
    digitalWrite(RED_LED, LOW);
    delay(100);
    digitalWrite(GREEN_LED, LOW);
    delay(100);
    digitalWrite(YELLOW_LED, LOW);
    delay(100);
    digitalWrite(BLUE_LED, LOW);
    gameOverVictoryCount++;
  }
}

void gameOverBlinkSequence() {
  while (gameOverCount < 3) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
    delay(500);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    delay(500);
    gameOverCount++;
  }
}

void switchBetweenGameStates() {
  int state = currentGameState();
  switch (state) {
    case READY_FOR_NEXT_ROUND:
      Serial.println("Ready for the next round");
      prepareNextRound();
      break;
    case WAITING_FOR_USER_INPUT:
      Serial.println("Waiting for user input");
      processUserInput();
      break;
    case VICTORY:
      Serial.println("Hurray! You won the game!");
      victoryBlinkSequence();
      break;
    case GAME_OVER:
      Serial.println("Game over");
      gameOverBlinkSequence();
      break;
  }
  delay(500);
}

void loop() {
  switchBetweenGameStates();
}