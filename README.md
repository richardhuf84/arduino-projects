# 2d platform game on Arduino UNO

This is the repository for a small project to make a 2d platformer and run it on an Arduino Uno microcontroller.

## Overview

### Hardware bill of materials

- Arduino Uno R4 Minima
- Duinotech 1.5" OLED display, 128px x 128px
- A Super Nintendo Classic mini controller, PAL, Model CLV-202
- A breadboard
- Some red LEDs 
- a 180 ohm resistor
- a simple push button

### Software

- [Arduino IDE](https://docs.arduino.cc/software/ide/)
- Language: C++ (used on Arduino microcontrollers) 
  - [Arduino programming docs](https://docs.arduino.cc/programming/)
  - [Microsoft docs](https://learn.microsoft.com/en-us/cpp/?view=msvc-170)
- Git for version control
- OLED Display Driver: [Adafruit_SSD1351](https://github.com/adafruit/Adafruit-SSD1351-library)
- `Adafruit_GFX` Graphics library:
  -  [Repo](https://github.com/adafruit/adafruit-gfx-library)
  -  [Docs](https://learn.adafruit.com/adafruit-gfx-graphics-library)
- [NintendoExtensionCtrl](https://github.com/dmadison/NintendoExtensionCtrl) library for the controller

## Project outline

This is really just a fun little excercise, to learn more about building circuits and programming with Arduino. Thinking about the platform elements, I'm drawing some inspiration from Super Mario Bros. 2 on NES.

## Roadmap

- [ ] Add collectibles (raspberries). Collecting 3 raspberries turns on the red LED. Something cool happens. Maybe super speed or a brief time of being invincible (like a star in Mario).
- [ ] There's a bug where it draws gradient banding on the screen. Look into https://github.com/olikraus/u8g2 for better memory management.
- [ ] Look at setting up a camera / viewport, to allow for movement a virtual 'camera' across level assets, and tracking the player. This [Lazy Foo' Productions scrolling tutorial](https://lazyfoo.net/SDL_tutorials/lesson21/index.php) could be useful.
- [x] Bug 🐞 I've mapped both `a` and `b` to jump. Feels intuitive for a platformer. However I need to refactor, as it registers pressing __either__ as jumping. Introduced a bug where you can jump forever.
- [x] Setup basic player. Includes movement and jump support, as well as an 'action' button which turns on an LED for now.
- [x] Setup basic enemy. Moves, and kills the player on contact.
- [x] Setup game over and retry screen.