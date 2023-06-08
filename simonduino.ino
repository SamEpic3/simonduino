#include "pitches.h"

#include <ezButton.h>
#include "SimonLED.hpp"

//OLED display setup
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, A5, A4,U8X8_PIN_NONE);

//Y position for each display lines
#define DISPLAY_LINE_1 15
#define DISPLAY_LINE_2 31
#define DISPLAY_LINE_3 47
#define DISPLAY_LINE_4 63
#define DISPLAY_LINE_CENTER 39

//Define inputs and outputs pins numbers
#define LED_RED 6
#define LED_GREEN 7
#define LED_YELLOW 8
#define LED_BLUE 10

#define SW_RED 2
#define SW_GREEN 3
#define SW_YELLOW 4
#define SW_BLUE 5
#define NO_INPUT -1

#define BUZZER 9

//Create array of 4 ezButtons
ezButton btnsArray[] = {
  ezButton(SW_RED),
  ezButton(SW_GREEN),
  ezButton(SW_YELLOW),
  ezButton(SW_BLUE)
};

/*This variable will be set to the corresponding btnsArray index
  when a button is pressed in the WAITING_FOR_INPUT state*/
int inputSw = NO_INPUT; 

#define DEBOUNCE_TIME 50 //debounce time for buttons

//Array of notes corresponding to each inputs
int tonesArray[] = {
  NOTE_E4,
  NOTE_CS4,
  NOTE_A4,
  NOTE_E3
};

//Create array of 4 SimonLEDs
SimonLED LEDsArray[] = {
  SimonLED(LED_RED, BUZZER, tonesArray[0]),
  SimonLED(LED_GREEN, BUZZER, tonesArray[1]),
  SimonLED(LED_YELLOW, BUZZER, tonesArray[2]),
  SimonLED(LED_BLUE, BUZZER, tonesArray[3])
};

//Delay for LEDs blinking
#define LED_BLINK_DURATION 500

byte currentLevel = 1;
const byte tonesPerLevel = 31;
byte levelsArray[tonesPerLevel]; //Array of random tones from 0 to 3

//When tonesPlayed > currentTone, currentTone is incremented by 1 and the sequence is replayed
byte currentTone = 0; //number of tones played by Simon
byte tonesPlayed = 0; //number of tones played by the user

//Main program states
#define NO_STATE 0
#define PLAYING_SEQUENCE 1
#define WAITING_FOR_INPUT 2
byte currentState = NO_STATE;
unsigned long changeStateTimer = 0; //Timer before Simon plays the tones
#define changeStateDelay 2000 //Timer delay (ms)


void setup() {
    
  randomSeed(analogRead(0)); //Use analog pin 0 for generating random number
  
  u8g2.begin(); //Init display
  
  const char* displayString = "Loading...";
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.setCursor((128 - u8g2.getStrWidth(displayString)) / 2, DISPLAY_LINE_CENTER);
    u8g2.print(displayString);
  } while ( u8g2.nextPage() );


  for(int i = 0; i <= 3; i++) {
    btnsArray[i].setDebounceTime(DEBOUNCE_TIME);
  }

  pinMode(BUZZER, INPUT); //Temporarily set pin to INPUT to avoid noise

  simonReady();
}

void loop() {
  //Call loop function for ezButton and SimonLED
  for(int i = 0; i <= 3; i++) {
    btnsArray[i].loop();
    LEDsArray[i].loop();
  }

  //PLAYING_SEQUENCE state
  if(currentState == PLAYING_SEQUENCE) {
    if(changeStateTimer == 0) changeStateTimer = millis(); //Delay before playing sequence
    if(millis() - changeStateTimer >= changeStateDelay) {
      u8g2.firstPage();
      do {
        u8g2.setCursor(20, DISPLAY_LINE_2);
        u8g2.print("Level: ");
        u8g2.print(currentLevel);
        u8g2.setCursor(20, DISPLAY_LINE_3);
        u8g2.print("Tones: ");
        u8g2.print(currentTone+1);
      } while ( u8g2.nextPage() );
      for(byte i=0; i <= currentTone; i++) {
        LEDsArray[levelsArray[i]].blinkLED(LED_BLINK_DURATION);
        delay(LED_BLINK_DURATION);
        LEDsArray[levelsArray[i]].stopBlink();
        delay(100);
      }
      currentState = WAITING_FOR_INPUT;
      changeStateTimer = 0;
    }
  }

  //WAITING FOR INPUT
  if(currentState == WAITING_FOR_INPUT) {
    //If a button is pressed (rising) in btnsArray, set inputSw to corresponding button
    for(int i = 0; i <= 3; i++) {
      if(btnsArray[i].isPressed()) {
        inputSw = i; 
      }
    }

    //If any button was pressed, blink the corresponding LED
    if(inputSw != NO_INPUT) {
      LEDsArray[inputSw].blinkLED(LED_BLINK_DURATION);
    }

    //Input is correct
    if(inputSw == levelsArray[tonesPlayed]) {
      tonesPlayed++;
      //Sequence completed
      if(tonesPlayed > currentTone) {
        tonesPlayed = 0;
        currentTone++;
        currentState = PLAYING_SEQUENCE;
        //Level completed
        if(currentTone > tonesPerLevel-1) {
          delay(LED_BLINK_DURATION);
          LEDsArray[inputSw].stopBlink();
          delay(500);
          playLevelCompletedMelody();
          generateLevel();
          currentLevel++;
        }
      }
      
    }
    else if(inputSw != NO_INPUT) {
      delay(LED_BLINK_DURATION);
      LEDsArray[inputSw].stopBlink();
      delay(500);
      gameOver();
    }
  }
  inputSw = NO_INPUT;
}

void setLEDsHigh() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void setLEDsLow() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_BLUE, LOW);
}

void gameOver() {
  const char* displayString = "Game over!";
  u8g2.firstPage();
  do {
    u8g2.setCursor((128 - u8g2.getStrWidth(displayString)) / 2, DISPLAY_LINE_CENTER);
    u8g2.print("Game over!");
  } while ( u8g2.nextPage() );
  playGameOverMelody();
  startNewGame();
}

void simonReady() {
  const char* displayString = "Simonduino";
  u8g2.firstPage();
  do {
    u8g2.setCursor((128 - u8g2.getStrWidth(displayString)) / 2, DISPLAY_LINE_CENTER);
    u8g2.print(displayString);
  } while ( u8g2.nextPage() );
  
  playSimonMelody();
  startNewGame();
}

void generateLevel() {
  for(int i = 0; i < tonesPerLevel; i++) {
    levelsArray[i] = random(4);
  }
  tonesPlayed = 0;
  currentTone = 0;
  inputSw = NO_INPUT;
}

void startNewGame() {
  generateLevel();
  currentLevel = 1;
  currentState = PLAYING_SEQUENCE;
}

void playSimonMelody() {
  pinMode(BUZZER, OUTPUT);
  
  int notes[] = {
    NOTE_AS4, 0, NOTE_AS4, NOTE_AS4, NOTE_AS4, 0, NOTE_AS4, 0, NOTE_AS4, 0,
    NOTE_AS4, 0, NOTE_C5, 0, NOTE_C5, NOTE_C5, NOTE_C5, 0, NOTE_C5, 0, NOTE_C5, 0,
    NOTE_C5, 0, NOTE_F4, 0, NOTE_F4, NOTE_F4, NOTE_F4, 0, NOTE_F4, 0, NOTE_F4
  };

  byte duration[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 4, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4
  };

  byte ledOnPosition = 3;
  const int MELODY_LENGTH = 33;
  const int NOTE_DURATION = 60;
  const int PAUSE_BETWEEN_NOTES = NOTE_DURATION * 1.30;

  for(int i = 0; i < MELODY_LENGTH; i++) {
    
    //LEDS
    LEDsArray[ledOnPosition].setState(LOW);
    if(notes[i] != 0) {
      ledOnPosition++;
      if(ledOnPosition > 3) ledOnPosition = 0;
      LEDsArray[ledOnPosition].setState(HIGH);
    }

    //BUZZER
    if(notes[i] != 0) {
      tone(BUZZER, notes[i], duration[i] * NOTE_DURATION);
    }
    else {
      delay(duration[i] * NOTE_DURATION);
    }
    delay(PAUSE_BETWEEN_NOTES * duration[i]);
    noTone(BUZZER);
  }

  LEDsArray[ledOnPosition].setState(LOW);
  
  pinMode(BUZZER, INPUT);
}

void playGameOverMelody() {
  pinMode(BUZZER, OUTPUT);
  
  int notes[] = {
    NOTE_C3, 0, NOTE_C3, 0, NOTE_C3, 0, NOTE_C3
  };

  byte duration[] = {
    1, 1, 1, 1, 1, 1, 1
  };

  const int MELODY_LENGTH = 7;
  const int NOTE_DURATION = 60;
  const int PAUSE_BETWEEN_NOTES = NOTE_DURATION * 1.30;

  for(int i = 0; i < MELODY_LENGTH; i++) {
    
    //LEDS
    if(notes[i] != 0) setLEDsHigh();

    //BUZZER
    if(notes[i] != 0) {
      tone(BUZZER, notes[i], duration[i] * NOTE_DURATION);
    }
    else {
      delay(duration[i] * NOTE_DURATION);
    }

    delay(PAUSE_BETWEEN_NOTES * duration[i]);
    setLEDsLow();
    noTone(BUZZER);
  }
  
  pinMode(BUZZER, INPUT);
}

void playLevelCompletedMelody() {
  pinMode(BUZZER, OUTPUT);

  byte ledOnPosition = 0;
  
  int notes[] = {
    NOTE_G3, 0, NOTE_B3, 0, NOTE_D4, 0, NOTE_G4
  };

  byte duration[] = {
    1, 1, 1, 1, 1, 1, 1
  };

  const int MELODY_LENGTH = 7;
  const int NOTE_DURATION = 100;
  const int PAUSE_BETWEEN_NOTES = NOTE_DURATION * 1.30;

  for(int i = 0; i < MELODY_LENGTH; i++) {
    
    //LEDS
    if(notes[i] != 0) {
      LEDsArray[ledOnPosition].setState(HIGH);
      ledOnPosition++;
    }

    //BUZZER
    if(notes[i] != 0) {
      tone(BUZZER, notes[i], duration[i] * NOTE_DURATION);
    }
    else {
      delay(duration[i] * NOTE_DURATION);
    }

    delay(PAUSE_BETWEEN_NOTES * duration[i]);
    
    noTone(BUZZER);
  }

  setLEDsLow();
  pinMode(BUZZER, INPUT);
}
