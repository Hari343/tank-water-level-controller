
#include <Wire.h>

#include <LiquidCrystal_I2C.h>

// C++ code
//



const int delayTime = 1000; // in ms

// vars for level controller
const float speedOfSound = 0.034; // in centimeters per microsecond
const float tankHeight = 250; // all heights and levels are in cm
const float maxLevel = 200; 
const float minLevel = 50;
bool levelControlActive = true; // for programmatically turning level control ON and OFF


// vars for tilt controller
bool isTilted = false;

// vars for lcd
String previousUpdate = "IoT Group 6";

// pin numbers
const byte trigPin = 2;
const byte echoPin = 4; 
const byte motorPin = 7;
const byte tiltSensorPin = 8;
const byte buzzerPin = 6;

LiquidCrystal_I2C lcd(0x20,  16, 2);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Set baud rate for the COM port
  Serial.begin(9600);
  
  // Set HC-SR04 pinout
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);
  
  // Set buzzer pinout
  pinMode(buzzerPin, OUTPUT);
  
  digitalWrite(trigPin, LOW); // just in case
  digitalWrite(buzzerPin, LOW);
  digitalWrite(tiltSensorPin, HIGH);
  
  // Set motor pinout
  pinMode(motorPin, OUTPUT);
  
  digitalWrite(motorPin, LOW);
  
  // initialize lcd
  lcd.init();
  lcd.backlight();

}

void refreshScreen()
{
  
  if(!isTilted)
  {
    float level = tankHeight - measureDistance();
    String message = "level (cm):";
    String update = message + level;
    if(update != previousUpdate)
    {
      lcd.clear();
    }
    lcd.setCursor(0,0);
    lcd.print(message); 
    lcd.setCursor(0,1);
    lcd.print(level);
    previousUpdate = update;
  }
  else
  {
    String row1 = "Tank is tilted";
    String row2 = "Reset Tank";
    String update = row1 + row2;
    if (update != previousUpdate)
    {
      lcd.clear();
    }
    lcd.setCursor(0,0);
    lcd.print(row1);
    lcd.setCursor(0, 1);
    lcd.print(row2);
    previousUpdate = update;
    
  }
}


// This will measure the HC-SR04 reading in cm
float measureDistance()
{
    
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW); 
  
  float duration = pulseIn(echoPin, HIGH);
  return (duration * speedOfSound) / 2;
}

// main level controller logic
void controlLevel()
{
  if (levelControlActive)
  {
    float level = tankHeight - measureDistance();
    if (level < minLevel)
    {
      // start motor
      digitalWrite(motorPin, HIGH);
    }
    if (level >= maxLevel)
    {
      // stop motor
      digitalWrite(motorPin, LOW);
    }
  }
  else 
  {
    // if level controller is disabled due to tank tilt, make sure motor is not running
    digitalWrite(motorPin, LOW);
  }
}

// Check for tilt. If tilted stop lever controller
void checkTilt() 
{
  if(digitalRead(tiltSensorPin) == LOW)
  {
    isTilted = true;
    levelControlActive = false;
    // start buzzer
    tone(buzzerPin, 500);
  }
  else
  {
    isTilted = false;
    levelControlActive = true;
    // stop buzzer
    noTone(buzzerPin);
  }
}

// this will signal that the program is running
void blink() 
{
  if (digitalRead(LED_BUILTIN) == LOW)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
  

void loop()
{
  blink();
  controlLevel();
  checkTilt();
  refreshScreen();
  delay(delayTime);
}