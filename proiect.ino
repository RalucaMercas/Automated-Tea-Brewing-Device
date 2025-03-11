#include <LiquidCrystal.h>
#include "pitches.h"
#include <Servo.h>

Servo servo1;
Servo servo2;
int servoPos1 = 180;
int servoPos2 = 20;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

bool isRunning = false;   
bool paused = false;       
int totalSeconds = 300;    
bool initialized = false;
bool finished = false;
unsigned long lastButtonPressTime = 0; 
const int debounceDelay = 200;        

int buzzerPin = 50;

const int redPin = 48;    
const int yellowPin = 49; 
const int greenPin = 46; 

int melody[] = {
  NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, 
  NOTE_G5, NOTE_E5, NOTE_C5, NOTE_G4, 
  NOTE_A4, NOTE_B4, NOTE_C5           
};

int noteDurations[] = {
  4, 4, 4, 2, 
  4, 4, 4, 2, 
  4, 4, 1      
};

void setup() {
  Serial.begin(9600);

  servo1.attach(23);
  servo2.attach(22);
  servo1.write(180);
  servo2.write(20);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Set Timer:       ");
  lcd.setCursor(0, 1);
  displayTime(totalSeconds); 

  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

}

void loop() {
  if(!isRunning && !finished){
    digitalWrite(redPin, HIGH);
  }

  int buttonValue = analogRead(A0); 
  unsigned long currentTime = millis();

  if (currentTime - lastButtonPressTime > debounceDelay) {
    if (buttonValue >= 0 && buttonValue <= 50) {
      // Right
    } else if ( buttonValue > 50 && buttonValue <= 200 && !initialized) {
      // Up 
      increaseTime();
      lastButtonPressTime = currentTime;
    } else if (buttonValue > 200 && buttonValue <= 400 && !initialized) {
      // Down
      decreaseTime();  
      lastButtonPressTime = currentTime;
    } else if (buttonValue > 400 && buttonValue <= 600) {
      // Left
      stopCounter();
      lastButtonPressTime = currentTime;
    } else if (buttonValue > 600 && buttonValue <= 800) {
      // Select
      startCounter();
      lastButtonPressTime = currentTime;
    }
  }

  if (!finished) {
    digitalWrite(yellowPin, HIGH);
    if (isRunning) {
      digitalWrite(redPin, LOW);

      if (totalSeconds > 0) {
        lcd.setCursor(0, 0);
        lcd.print("Preparing Tea... ");
        lcd.setCursor(0, 1);
        displayTime(totalSeconds--); 
        delay(1000);
      } else {
        stopCounter(); 
        lcd.setCursor(0, 0);
        lcd.print("Tea is Ready!    ");
        lcd.setCursor(0, 1);
        lcd.print("Enjoy Your Tea!");
        finished = true;
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, HIGH);

        int targetAngle = 20; 
        while (servoPos2 > targetAngle) {
          servoPos2--;
          servo2.write(servoPos2);
          delay(15);
        }
        servo2.write(20);
        playJingle();
      }
    } else {
      lcd.setCursor(0, 0);
      if (initialized) {
        lcd.print("Paused preparing ");
      } else {
        lcd.print("Set Timer:       ");
      }
      lcd.setCursor(0, 1);
      displayTime(totalSeconds); 
      handleJoystick();
    }
  }
}

void startCounter() {
  if (!isRunning) {
    initialized = true;
    isRunning = true;

    int targetPosition = 90;
    while (servoPos1 != targetPosition) {
      if (servoPos1 < targetPosition) {
        servoPos1++;
        servo1.write(servoPos1);
      } else if (servoPos1 > targetPosition) {
        servoPos1--;
        servo1.write(servoPos1);
      }
      delay(15); 
    }

    while (servoPos2 != targetPosition) {
      if (servoPos2 < targetPosition) {
        servoPos2++;
        servo2.write(servoPos2);
      } else if (servoPos2 > targetPosition) {
        servoPos2--;
        servo2.write(servoPos2);
      }
      delay(15);
    }

    lcd.setCursor(0, 0);
    lcd.print("Preparing Tea... ");
  }
}

void stopCounter() {
  if (isRunning) {
    isRunning = false;
    lcd.setCursor(0, 0);
    lcd.print("Paused preparing ");
  }
}

void increaseTime() {
  totalSeconds += 60; 
  if (totalSeconds > 3600) { 
    totalSeconds = 3600;
  }
  lcd.setCursor(0, 0);
  lcd.print("Time Increased: ");
  lcd.setCursor(0, 1);
  displayTime(totalSeconds); 
}

void decreaseTime() {
  totalSeconds -= 60; 
  if (totalSeconds < 60) {
    totalSeconds = 60;
  }
  lcd.setCursor(0, 0);
  lcd.print("Time Decreased: ");
  lcd.setCursor(0, 1);
  displayTime(totalSeconds); 
}

void displayTime(int secondsLeft) {
  int minutes = secondsLeft / 60;     
  int seconds = secondsLeft % 60;    

  if (minutes < 10) {
    lcd.print("0");
  }
  lcd.print(minutes);
  lcd.print(":");

  if (seconds < 10) {
    lcd.print("0"); 
  }
  lcd.print(seconds);
}

void playJingle() {
  for (int thisNote = 0; thisNote < 11; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];

    tone(buzzerPin, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    noTone(buzzerPin);
  }
}

void handleJoystick() {
  // servo1
  if (analogRead(15) < 200 && servoPos1 < 180) {
    servoPos1++;
    servo1.write(servoPos1);
  }
  if (analogRead(15) > 700 && servoPos1 > 0) {
    servoPos1--;
    servo1.write(servoPos1);
  }

  // servo2
  if (analogRead(14) < 200 && servoPos2 < 180) {
    servoPos2++;
    servo2.write(servoPos2);
  }
  if (analogRead(14) > 700 && servoPos2 > 0) {
    servoPos2--;
    servo2.write(servoPos2);
  }

  delay(15); 
}