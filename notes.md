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

## V1
Okay, so I determined the strange issue with inconsistent screen updating is most likely caused by my faulty screen on the shield. I spent some time wiring up 3 new buttons today, to digital pins 10, 11, and 12

10 will serve as the "alarm-set" button. Holding it while pressing the other keys will increment the alarm time, and not the actual clock time.

11 will serve as minutes, pressing it repeatedly will increment the minutes by one, holding it will rapidly increase it just like before.

12 will serve as hours, behaving the same as the minute button except that it is hours instead.

13 will be buzzer, its an active one so now PWM, yay!

I wrote some code to accommodate this, so now it is in flux-clock-digital.ino

### Alarm implementation
So I'm implementing the alarm functionality now, and it has occured to me I have no alarm on/off switch. Sigh time to wire one up.

Update: I cannot seem to find a suitable switch in my vicinity so for now it is just going to be a wire :heavysob:

Pin 3 is now the alarm toggle!

---

I wired up the buttons and buzzer today, implemented alarm clock logic (it works!) and stopped using `lcd.clear()` every loop. It's coming along great! Tomorrow I will hopefully replace the `delay(200)`'s in my code with exact timings and maybe start looking out for an RTC module for better stuff than `millis()` over and over.

*I spent about 4 hours coding today, according to Hackatime. Putting 45 minutes for all the wiring and testing of the hardware*

Here's some pics of the hardware:
![image](https://cdn.hackclub.com/019f4a62-fb9d-7220-b10d-0cedd12c1de4/IMG_8362.jpg)
![image](https://cdn.hackclub.com/019f4a63-1559-7347-b67c-26074a8ffa38/IMG_8361.jpg)
![image](https://cdn.hackclub.com/019f4a63-3c48-7c42-93ad-751d3ae2c2af/IMG_8360.jpg)


## V2
So after a bit of time, I've decided to switch to an Arduino UNO R4 WiFi. This will give me access to wifi, RTC, and more flexibility overall.

I also found the [BigCrystal](https://github.com/gregington/BigCrysta) library, to allow me to print big on the LCD.

I also plan to implement screen dimming, since I have an RGB LCD screen and it supports dimming.

Importantly though, I am running out of pins, with only 3 remaining at the moment. I'll have to figure out what to do about that.