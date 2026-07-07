# Notes
## V0.0
I'm starting this project with a very simple implementation of the clock:
- The time displayed in 24hrs
- Using the up down left and right buttons to set the time
- the clock should tick upwards

I'll use the LiquidCrystal library:
https://docs.arduino.cc/learn/electronics/lcd-displays/

I have this display:
https://arduinogetstarted.com/tutorials/arduino-lcd-keypad-shield

Based on the pin mapping i think it is running in 4-bit mode.

I used the second [example code](https://arduinogetstarted.com/tutorials/arduino-lcd-keypad-shield) as a starting point for my code. (it is public domain)

## V0.1
*The first non-example-code version*

I've decided to use millis() to track time until I can get my hands on an RTC module, the `unsigned long` I am using will last ~49 days but its pretty inaccurate and will probably start slipping.

I've decided to do show time with a function to convert the millis value to hours, minutes and seconds, with it automatically eliminating days using a % (mod)

A millisecond = 1/1000 of a second, 1/60,000 of a minute, 1/3,600,000 of an hour, 1/86,400,000 of a day

## V0.2
I have succeeded in adding time setting using the difference in milliseconds.

Currently have a bug where the screen fills with gibberish and have no idea why right now

I also added the function to reset the seconds count which probably isn't exactly exact (<1000ms drift)