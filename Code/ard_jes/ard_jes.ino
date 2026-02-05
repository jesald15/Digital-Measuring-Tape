#include <LiquidCrystal.h>
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);
const int clkPin = 2;
const int dtPin = 3;
const int swPin = 4;
volatile int position = 0;
float distance = 0;
const float pi = 3.14;
const float diameter = 3.4;
const int pulsesPerRev = 40;
unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 2;
void setup()
{
  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT_PULLUP);
  pinMode(swPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(clkPin), readEncoder, CHANGE);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("MEASURING WHEEL");
  delay(2000);
  lcd.clear();
}
void loop()
{
if (digitalRead(swPin) == LOW)
{
position = 0;
distance = 0;
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("RESETTING...");
delay(500);
lcd.clear();
}
distance = ((pi * diameter) / pulsesPerRev) * position;
if (distance < 0) distance = 0;
lcd.setCursor(0, 0);
lcd.print("Distance:");
lcd.setCursor(0, 1);
lcd.print(distance, 2);
lcd.print(" cm ");
delay(100);
}
void readEncoder()
{
unsigned long currentTime = millis();
if (currentTime - lastInterruptTime > debounceDelay)
{
if (digitalRead(dtPin) != digitalRead(clkPin))
{
position++;
}
else
{
position--;
}
lastInterruptTime = currentTime;
}
}