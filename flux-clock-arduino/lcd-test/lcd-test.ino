#include <LiquidCrystal.h>

// Same wiring as the LCD keypad shield used in flux-clock.ino
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Hello World!");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print("@starthackclub");
  delay(300);
}
