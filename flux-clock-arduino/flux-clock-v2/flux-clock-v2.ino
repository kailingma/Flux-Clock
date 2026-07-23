// FLUX CLOCK Version (2)
// 
// This version will integrate the RTC module and use NTP through the WiFi module of the
// Arudino R4 WiFi. Some of the new code is from the RTC_NTPSync Arduino example.
//
// V1 Notes:
// This will be the first functional version of the FLUX CLOCK. Aka alarms
// Buttons have been switched to digital, and a different screen
// has been wired to replace the broken one on the shield.
//
// I will also wire a buzzer for this version. (pin 13)

// TODO
// Implement NTP functionality
// Implement AM/PM
// See if its possible to make text bigger and stretch accross multiple squares

#include <LiquidCrystal.h>
#include <BigCrystal.h>
#include "RTC.h"
#include <NTPClient.h>

// Some code for wifi from the example
#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif

#include <WiFiUdp.h>
#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);

// Current time variables for readability
unsigned int hours = 0;
unsigned int minutes = 0;
unsigned int seconds = 0;

// Time offset from UTC
auto timeZoneOffsetHours = 0;

// Alarm setting
int alarmHour = 12; // set to 12 hours as to not awaken anyone ;)
int alarmMinute = 0;
int alarmSnooze = 0; // How long to snooze (in increments of 9)
unsigned long alarmSnoozeStartTime = 0; // Millis value of when the snooze happened
bool alarmON = false;
bool alarmRinging = false;
bool alarmKeyWasPressed = false;
int lastTriggeredHour = -1;
int lastTriggeredMinute = -1;

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

// Create a BigCrystal instance wrapping the lcd for large text display
BigCrystal bigCrystal(&lcd);

// Some big font stuff I dont understand to fix a known bug
void loadBigChars() {
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t customChar[8];
    for (uint8_t j = 0; j < 8; j++) {
      customChar[j] = pgm_read_byte(BF_fontShapes + (i * 8) + j);
    }
    bigCrystal.createChar(i, customChar);
  }
}

// A modified version of the printBig function with no spacing
uint8_t printBigTight(char *str, uint8_t col, uint8_t row) {
  uint8_t totalWidth = 0;
  for (char *c = str; *c != '\0'; c++) {
    uint8_t w = bigCrystal.widthBig(*c);
    if (w == 0) continue;           // unsupported character, skip
    bigCrystal.writeBig(*c, col + totalWidth, row);
    totalWidth += (w - 1);          // raw width, no spacer
  }
  return totalWidth;
}

// Update the time on the screen
void updateTime() {
  // Define a variable to store the time
  RTCTime currentTime;
  // Get current time from RTC
  RTC.getTime(currentTime);
  // Set time variables
  hours = currentTime.getHour();
  minutes = currentTime.getMinutes();
  seconds = currentTime.getSeconds();
  bool pm = false;
  // Format AM/PM
  if (hour > 12)
  {
    unsigned int hour12 = hour - 12;
    pm = true;
  }
  else {
    hour12 = hour;
  }
  
  // Prepare to print the time to the screen
  char timeFormatted[8];  // "HH : MM" + null terminator
  sprintf(timeFormatted, "%02u : %02u", hour12, minute);  // zero-pad each field to 2 digits

  // Print big clock font
  printBigTight(timeFormatted, 0, 0);
  // Last character (15/15, first line)
  lcd.setCursor(15, 1);
  if (pm) {
    lcd.print("P");
  }
  else
  {
    lcd.print("A");
  }
  // Print alarm status
  lcd.setCursor(15, 0);
  if (alarmON)
  {
    lcd.print("#");
  }
}

void checkAlarm() {
  // If the time matches, it hasn't been snoozed, and the alarm is on
  if (alarmON && (alarmHour == hours) && (alarmMinute == minutes) && (alarmSnooze == 0) 
  && !(lastTriggeredHour == (int)hours && lastTriggeredMinute == (int)minutes))
  {
    // Ring alarm
    alarmRinging = true;
    lastTriggeredHour = (int)hours;
    lastTriggeredMinute = (int)minutes;
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
  if (!(lastTriggeredHour == (int)hours && lastTriggeredMinute == (int)minutes))
  {
    lastTriggeredHour = -1;
    lastTriggeredMinute = -1
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
    // Increment the timewhile (true); offset
    setTimeMillis = (setTimeMillis + millisPerMinute) % millisPerDay;
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
    // Decrement the time zone offset
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

void setupTime() {
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  connectToWiFi;
}

void syncTime() {
  if !(WiFi.status() == WL_CONNECTED)
  {
    connectToWifi();
  }
  RTC.begin();
  // Tell the user we are syncing the clock
  Serial.println("Syncing clock...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SYNCING CLOCK");
  // Start the time client
  timeClient.begin();
  // Update time
  timeClient.update();
  

  // Get the current date and time from an NTP server and convert
  // it to UTC -7 by passing the time zone offset in hours.
  // You may change the time zone offset to your local one.
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime); 
  
  Serial.println("The RTC was just set to: " + String(currentTime));
  // Print success
  lcd.setCursor(0, 1);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectToWiFi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    lcd.setCursor(0,1);
    lcd.clear
    lcd.print("WIFI FAILED!");
    delay(2000);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started Serial.");  // Debugging

  // Set the pinModes
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ALARM_TOGGLE, INPUT_PULLUP);
  pinMode(ALARM_PIN, INPUT_PULLUP);
  pinMode(MINUTE_PIN, INPUT_PULLUP);
  pinMode(HOUR_PIN, INPUT_PULLUP);

  // Begin the standard LiquidCrystal library
  lcd.begin(16, 2);
  lcd.clear();
  // Begin bigCrystal (for big characters!)
  bigCrystal.begin(16, 2);
  // Load the big characters into PSRAM
  loadBigChars();
  // NTP Time sync
  syncTime();
  // Prepare to start clocking
  lcd.clear();
  delay(200);
}

void loop() {
  //bigCrystal.clear();
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
