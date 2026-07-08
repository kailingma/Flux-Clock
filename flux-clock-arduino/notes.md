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

## V0.3
Today I'm experimenting with making an alarm function, which is going to be difficult because of the way this shield sends button presses. All the buttons are represented as a single analog value on `A0`.
The priority (in decreasing resistance, high priority to low priority, and high priority will overwrite low):

Right -> UP -> Down -> Left -> Select

So I'm going to make it have a set mode when pressing select for 2 seconds or more.

## Not another version really
*I got tired of fighting with the LCD to work normally so now I'm trying a different setup*

So I effectively redid the hardware side of this at least for the alpha configuration, minus the buttons. I wired up a different screen I have on hand as well as another arduino to see if it has the same issues with clock ticking as my setup currently.

As a result now I know this is the pinout:

`const int rs = 9, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7;`

I'm having a problem where one uptick in seconds will often take many seconds to update and I'm not sure what's causing it. I theorize my screen is defective since the backlight is pretty bad too, so I tried a different LCD I have on hand. See [lcd-test.ino](https://github.com/kailingma/Flux-Clock/blob/main/flux-clock-arduino/flux-clock/flux-clock.ino) for the code I used for this picture.

![image](https://cdn.hackclub.com/019f43d3-d9a8-7ba4-825c-4329714675bf/IMG_8358.jpg)