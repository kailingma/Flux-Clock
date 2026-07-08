#include <LiquidCrystal.h>

// Time tracking using millis()
unsigned long timeMillis = millis();
long setTimeMillis = 0;
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

// Select button hold time
unsigned long holdTime = 0;

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

// Recompute hours/minutes/seconds from millis() plus the user-set offset
void updateTime() {
  // Using % (mod) drops in chunks of 60 (minutes or hours) to only count remaining bits
  // mod by a day first so the clock rolls over at midnight instead of running past 24h
  timeMillis = (millis() + setTimeMillis) % millisPerDay;
  hours = timeMillis / millisPerHour;             // whole hours elapsed today
  minutes = (timeMillis / millisPerMinute) % 60;  // drop already-counted hours, keep 0-59
  seconds = (timeMillis / millisPerSecond) % 60;  // drop already-counted minutes, keep 0-59
}

// Recompute hours/minutes/seconds from millis() plus the user-set offset
void resetSeconds() {
  // Using % (mod) drops in chunks of 60 (minutes or hours) to only count remaining bits
  // mod by a day first so the clock rolls over at midnight instead of running past 24h
  timeMillis = (millis() + setTimeMillis) % millisPerDay;
  int temp_seconds = (timeMillis / millisPerSecond) % 60;  // drop already-counted minutes, keep 0-59
  // Reset the seconds count
  setTimeMillis -= temp_seconds * millisPerSecond;
}

void resetHoldTime() {
  holdTime = 0;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started");  // Debugging

  lcd.begin(16, 2);
  lcd.print("Hello!");
  delay(3000);
}

bool changingAlarm = false;

void loop() {
  updateTime();

  char timeFormatted[9];  // "HH:MM:SS" + null terminator
  sprintf(timeFormatted, "%02u:%02u:%02u", hours, minutes, seconds);  // zero-pad each field to 2 digits

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(timeFormatted);  // row 1: clock

  int key = getKey();

  lcd.setCursor(0, 1);  // row 2: which button (if any) is pressed
  switch (key) {
    case KEY_RIGHT:
      resetHoldTime();
      // Tell what we are changing
      lcd.print("RIGHT-ADD MINUTE");
      // Adjust the offset by one minute
      setTimeMillis += millisPerMinute;
      // Reset the seconds counter
      resetSeconds();
      // Loop again
      break;
    case KEY_UP:
      resetHoldTime();
      // Tell what we are changing
      lcd.print("UP-ADD HOUR");
      // Adjust the offset by the time in an hour
      setTimeMillis += millisPerHour;
      break;
    case KEY_DOWN:
      resetHoldTime();
      // Print what we are changing
      lcd.print("DOWN-SUB HOUR");
      // Adjust the offset by the time in an hour
      setTimeMillis -= millisPerHour;
      break;
    case KEY_LEFT:
      resetHoldTime();
      // Tell what we are changing
      lcd.print("LEFT-SUB MINUTE");
      // Remove 60,000 ms (one minute)
      setTimeMillis -= millisPerMinute;
      // Reset the seconds counter
      resetSeconds();
      // Loop
      break;
    case KEY_SELECT:
      // Check if the button was held for 2 seconds or more 
      if (millis() - holdTime >= 2000)
      {
        // Tell the person their holding was successful
        lcd.print("ALARM SETTING");
        // Flag to the other functions that the next moves should change the alarm
        changingAlarm = true;
        // Reset the hold time
        holdTime = 0;
        // Loop
        break;
      } else if (holdTime == 0)
      {
        // Prepare the holdTime
        holdTime = millis();
        lcd.print("SET ALARM?");
        break;
      }
      break;
    default:
      // Clear the hold attempt
      
      // Leave the bottom row empty
      break;
  }

  delay(200);
}
