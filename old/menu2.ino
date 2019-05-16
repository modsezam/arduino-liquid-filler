#include <Indio.h>
#include <Wire.h>
#include <UC1701.h>

static UC1701 lcd;

// constants won't change. They're used here to set pin numbers:
//const int buttonPin = 24;    // the number of the pushbutton pin
//const int ledPin = 26;      // the number of the LED pin

const int backlightPin = 26; // PWM output pin that the LED backlight is attached to
const int buttonEnterPin = 24;
const int buttonUpPin = 25;
const int buttonDownPin = 23;
const int D0 = 0; // pompa 1, 2
const int D1 = 1; // zawor 1
const int D2 = 2; // zawor 2
const int D3 = 3;
const int D4 = 4;
const int D5 = 5;
const int D6 = 6;
//const int D7 = 7;
const int buttonExternalPin = 8; // przycisk

// Variables will change:
int ledState = LOW;         // the current state of the output pin
int buttonEnterState;             // the current reading from the input pin
int buttonUpState;             // the current reading from the input pin
int buttonDownState;             // the current reading from the input pin
int buttonExternalState;             // the current reading from the input pin
int lastButtonEnterState = LOW;   // the previous reading from the input pin
int lastButtonUpState = LOW;   // the previous reading from the input pin
int lastButtonDownState = LOW;   // the previous reading from the input pin
int lastButtonExternalState = LOW;   // the previous reading from the input pin

int backlightIntensity = LOW;        // LCD backlight intesity

int test = 0;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceEnterTime = 0;  // the last time the output pin was toggled
unsigned long lastDebounceUpTime = 0;  // the last time the output pin was toggled
unsigned long lastDebounceDownTime = 0;  // the last time the output pin was toggled
unsigned long lastDebounceExternalTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 40;    // the debounce time; increase if the output flickers

int buttonExternalPress = LOW;
int buttonEnterPress = LOW;
int buttonUpPress = LOW;
int buttonDownPress = LOW;

int executionPumpOn = LOW;
int executionPumpOff = LOW;
int executionValve1On = LOW;
int executionValve1Off = LOW;
int executionValve2On = LOW;
int executionValve2Off = LOW;

long timerPumpOn = 10;
long timerPumpOff = 9800;
long timerValve1On = 701;
long timerValve1Off = 9801;
long timerValve2On = 700;
long timerValve2Off = 9880;
long timers[] = { timerPumpOn, timerPumpOff, timerValve1On, timerValve1Off, timerValve2On, timerValve2Off };

int runFillingBottles = LOW;
int menuPosition = 0;
int subMenu = 0;
int menuPositionMax = 0;
int editSettings = LOW;
int bottleSize = 1;

unsigned long lastMillisTimeFill = 0;

void setup() {

  pinMode(backlightPin, OUTPUT); //set backlight pin to output 
  digitalWrite (backlightPin, backlightIntensity); //convert backlight intesity from a value of 0-5 to a value of 0-255 for PWM.
  pinMode(buttonEnterPin, INPUT);
  pinMode(buttonUpPin, INPUT);
  pinMode(buttonDownPin, INPUT);
  //pinMode(buttonExternalPin, INPUT);
  
  Indio.digitalMode(buttonExternalPin, INPUT);
  Indio.digitalMode(D1, OUTPUT);
  Indio.digitalMode(D2, OUTPUT);
  Indio.digitalMode(D3, OUTPUT);
  Indio.digitalWrite(D1,LOW);
  Indio.digitalWrite(D2,LOW);
  Indio.digitalWrite(D3,LOW);

  lcd.begin();  //sets the resolution of the LCD screen

  menuPositionMax = 5;
  lcd.clear();
  lcd.setCursor(99, 0); //set the cursor to the fifth pixel from the left edge, third row.
  lcd.print("[1L]    ");
  lcd.setCursor(0, 1); //set the cursor to the fifth pixel from the left edge, third row.
  lcd.print(">");
  lcd.setCursor(7, 0); //set the cursor to the fifth pixel from the left edge, third row.
  lcd.print("[1]Time set:");
  
  printTimers();
  
  //debug  
  Serial.begin(9600); //enables Serial port for debugging messages
  delay(1000);
}

void loop() {
 
  readButtons();

  /* ----------------- EXTERNAL BUTTON ------------------- */
  //po nacisnieciu przycisku external uruchomienie timera i aktywacja jednorazowa pompy i zaworow
  if (buttonExternalPress == HIGH){
    buttonExternalPress = LOW;
    
    if (runFillingBottles == LOW){
      menuPosition = 0;
      lcd.setCursor(0, (menuPosition + 1));
      lcd.print(">");
      lcd.setCursor(7, 7);
      lcd.print("RUN.Filling bottles    ");
      runFillingBottles = HIGH;
      executionPumpOn = 1;
      executionPumpOff = 1;
      executionValve1On = 1;
      executionValve1Off = 1;
      executionValve2On = 1;
      executionValve2Off = 1;
      lastMillisTimeFill = millis();
    }
    else if (runFillingBottles == HIGH){
      runFillingBottles = LOW;
      executionPumpOn = 0;
      executionPumpOff = 0;
      executionValve1On = 0;
      executionValve1Off = 0;
      executionValve2On = 0;
      executionValve2Off = 0;
      lastMillisTimeFill = 0;
      Indio.digitalWrite(D1,LOW);
      Indio.digitalWrite(D2,LOW);
      Indio.digitalWrite(D3,LOW);
      lcd.setCursor(7, 7);
      lcd.print("STOP                   ");
    }
    
  }
  
  /* ----------------- ENTER BUTTON ------------------- */
  if (buttonEnterPress == HIGH){
    buttonEnterPress = LOW;
    
    if (editSettings == LOW){
      editSettings = HIGH;
      lcd.setCursor(0, 0);
      lcd.print("*");
      //Serial.println("editSettings - HIGH");
    }
    else {
      editSettings = LOW;
      lcd.setCursor(0, 0);
      lcd.print(" ");
      //Serial.println("editSettings - LOW");
    }

    // wybor butelki 1 czy 5 L
    if (menuPosition == -1){
      lcd.setCursor(0, 0);
      lcd.print(">");
      editSettings = LOW;
      if (bottleSize == 1){
        lcd.setCursor(99, 0); //set the cursor to the fifth pixel from the left edge, third row.
        lcd.print("[5L]  ");
        bottleSize = 5;
        timers[0] = 10;
        timers[1] = 46000;
        timers[2] = 701;
        timers[3] = 45801;
        timers[4] = 700;
        timers[5] = 45800;
        printTimers();
      }
      else if (bottleSize == 5){
        lcd.setCursor(99, 0); //set the cursor to the fifth pixel from the left edge, third row.
        lcd.print("[1L]  ");
        bottleSize = 1;
        timers[0] = 10;
        timers[1] = 9800;
        timers[2] = 701;
        timers[3] = 9801;
        timers[4] = 700;
        timers[5] = 9880;
        printTimers();
      }
    }

    
    if (menuPosition == 6){
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.setCursor(0, (menuPosition + 1));
      lcd.print(" ");
      menuPosition = 0;
      lcd.setCursor(0, (menuPosition + 1));
      lcd.print(">");
      lcd.setCursor(0, 7);
      lcd.print("                    ");
      subMenu = 1;
      editSettings = LOW;
      
      // zapisywanie do EEPROM - do zrobienia
      
    }

    
  }

  /* ----------------- UP BUTTON ------------------- */
  if (buttonUpPress == HIGH){
    buttonUpPress = LOW;
    if (editSettings == LOW){
      lcd.setCursor(0, (menuPosition + 1));
      lcd.print(" ");
      if (menuPosition < 0){
        menuPosition = menuPositionMax + 1;
      }
      else {
        menuPosition--;
      }
      lcd.setCursor(0, (menuPosition + 1));
      lcd.print(">");
      //lcd.setCursor(0, 7);
      //lcd.print(menuPosition);
      lcd.setCursor(7, 7);
      lcd.print("[Save]                              ");
    }
    else {
      timers[menuPosition]++;
      lcd.setCursor(78, (menuPosition + 1));
      lcd.print(timers[menuPosition]);
      lcd.print("  ");
      lcd.setCursor(7, 7);
      lcd.print("[Save]                            ");
    }
  }


  /* ----------------- DOWN BUTTON ------------------- */
  if (buttonDownPress == HIGH){
    buttonDownPress = LOW;
    
    if (editSettings == LOW){
      lcd.setCursor(0, (menuPosition + 1));
      lcd.print(" ");
      if (menuPosition > menuPositionMax){
        menuPosition = -1;
      }
      else {
        menuPosition++;
      }
      lcd.setCursor(0, (menuPosition + 1));
      lcd.print(">");
      //lcd.setCursor(0, 7);
      //lcd.print(menuPosition);
      lcd.setCursor(7, 7);
      lcd.print("[Save]                                ");
      }
    if (editSettings == HIGH) {
      timers[menuPosition]--;
      lcd.setCursor(78, (menuPosition + 1));
      lcd.print(timers[menuPosition]);
      lcd.print("  ");
      lcd.setCursor(7, 7);
      lcd.print("[Save]                                ");
    }
  }

  
  
  if (executionPumpOn == 1 && (millis() - lastMillisTimeFill) >= timers[0]){
    executionPumpOn = 0;
    //lastMillisTimeFill = millis();
    Indio.digitalWrite(D1,HIGH);
    //Serial.print("PUMP UP - ");
    //Serial.println(millis() - lastMillisTimeFill);
    
  }
  if (executionPumpOff == 1 && (millis() - lastMillisTimeFill) >= timers[1]){
    executionPumpOff = 0;
    //lastMillisTimeFill = millis();
    Indio.digitalWrite(D1,LOW);
    //Serial.print("PUMP DOWN - ");
    //Serial.println(millis() - lastMillisTimeFill);
    runFillingBottles = LOW;
    lcd.setCursor(7, 7);
    lcd.print("END                         ");
  }
  if (executionValve1On == 1 && (millis() - lastMillisTimeFill) >= timers[2]){
    executionValve1On = 0;
    //lastMillisTimeFill = millis();
    Indio.digitalWrite(D2,HIGH);
    //Serial.print("VALVE 1 UP - ");
    //Serial.println(millis() - lastMillisTimeFill);
  }
  if (executionValve1Off == 1 && (millis() - lastMillisTimeFill) >= timers[3]){
    executionValve1Off = 0;
    //lastMillisTimeFill = millis();
    Indio.digitalWrite(D2,LOW);
    //Serial.print("VALVE 1 DOWN - ");
    //Serial.println(millis() - lastMillisTimeFill);
  }
  if (executionValve2On == 1 && (millis() - lastMillisTimeFill) >= timers[4]){
    executionValve2On = 0;
    //lastMillisTimeFill = millis();
    Indio.digitalWrite(D3,HIGH);
    //Serial.print("VALVE 2 UP - ");
    //Serial.println(millis() - lastMillisTimeFill);
  }
  if (executionValve2Off == 1 && (millis() - lastMillisTimeFill) >= timers[5]){
    executionValve2Off = 0;
    //lastMillisTimeFill = millis();
    Indio.digitalWrite(D3,LOW);
    //Serial.print("VALVE 2 DOWN - ");
    //Serial.println(millis() - lastMillisTimeFill);
  }
  
}



void printTimers(){
  lcd.setCursor(7, 1); //set the cursor to the fifth pixel from the left edge, third row.
  lcd.print("tPumpOn:");
  lcd.setCursor(78, 1);
  lcd.print(timers[0]); //print text on screen
  lcd.print("     ");
  lcd.setCursor(7, 2);
  lcd.print("tPumpOff:");
  lcd.setCursor(78, 2);
  lcd.print(timers[1]);
  lcd.print("     ");
  lcd.setCursor(7, 3);
  lcd.print("tValve1On:");
  lcd.setCursor(78, 3);
  lcd.print(timers[2]);
  lcd.print("     ");
  lcd.setCursor(7, 4);
  lcd.print("tValve1Off:");
  lcd.setCursor(78, 4);
  lcd.print(timers[3]);
  lcd.print("     ");
  lcd.setCursor(7, 5);
  lcd.print("tValve2On:");
  lcd.setCursor(78, 5);
  lcd.print(timers[4]);
  lcd.print("     ");
  lcd.setCursor(7, 6);
  lcd.print("tValve2Off:");
  lcd.setCursor(78, 6);
  lcd.print(timers[5]);
  lcd.print("     ");
}




void readButtons() {

  // read the state of the switch into a local variable:
  int readingEnter = digitalRead(buttonEnterPin);
  int readingUp = digitalRead(buttonUpPin);
  int readingDown = digitalRead(buttonDownPin);
  int readingExternal = Indio.digitalRead(buttonExternalPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  
  if (readingEnter != lastButtonEnterState) {
    // reset the debouncing timer
    lastDebounceEnterTime = millis();
  }

  if (readingUp != lastButtonUpState) {
    // reset the debouncing timer
    lastDebounceUpTime = millis();
  }

  if (readingDown != lastButtonDownState) {
    // reset the debouncing timer
    lastDebounceDownTime = millis();
  }

  if (readingExternal != lastButtonExternalState) {
    // reset the debouncing timer
    lastDebounceExternalTime = millis();
    //lcd.print("y");
  }

  if ((millis() - lastDebounceEnterTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingEnter != buttonEnterState) {
      buttonEnterState = readingEnter;

      // only toggle the LED if the new button state is HIGH
      if (buttonEnterState == LOW) {
        //ledState = !ledState;
        //lcd.print("E");
        buttonEnterPress = HIGH;
      }
    }
  }
  
  if ((millis() - lastDebounceUpTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingUp != buttonUpState) {
      buttonUpState = readingUp;

      // only toggle the LED if the new button state is HIGH
      if (buttonUpState == LOW) {
        //ledState = !ledState;
        //lcd.print("U");
        buttonUpPress = HIGH;
      }
    }
  }

  if ((millis() - lastDebounceDownTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingDown != buttonDownState) {
      buttonDownState = readingDown;

      // only toggle the LED if the new button state is HIGH
      if (buttonDownState == LOW) {
        //ledState = !ledState;
        //lcd.print("D");
        buttonDownPress = HIGH;
      }
    }
  }

  if ((millis() - lastDebounceExternalTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingExternal != buttonExternalState) {
      buttonExternalState = readingExternal;
      //lcd.print("y");

      // only toggle the LED if the new button state is HIGH
      if (buttonExternalState == HIGH) {
        //ledState = !ledState;
        //lcd.print("X");
        buttonExternalPress = HIGH;
      }
    }
  }
  
  // set the LED:
  //digitalWrite(backlightPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonEnterState = readingEnter;
  lastButtonUpState = readingUp;
  lastButtonDownState = readingDown;
  lastButtonExternalState = readingExternal;
}

