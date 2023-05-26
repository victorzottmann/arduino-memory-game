#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

// LED pins and extra potentiometer for random LED sequence values
const int redLed = 11;    // digital pin 11
const int greenLed = 10;  // digital pin 10
const int yellowLed = 9;  // digital pin 9
const int blueLed = 8;    // digital pin 8
const int pot = 0;        // analog pin A0
const int unusedPin = 1;  // analog pin A1

// Button pins
const int redBtn = 6;     // digital pin 6
const int greenBtn = 5;   // digital pin 5
const int yellowBtn = 4;  // digital pin 4
const int blueBtn = 3;    // digital pin 3
const int whiteBtn = 2;   // digital pin 2

// Other constants
const int ledSequenceSize = 4;
const int allRoundsCompleted = 5;
const int allRoundsNotCompleted = 6;
const int undefined = -1;

// Global variables
int ledPins[ledSequenceSize];
int btnPins[] = { redBtn, greenBtn, yellowBtn, blueBtn };
int currentRound = 0;
int ledsAnswered = 0;
int gameOverVictoryCount = 0;
int gameOverCount = 0;
int delayTime;
int inputCount = 1;
bool restartGame = false;
bool levelAssigned = false;
bool userIsPlaying = false;
String level = "";

// Game states
enum States {
  WELCOME,
  READY_FOR_NEXT_ROUND,
  WAITING_FOR_USER_INPUT,
  VICTORY,
  GAME_OVER
};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(redBtn, INPUT);
  pinMode(greenBtn, INPUT);
  pinMode(yellowBtn, INPUT);
  pinMode(blueBtn, INPUT);

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  welcome();
}

String getLevel() {
  int potValue = analogRead(pot);
  int potMapped = map(potValue, 1023, 0, 0, 100);

  if (potMapped >= 0 && potMapped <= 33) {
    level = "Easy";
    delayTime = 300;
  } else if (potMapped > 33 && potMapped <= 66) {
    level = "Medium";
    delayTime = 200;
  } else {
    level = "Hard";
    delayTime = 150;
  }

  return level;
}

void assignLevel() {
  level = getLevel();

  if (digitalRead(whiteBtn) == LOW && !levelAssigned) {
    Serial.println();
    Serial.print("Assigned level: ");
    Serial.print(level);

    levelAssigned = true;
  }
}

void welcome() {
  level = getLevel();

  Serial.println();
  Serial.print("Select level: ");
  Serial.print(level);

  while (!levelAssigned) {
    assignLevel();

    if (levelAssigned) {
      startGame();
    }
  }
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
  if (!levelAssigned) {
    return;
  }

  /* 
    This truly generates a random number given the potentiometer value
    However, if you disconnect the potentiometer, the number will continue to be random because
    Arduino will try to interpret the voltage of the air, which has a variable electrical charge 
    and which depends on several factors, like pressure, humidity, etc, to generate a random value. 
    If random() was to be called without the existence of randomSeed(), the random number 
    would actually always be the same. 
    In other words, randomSeed() feeds into random() with a random number 
  */
  int randomValue = analogRead(unusedPin);
  randomSeed(randomValue);

  delay(1000);

  Serial.println();
  Serial.println("Game started");
  Serial.print("LED sequence: ");
  for (int i = 0; i < ledSequenceSize; i++) {
    ledPins[i] = randomLedPicker();
    Serial.print(ledPins[i]);
    Serial.print(" ");
  }
}

int blink(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(delayTime);
  digitalWrite(ledPin, LOW);
  delay(delayTime);
  return ledPin;
}

int verifyUserInput() {
  bool redBtnIsPressed = digitalRead(redBtn) == LOW;
  bool greenBtnIsPressed = digitalRead(greenBtn) == LOW;
  bool yellowBtnIsPressed = digitalRead(yellowBtn) == LOW;
  bool blueBtnIsPressed = digitalRead(blueBtn) == LOW;

  if (redBtnIsPressed) {
    int result = blink(redLed);
    while (digitalRead(redBtn) == LOW) {}
    return result;  // return the number associated with redLed when it blinks (11)
  }

  if (greenBtnIsPressed) {
    int result = blink(greenLed);
    while (digitalRead(greenBtn) == LOW) {}
    return result;  // return the number associated with greenLed when it blinks (10)
  }

  if (yellowBtnIsPressed) {
    int result = blink(yellowLed);
    while (digitalRead(yellowBtn) == LOW) {}
    return result;  // return the number associated with yellowLed when it blinks (9)
  }

  if (blueBtnIsPressed) {
    int result = blink(blueLed);
    while (digitalRead(blueBtn) == LOW) {}
    return result;  // return the number associated with blueLed when it blinks (8)
  }

  return undefined;  // return a number outside the available pin range (-1)
}

void processUserInput() {
  int answer = verifyUserInput();

  if (answer == undefined) {
    return;
  }

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
    inputCount++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Input ");

    if (inputCount > currentRound) {
      inputCount = currentRound;
    }

    lcd.print(inputCount);
    lcd.print(" of ");
    lcd.print(currentRound);
    lcd.print(" V");
  } else {
    currentRound = allRoundsNotCompleted;
  }
}

void blinkLedsForCurrentRound() {
  /* 
    Changed from ledSequenceSize to check if the blink sequence would adapt accordingly.
    For example, if in round 1, blink once; if in round 2, blink twice, and so on.
  */
  for (int i = 0; i < currentRound; i++) {
    blink(ledPins[i]);
  }
}

int currentGameState() {
  if (!levelAssigned) {
    return WELCOME;
  }

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
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Round ");
    lcd.print(currentRound);

    lcd.setCursor(0, 1);
    lcd.print("Inputs: ");
    lcd.print(inputCount);
    lcd.print(" of ");
    lcd.print(currentRound);
    lcd.print(" N");

    delay(1000);

    blinkLedsForCurrentRound();
  }
}

void victoryBlinkSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You won!");

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

  lcd.setCursor(0, 0);
  lcd.print("Try again?");
}

void gameOverBlinkSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game over");

  while (gameOverCount < 5) {
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

  lcd.setCursor(0, 1);
  lcd.print("Try again?");
}

void restart() {
  currentRound = 0;
  ledsAnswered = 0;
  gameOverVictoryCount = 0;
  gameOverCount = 0;
  restartGame = false;
  levelAssigned = false;
  userIsPlaying = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Restarting game");

  lcd.setCursor(0, 1);
  lcd.print("...");

  delay(1000);
  welcome();
}

void switchBetweenGameStates(bool restartGame) {
  if (restartGame) {
    restart();
    return;
  }

  int state = currentGameState();
  switch (state) {
    case WELCOME:
      assignLevel();
      userIsPlaying = false;
      break;
    case READY_FOR_NEXT_ROUND:
      if (levelAssigned) {
        prepareNextRound();
      }
      break;
    case WAITING_FOR_USER_INPUT:
      processUserInput();
      userIsPlaying = true;
      break;
    case VICTORY:
      victoryBlinkSequence();
      userIsPlaying = true;
      break;
    case GAME_OVER:
      gameOverBlinkSequence();
      userIsPlaying = true;
      break;
  }
}

void loop() {
  bool whiteBtnClicked = digitalRead(whiteBtn) == LOW;
  if (whiteBtnClicked && userIsPlaying) restartGame = true;

  switchBetweenGameStates(restartGame);
}
