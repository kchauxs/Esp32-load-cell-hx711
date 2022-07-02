#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <preferences.h>
#include <Pushbutton.h>

const byte BUTTON_MODE = 12;
const byte BUTTON_TARE = 14;
const byte LOADCELL_DOUT_PIN = 16;
const byte LOADCELL_SCK_PIN = 4;

int weightReading = 0;
int lastWeightReading = 0;
int knownWeight[5] = {180, 250, 500, 750, 1000};

HX711 balance;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences preferences;

Pushbutton tareButton(BUTTON_TARE);

long getScale()
{
  long scale = preferences.getLong("scale", 0);
  return scale;
}

void setScale(long scale)
{
  preferences.putLong("scale", scale);
}

void calibration()
{
  int weightIndex = 0;
  bool isScaled = false;
  long adcLecture = 0;

  lcd.clear();
  digitalWrite(BUILTIN_LED, HIGH);

  lcd.setCursor(4, 0);
  lcd.print("Balance");
  lcd.setCursor(2, 1);
  lcd.print("Calibration");
  delay(1500);

  balance.read();
  balance.set_scale();
  balance.tare(20);

  lcd.clear();

  while (!isScaled)
  {

    lcd.setCursor(0, 0);
    lcd.print("Known Weight:");
    lcd.setCursor(0, 1);
    lcd.print(knownWeight[weightIndex]);
    lcd.print(" g        ");

    if (tareButton.getSingleDebouncedPress())
      weightIndex = (weightIndex > 2) ? 0 : weightIndex + 1;

    if (!digitalRead(BUTTON_MODE))
      continue;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Put the weight");
    lcd.setCursor(0, 1);
    lcd.print("and wait ...");
    delay(2000);

    adcLecture = balance.get_value(100);
    long scale = adcLecture / knownWeight[weightIndex];

    setScale(scale);
    Serial.println("Info: New scale: " + (String)scale);
    delay(100);

    isScaled = true;
    lcd.clear();
    digitalWrite(BUILTIN_LED, LOW);
  }
}

void readWeight()
{
  if (!balance.wait_ready_timeout(200))
    return;

  weightReading = round(balance.get_units());
}

void displayWeightOnLcd()
{
  if (weightReading == lastWeightReading)
    return;

  lcd.setCursor(0, 0);
  lcd.print("Weight: ");
  lcd.print(weightReading);
  lcd.println(" g        ");

  lastWeightReading = weightReading;
}

void checkTareButtonIsPressed()
{
  if (tareButton.getSingleDebouncedPress())
    balance.tare();
}

void setup()
{
  Serial.begin(115200);

  preferences.begin("load-cell", false);
  balance.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  pinMode(BUTTON_MODE, INPUT);
  pinMode(BUTTON_TARE, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(3, 0);
  lcd.print("Starting");
  delay(2000);

  if (digitalRead(BUTTON_MODE) && digitalRead(BUTTON_TARE))
    calibration();

  lcd.setCursor(1, 0);
  lcd.print("Remove the weight");
  lcd.setCursor(1, 1);
  lcd.print("and wait...");
  delay(2000);

  long scale = getScale();
  Serial.println("Info: Scale: " + (String)scale);

  balance.set_scale(scale);
  balance.tare(20);
  lcd.clear();

  lcd.setCursor(1, 0);
  lcd.print("Ready....");
  preferences.end();
  delay(1000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Weight: ");
  lcd.print(weightReading);
  lcd.println(" g        ");
}

void loop()
{
  yield();
  readWeight();
  displayWeightOnLcd();
  checkTareButtonIsPressed();
}