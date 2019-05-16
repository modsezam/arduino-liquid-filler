#include <Bounce2.h>
#include <UC1701.h>

static UC1701 lcd;

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
const int D7 = 7;
const int D8 = 8; // przycisk

int backlightIntensity = 0;        // LCD backlight intesity

int time0 = 0; //zwłoka właczenia pomp
int time1 = 20; // włączenie zaworu 1
int time2 = 0; // włączenie zaworu 2
int time3 = 2000; //wyłaczenie zaworu 1
int time4 = 0; //wyłacznie zaworu 2

long OnTime1 = 0;
long OffTime1 = 200;
long OnTime2 = 20;
long OffTime2 = 100;
long OnTime3 = 20;
long OffTime3 = 100;
int State1 = LOW;
int State2 = LOW;
int State3 = LOW;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;

int flagStart = 0;

// Instantiate a Bounce object
Bounce debouncer1 = Bounce(); 
// Instantiate another Bounce object
Bounce debouncer2 = Bounce(); 

void setup() {

  // Setup the first button with an internal pull-up :
  //pinMode(buttonEnterPin,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer1.attach(buttonEnterPin);
  debouncer1.interval(5); // interval in ms
  
   // Setup the second button with an internal pull-up :
  //pinMode(buttonUpPin,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer2.attach(buttonUpPin);
  debouncer2.interval(5); // interval in ms

  
  pinMode(backlightPin, OUTPUT); //set backlight pin to output 
  digitalWrite (backlightPin, backlightIntensity); //convert backlight intesity from a value of 0-5 to a value of 0-255 for PWM.
  pinMode(buttonEnterPin, INPUT);
  pinMode(buttonUpPin, INPUT);
  pinMode(buttonDownPin, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  
  lcd.begin();  //sets the resolution of the LCD screen
  
  lcd.setCursor(0, 0); //set the cursor to the fifth pixel from the left edge, third row.
  lcd.print(">");
  
  lcd.setCursor(7, 0); //set the cursor to the fifth pixel from the left edge, third row.
  lcd.print(time0); //print text on screen
  lcd.print("ms zwloka wlacz.");
  lcd.setCursor(7, 1);
  lcd.print(time1);
  lcd.print("ms wlacz zaw1");
  lcd.setCursor(7, 2);
  lcd.print(time2);
  lcd.print("ms wlacz zaw2");
  lcd.setCursor(7, 3);
  lcd.print(time3);
  lcd.print("ms wylacz zaw1");
  lcd.setCursor(7, 4);
  lcd.print(time4);
  lcd.print("ms wylacz zaw2");


}

void loop() { 

  // Update the Bounce instances :
  debouncer1.update();
  debouncer2.update();

  // Get the updated value :
  int value1 = debouncer1.read();
  int value2 = debouncer2.read();


if ( value1 == LOW) {

  flagStart = 1;
  
}



if ( flagStart == 1 ) {
  

    unsigned long currentMillis = millis();
    if((D1 == HIGH) && (currentMillis - previousMillis1 >= OnTime1))
      {
        State1 = LOW;  // Turn it off
        previousMillis1 = currentMillis;  // Remember the time
        digitalWrite(D1, State1);  // Update the actual LED
        lcd.setCursor(7, 6);
        lcd.print("start ...  ");
      }
      else if ((D1 == LOW) && (currentMillis - previousMillis1 >= OffTime1))
      {
        
        State1 = HIGH;  // turn it on
        previousMillis1 = currentMillis;   // Remember the time
        digitalWrite(D1, State1);    // Update the actual LED
        lcd.setCursor(7, 6);
        lcd.print("end ...  ");
        //flagStart = 0;
      }
        
  } 

/*
if (flagStart == 1){
  flagStart = 0;
  lcd.setCursor(7, 6);
  lcd.print("start ...  ");
  
  delay(time0);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);
  delay(time1);
  digitalWrite(D2, HIGH);
  delay(time2);
  digitalWrite(D3, HIGH);
  delay(time3);
  digitalWrite(D2, LOW);
  digitalWrite(D0, LOW);
  delay(time4);
  digitalWrite(D3, LOW);
  digitalWrite(D0, LOW);

  lcd.setCursor(7, 6);
  lcd.print("koniec ...  ");
  delay(300);
  lcd.setCursor(7, 6);
  lcd.print("            ");
}
*/





  
}
