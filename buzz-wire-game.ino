#include <LiquidCrystal_I2C.h>
#include "pitches.h"

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x3F,16,2);

const int GREEN_LED = 12;
const int RED_LED = 13;
const int CIRCUIT = 9;
const int BUTTON = 2;
const int BUZZER = 10;

int gameInit = false;
int strikes = 0;
int gameWin = false;

unsigned long previousMillis = 0; // time last changed
const long interval = 1000; // interval between countdown
int count = 30; // challenge mode timer

unsigned long previousMillis2 = 0; // time last changed
const long interval2 = 200; // interval between countdown
bool greenOn = false;

unsigned long previousMillis3 = 0; // time last changed
const long interval3 = 100; // interval between countdo

// music

int songState = 0;

int melody[] = {
 NOTE_F4, NOTE_E4, NOTE_D4, NOTE_CS4,
 NOTE_C4, NOTE_B3, NOTE_AS3, NOTE_A3,
 NOTE_G3, NOTE_A3, NOTE_AS3, NOTE_A3, 
 NOTE_G3, NOTE_C4, 0, 
 
 NOTE_C4, NOTE_A3, NOTE_A3, NOTE_A3,
 NOTE_GS3, NOTE_A3, NOTE_F4, NOTE_C4, 
 NOTE_C4, NOTE_A3, NOTE_AS3, NOTE_AS3, 
 NOTE_AS3, NOTE_C4, NOTE_D4, 0, 
 
 NOTE_AS3, NOTE_G3, NOTE_G3, NOTE_G3,
 NOTE_FS3, NOTE_G3, NOTE_E4, NOTE_D4, 
 NOTE_D4, NOTE_AS3, NOTE_A3, NOTE_A3, 
 NOTE_A3, NOTE_AS3, NOTE_C4, 0,
 
 NOTE_C4, NOTE_A3, NOTE_A3, NOTE_A3,
 NOTE_GS3, NOTE_A3, NOTE_A4, NOTE_F4, 
 NOTE_F4, NOTE_C4, NOTE_B3, NOTE_G4, 
 NOTE_G4, NOTE_G4, NOTE_G4, 0,
 
 NOTE_G4, NOTE_E4, NOTE_G4, NOTE_G4,
 NOTE_FS4, NOTE_G4, NOTE_D4, NOTE_G4, 
 NOTE_G4, NOTE_FS4, NOTE_G4, NOTE_C4, 
 NOTE_B3, NOTE_C4, NOTE_B3, NOTE_C4, 0
};

int tempo[] = {
 8, 16, 8, 16,
 8, 16, 8, 16,
 16, 16, 16, 8,
 16, 8, 3,
 
 12, 16, 16, 16,
 8, 16, 8, 16,
 8, 16, 8, 16,
 8, 16, 4, 12,
 
 12, 16, 16, 16,
 8, 16, 8, 16,
 8, 16, 8, 16,
 8, 16, 4, 12,
 
 12, 16, 16, 16,
 8, 16, 8, 16,
 8, 16, 8, 16,
 8, 16, 4, 16,
 
 12, 17, 17, 17,
 8, 12, 17, 17, 
 17, 8, 16, 8,
 16, 8, 16, 8, 1 
};

int winMelody[] = {
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, //1
  NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, 
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,
};

int failMelody[] = {
  NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, //45
  NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
  NOTE_G4,8, NOTE_D4,8, NOTE_E4,-2,  
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int winNotes = sizeof(winMelody) / sizeof(winMelody[0]) / 2;
int failNotes = sizeof(failMelody) / sizeof(failMelody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / 200; // tempo: 200

int divider = 0, noteDuration = 0;

void setup() {
  pinMode(CIRCUIT, INPUT); // setup circuit
  pinMode(BUTTON, INPUT); // setup button
  pinMode(BUZZER, OUTPUT);
  
  pinMode(GREEN_LED, OUTPUT); // setup green led
  pinMode(RED_LED, OUTPUT); // setup red led

  lcd.init(); // setup LCD
  lcd.backlight(); //Encender la luz de fondo.
}

void loop() {
  if(!gameInit) {
    if(!gameWin) { 
      clickToPlay();
      if(digitalRead(BUTTON) == HIGH) {
        gameInit = true;
      }
    } else {
      youWin();
    }
  }
  else {
    showCountdown();
    if(count <= 0) {
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      printGameOver();
      resetGame();
    }
    if(digitalRead(CIRCUIT) == HIGH) {
      strikes++;
      buzz(BUZZER, NOTE_B0, 1000/24);
      delay(100);
      buzz(BUZZER, NOTE_B0, 1000/24);
      delay(100);
      buzz(BUZZER, NOTE_B0, 1000/24);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      if(strikes >= 3) {
        printGameOver();
        resetGame();
      }
      delay(200);
    }
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    if(digitalRead(BUTTON) == HIGH) {
      gameInit = false;
      gameWin = true;
    }
  }
}

void resetGame() {
  gameInit = false;
  gameWin = false;
  lcd.clear();
  strikes = 0;
  count = 30;
  greenOn = false;
  sing();
}

void clickToPlay() {
  lcd.setCursor(0, 0);
  lcd.print("Click the button");
  lcd.setCursor(0, 1);
  lcd.print("to PLAY!");
  unsigned long currentMillis = millis(); // current time
  if (currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    if(greenOn){
      digitalWrite(GREEN_LED, LOW);
      greenOn = false;
    } else {
      digitalWrite(GREEN_LED, HIGH);
      greenOn = true;
    }
  }
}

void youWin() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You WIN!");
  winSong();
  printSeconds();
  resetGame();
}

void showCountdown() {
  String strikesText = "Strikes: " + String(strikes) + "/3";
  lcd.setCursor(0,0);
  lcd.print(strikesText);

  String countText = String(count) + " seconds";
  lcd.setCursor(0,1);
  lcd.print(countText);
  delay(100);

  unsigned long currentMillis = millis(); // current time
  if (currentMillis - previousMillis >= interval) {
    lcd.clear();
    previousMillis = currentMillis;
    count--;
  }
}

void printGameOver() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GAME OVER");
  failSong();
  printSeconds();
}

void printSeconds() {
  for(int i = 5; i >= 1; i--) {
    String text = "Wait " + String(i) + " seconds.";
    lcd.setCursor(0,1);
    lcd.print(text);
    delay(1000);
  }
}

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000/frequency/2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length/ 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i=0; i < numCycles; i++){ // for the calculated length of time...
    digitalWrite(targetPin,HIGH); // write the buzzer pin high to push out the diaphragm
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin,LOW); // write the buzzer pin low to pull back the diaphragm
    delayMicroseconds(delayValue); // wait again for the calculated delay value
 }
}

void sing() {
 // play the song in a non blocking way
 unsigned long currentMillis = millis();

 if (currentMillis - previousMillis3 >= interval3) {
  previousMillis3 = currentMillis;
  int noteDuration = 1000 / tempo[songState];
  buzz(10, melody[songState], noteDuration);
  int pauseBetweenNotes = noteDuration;
  delay(pauseBetweenNotes);

  // stop the tone playing:
  buzz(10, 0, noteDuration);
  
  ++songState;
  // start song again if finished
  if(songState > 79) {
    songState = 14; // skip intro 
  } 
 }
}

void winSong() {
  for (int thisNote = 0; thisNote < winNotes * 2; thisNote = thisNote + 2) {
    // calculates the duration of each note
    divider = winMelody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER, winMelody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(BUZZER);
  }
}

void failSong() {
  for (int thisNote = 0; thisNote < failNotes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = failMelody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER, failMelody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(BUZZER);
  }
}