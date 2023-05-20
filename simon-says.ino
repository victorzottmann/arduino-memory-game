const int redBtn = 11;
const int greenBtn = 10;
const int yellowBtn = 9;
const int blueBtn = 8;

const int redLed = 5;
const int greenLed = 4;
const int yellowLed = 3;
const int blueLed = 2;
const int pot = 0;

// other constants
const int ledSequenceSize = 4;
const int allRoundsCompleted = 5;
const int allRoundsNotCompleted = 6;
const int undefined = -1;

// global variables
int ledPins[ledSequenceSize];
int btnPins[] = { redBtn, greenBtn, yellowBtn, blueBtn };
int currentRound = 0;
int ledsAnswered = 0;
int gameOverVictoryCount = 0;
int gameOverCount = 0;

// game states
enum States {
  READY_FOR_NEXT_ROUND,
  WAITING_FOR_USER_INPUT,
  VICTORY,
  GAME_OVER
};

void setup() {
  Serial.begin(9600);

  pinMode(redBtn, INPUT);
  pinMode(greenBtn, INPUT);
  pinMode(yellowBtn, INPUT);
  pinMode(blueBtn, INPUT);

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  startGame();
}

int randomLedPicker() {
  /*
    Random from 2 to 5 (if no +1, the range would go between 2 and 4)
    I'm using the LED constants instead of numbers to make sure that
    the randomisation happens for the LEDs themselves in case I change their pins.
  */
  return random(blueLed, redLed + 1);
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
  int potLevel = analogRead(pot);
  randomSeed(potLevel);

  for (int i = 0; i < ledSequenceSize; i++) {
    ledPins[i] = randomLedPicker();
    Serial.print(ledPins[i]);
    Serial.print(" ");
  }
  Serial.println("Game started");
}

int blink(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
  delay(300);
  return ledPin;
}

int verifyUserInput() {
  bool redBtnIsClicked = digitalRead(redBtn) == HIGH;
  bool greenBtnIsClicked = digitalRead(greenBtn) == HIGH;
  bool yellowBtnIsClicked = digitalRead(yellowBtn) == HIGH;
  bool blueBtnIsClicked = digitalRead(blueBtn) == HIGH;

  if (redBtnIsClicked) return blink(redLed);        // return the number associated with redLed when it blinks
  if (greenBtnIsClicked) return blink(greenLed);    // return the number associated with greenLed when it blinks
  if (yellowBtnIsClicked) return blink(yellowLed);  // return the number associated with yellowLed when it blinks
  if (blueBtnIsClicked) return blink(blueLed);      // return the number associated with blueLed when it blinks

  return undefined;  // return a number outside the available pin range
}

void blinkLedsForCurrentRound() {
  /* 
    Changed from ledSequenceSize to check if the blink sequence would adapt accordingly.
    For example, if in round 1, blink once; if in round 2, blink twice, and so on.
    Remember not to leave it this way as it was just for testing
  */
  for (int i = 0; i < currentRound; i++) {
    blink(ledPins[i]);
  }
}

int currentGameState() {
  /*
    Leave it like this for now. Need to think about how to change the ledSequenceSize based on
    the level of difficulty as well.
    This also looks very ugly even though it works. Figure out a way to simplify it.
  */
  if (currentRound <= ledSequenceSize) {
    if (ledsAnswered == currentRound) {
      return READY_FOR_NEXT_ROUND;
    } else {
      return WAITING_FOR_USER_INPUT;
    }
  } else if (currentRound == allRoundsCompleted) {
    return VICTORY;
  } else {
    return GAME_OVER;
  }
}

void prepareNextRound() {
  delay(800);
  /* 
    ledsAnswered needs to be reset in every round to count how many
    leds the user answered in a given round. 
    For example, if only 3 correct out of 4 were answered then it's game over.
  */
  ledsAnswered = 0;
  currentRound++;
  if (currentRound <= ledSequenceSize) {
    blinkLedsForCurrentRound();
  }

  Serial.print("Round ");
  Serial.print(currentRound);
}

void processUserInput() {
  int answer = verifyUserInput();

  if (answer == undefined) {
    return;
  }

  // Serial.println(ledsAnswered); // first
  // Serial.println(ledPins[ledsAnswered]); // second
  // Serial.println(currentRound);

  /* 
    currentRound = 1
    Assume that the ledPins sequence is { 2, 4, 5, 2 }, or { blue, green, red, blue }
    and that ledsAnswered is 0 in the first round.
    If the user presses the blue button then the condition will be true since the
    answer (pin 2) is equal to the ledPins[0] (blue).

    currentRound = 2
    In round 2, the sequence will be { 2, 4 }, or { blue, green }.
    ledsAnswered is reset to 0.
    ledsAnswered[0] = 2, ledsAnswered++ 
    ledsAnswered[1] = 4, ledsAnswered++
  */
  if (answer == ledPins[ledsAnswered]) {
    ledsAnswered++;
  } else {
    // Serial.println("Wrong answer!");
    currentRound = allRoundsNotCompleted;
  }
}

void victoryBlinkSequence() {
  while (gameOverVictoryCount < 3) {
    digitalWrite(redLed, HIGH);
    delay(100);
    digitalWrite(greenLed, HIGH);
    delay(100);
    digitalWrite(yellowLed, HIGH);
    delay(100);
    digitalWrite(blueLed, HIGH);
    delay(100);
    digitalWrite(redLed, LOW);
    delay(100);
    digitalWrite(greenLed, LOW);
    delay(100);
    digitalWrite(yellowLed, LOW);
    delay(100);
    digitalWrite(blueLed, LOW);
    gameOverVictoryCount++;
  }
}

void gameOverBlinkSequence() {
  while (gameOverCount < 3) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, HIGH);
    delay(500);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    delay(500);
    gameOverCount++;
  }
}

void switchBetweenGameStates() {
  int state = currentGameState();
  switch (state) {
    case READY_FOR_NEXT_ROUND:
      // Serial.println("Ready for the next round");
      prepareNextRound();
      break;
    case WAITING_FOR_USER_INPUT:
      // Serial.println("Waiting for user input");
      processUserInput();
      break;
    case VICTORY:
      // Serial.println("Hurray! You won the game!");
      victoryBlinkSequence();
      break;
    case GAME_OVER:
      // Serial.println("Game over");
      gameOverBlinkSequence();
      break;
  }
}

void loop() {
  switchBetweenGameStates();
}
