// FLUX CLOCK Version (1)
// This will be the first functional version of the FLUX CLOCK. Aka alarms
// Buttons have been switched to digital, and a different screen
// has been wired to replace the broken one on the shield.
//
// I will also wire a buzzer for this version. (pin 13)

// TODO
// Right now the setTimeMillis offset is going into the next day if you want to reset it
// and there is no way to go down. In the future, this needs to wrap when the number
// becomes a factor of 24? after being divided by millisPerDay
// For the actual clock, we need to wrap, preserving the remaining milliseconds somehow, but
// I can't touch the millis() value directly.. so.. ?


#include <LiquidCrystal.h>

// Time tracking using millis()
unsigned long timeMillis = millis();
unsigned long setTimeMillis = 0;
unsigned int hours = 0;
unsigned int minutes = 0;
unsigned int seconds = 0;

// Alarm setting
int alarmHour = 12; // set to 12 hours as to not awaken anyone ;)
int alarmMinute = 0;
int alarmSnooze = 0; // How long to snooze (in increments of 9)
unsigned long alarmSnoozeStartTime = 0; // Millis value of when the snooze happened
bool alarmON = false;
bool alarmRinging = false;
bool alarmKeyWasPressed = false;

// Define constants for key representations
const int KEY_ALARM_HOUR = 0;
const int KEY_ALARM_MINUTE = 1;
const int KEY_ALARM_SNOOZE = 2;
const int KEY_HOUR = 3;
const int KEY_MINUTE = 4;
const int KEY_SELECT = 5;
const int KEY_NONE = 6;

// Define some of the other pins
const int BUZZER_PIN = 13;
const int ALARM_PIN = 10;
const int MINUTE_PIN = 11;
const int HOUR_PIN = 12;
const int ALARM_TOGGLE = 3;

// Start the LiquidCrystal library on the pins used by the shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

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
  
  // Prepare to print the time to the screen
  char timeFormatted[9];  // "HH:MM:SS" + null terminator
  sprintf(timeFormatted, "%02u:%02u:%02u", hours, minutes, seconds);  // zero-pad each field to 2 digits

  lcd.setCursor(0, 0);
  lcd.print(timeFormatted);  // row 1: clock
}

// Zero out the seconds portion of the current time, e.g. after the user sets the minute,
// so the clock doesn't keep whatever seconds value it happened to be at.
void resetSeconds() {
  // Recompute the current time-of-day in ms, same formula as updateTime()
  timeMillis = (millis() + setTimeMillis) % millisPerDay;
  // How many seconds we are into the current minute (0-59) - this is the part we want to subtract
  int temp_seconds = (timeMillis / millisPerSecond) % 60;
  // How many milliseconds we need to subtract off setTimeMillis to zero those seconds out
  unsigned long delta = (unsigned long)temp_seconds * millisPerSecond;

  // setTimeMillis is unsigned, so subtracting more than it currently holds would underflow
  // and wrap around to a huge number instead of going negative. If that would happen here,
  // "borrow" a full day's worth of ms first - subtracting delta afterward is then always
  // safe, and the final % below brings it back into the normal [0, millisPerDay) range.
  if (delta > setTimeMillis) {
    setTimeMillis += millisPerDay;
  }
  setTimeMillis -= delta;
  setTimeMillis %= millisPerDay;  // keep the offset bounded to a single day
}

void checkAlarm() {
  // If the time matches, it hasn't been snoozed, and the alarm is on
  if (alarmON && (alarmHour == hours) && (alarmMinute == minutes) && (alarmSnooze == 0))
  {
    // Ring alarm
    alarmRinging = true;
  }
  // Check the time since snooze
  unsigned long timeSinceSnooze = millis() - alarmSnoozeStartTime;
  // If the alarm has been snoozed
  if (alarmON && (timeSinceSnooze >= (alarmSnooze * millisPerMinute)) && (alarmSnooze > 0)) {
    // Ring alarm
    alarmRinging = true;
  }
  // If the alarm is off
  if (!alarmON)
  {
    // Stop the alarm
    alarmRinging = false;
    // Stop the snooze
    alarmSnooze = 0;
  }
}
void setAlarmText() {
  char alarmTimeFormatted[17];  // 16-segment wide display + null terminator
  sprintf(alarmTimeFormatted, "ALARM SET: %02u:%02u", alarmHour, alarmMinute);  // zero-pad each field to 2 digits

  lcd.setCursor(0, 1);
  lcd.print(alarmTimeFormatted);  // row 1: clock
}

void snoozeAlarm() {
  alarmRinging = false;
  if (alarmSnooze == 0)
  {
    // Set the time (in milliseconds) when the snooze was hit
    alarmSnoozeStartTime = millis();
  }
  // Snooze for 9 more minutes (standard time for snooze apparently)
  alarmSnooze += 9;
}

void ringAlarm(bool state) {
  digitalWrite(BUZZER_PIN, state);
}
// Handle key functions
void doKeyActions() {
  // Directly set the boolean value as true or false, inverse because pulldown
  bool alarm_key = !digitalRead(ALARM_PIN);

  // Make sure that the snooze key isn't over-hit
  bool alarmKeyJustPressed = alarm_key && !alarmKeyWasPressed;
  alarmKeyJustPressed = alarm_key;

  // Snooze an alarm?
  if (alarmKeyJustPressed && alarmRinging)
  {
    snoozeAlarm();
    Serial.println("Snoozed.");
  }

  // Check if the minute key is pressed
  if (!digitalRead(MINUTE_PIN) && !alarm_key)
  {
    // Increment the time offset
    setTimeMillis = (setTimeMillis + millisPerMinute) % millisPerDay;
    // Only reset seconds on minute setting, avoid resetting for hours (ie. time changes)
    resetSeconds();
  } 
  else if (!digitalRead(MINUTE_PIN) && alarm_key) 
  {
    // Set the alarm instead of time
    alarmMinute = (alarmMinute + 1) % 60;
    setAlarmText();
  }

  // Check if the hour key is pressed
  if (!digitalRead(HOUR_PIN) && !alarm_key)
  {
    // Increment the time offset
    setTimeMillis = (setTimeMillis + millisPerHour) % millisPerDay;
  } 
  else if (!digitalRead(HOUR_PIN) && alarm_key)
  {
    // Set the alarm instead of time
    alarmHour = (alarmHour + 1) % 24;
    setAlarmText();
  }

  if (!digitalRead(ALARM_TOGGLE))
  {
    alarmON = true;
  }
  else
  {
    // Turn off the alarm
    alarmON = false;
    // Stop snoozing
    alarmSnooze = 0;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started");  // Debugging
  // Set the pinModes
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ALARM_TOGGLE, INPUT_PULLUP);
  pinMode(ALARM_PIN, INPUT_PULLUP);
  pinMode(MINUTE_PIN, INPUT_PULLUP);
  pinMode(HOUR_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("Hello!");
  delay(3000);
}

void loop() {
  // Set the time and update the screen
  updateTime();
  // Check if it is alarm time
  checkAlarm();
  // Handle keys
  doKeyActions();
  // Wait until next loop
  delay(200);
  if (alarmRinging) {ringAlarm(true);}
  delay(200);
  if (alarmRinging) {ringAlarm(false);}
}
