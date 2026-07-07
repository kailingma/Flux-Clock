#include <LiquidCrystal.h>

// Time tracking using millis()
unsigned long timeMillis = millis();
unsigned long setTimeMillis = 0;
unsigned int hours = 0;
unsigned int minutes = 0;
unsigned int seconds = 0;

// Define constants for key representations
const int KEY_RIGHT = 0;
const int KEY_UP = 1;
const int KEY_DOWN = 2;
const int KEY_LEFT = 3;
const int KEY_SELECT = 4;
const int KEY_NONE = 5;

// Start the LiquidCrystal library on the pins used by the shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Handle the keys on the shield
int getKey() {
  int analogValue = analogRead(A0);
  Serial.println(analogValue);  // Debugging

  // The shield runs on analog values for the different buttons
  if (analogValue < 50) return KEY_RIGHT;
  else if (analogValue < 200) return KEY_UP;
  else if (analogValue < 400) return KEY_DOWN;
  else if (analogValue < 600) return KEY_LEFT;
  else if (analogValue < 800) return KEY_SELECT;
  else return KEY_NONE;
}

const unsigned long millisPerSecond = 1000;
const unsigned long millisPerMinute = 60000;
const unsigned long millisPerHour = 3600000;
const unsigned long millisPerDay = 86400000;

// Recompute hours/minutes/seconds from elapsed millis since setTimeMillis
void updateTime() {
  // Using % (mod) drops in chunks of 60 (minutes or hours) to only count remaining bits
  // mod by a day first so the clock rolls over at midnight instead of running past 24h
  timeMillis = (millis() - setTimeMillis) % millisPerDay;
  hours = timeMillis / millisPerHour;             // whole hours elapsed today
  minutes = (timeMillis / millisPerMinute) % 60;  // drop already-counted hours, keep 0-59
  seconds = (timeMillis / millisPerSecond) % 60;  // drop already-counted minutes, keep 0-59
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started");  // Debugging

  lcd.begin(16, 2);
  lcd.print("Hello!");
  delay(3000);
}

void loop() {
  updateTime();

  char timeFormatted[9];  // "HH:MM:SS" + null terminator
  sprintf(timeFormatted, "%02u:%02u:%02u", hours, minutes, seconds);  // zero-pad each field to 2 digits

  lcd.clear();
  lcd.print(timeFormatted);  // row 1: clock

  int key = getKey();

  lcd.setCursor(0, 1);  // row 2: which button (if any) is pressed
  switch (key) {
    case KEY_RIGHT:
      lcd.print("RIGHT");
      break;
    case KEY_UP:
      lcd.print("UP");
      break;
    case KEY_DOWN:
      lcd.print("DOWN");
      break;
    case KEY_LEFT:
      lcd.print("LEFT");
      break;
    case KEY_SELECT:
      lcd.print("SELECT");
      break;
    default:
      lcd.print("Press key!");
      break;
  }

  delay(200);
}
