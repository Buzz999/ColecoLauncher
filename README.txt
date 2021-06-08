==============
ColecoLauncher
==============
This small module is used to easily launch Coleco roms on a RC2014 system.
It is based on a arduino mini associated to a OLED display and a few buttons.
The goal is to communicate with the RC2014 to get the list of files stored on the SDCard
and send the right commands to the target to perform loading.
The only necessary actions from the user point of view is to power on the system, select the file with simple 
up & down buttons, then push the load buttons to perform loading.
That's it.

The simple module cannot work alone. A fully configured RC2014 Colecovision system is required.

SN76489 Sound Card for RC2014: https://github.com/jblang/sn76489
TMS9918A Video Card for RC2014: https://github.com/jblang/TMS9918A
Game Controller Board for RC2014: https://github.com/jblang/GameController
z80ctrl for RC2014: https://github.com/jblang/z80ctrl
z80ctrl CPU/RAM/CTC) for RC2014: https://github.com/jblang/z80ctrl (z80ram directory)
SC112 V1.0 Modular backplane: https://smallcomputercentral.wordpress.com/sc112-modular-backplane-rc2014/

All the needed information to build and configure your own Colecovision system is described here:
https://github.com/jblang (z80ctrl & TMS9918A sections).

All RC2014 extra boards are documented here:
https://smallcomputercentral.wordpress.com (Projects & Documentation sections).

-----------
Description
-----------

Hardware
--------
* Fully assembled RC2014 Colecovision system (see above)
* Arduino mini Pro 16MHZ/5V
* 128x64 OLED I2C display
* 3x push buttons
* Cable with 6 pin headers.

Connections (described in Control.cpp file) are:
* Buttons
 Arduino A0  -> Select button -> GND
 Arduino A1  -> DOWN button -> GND
 Arduino A2  -> UP button -> GND

* Display
 Arduino A4  ->   OLED SDA
 Arduino A5  ->   OLED SCL
                 OLED GND
 Arduino VCC -> OLED VCC (5v)
 Arduino GND -> OLED GND

* Z80CTRL (JP5)
 1: GND      - Arduino GND
 3: NC
 5: VCC      - Arduino VCC
 7: RX       - Arduino TX
 9: TX       - Arduino RX
 11: AVRRSTn - Arduino DTR

Firmware
--------
The source code (27/03/20210) is included into this package (src and include directories).
It as been compiled with Visual Studio Code for the Atmega328 16MHz 5V target.

The following libraies are necessary:
Adafruit BusIO
Adafruit GFX Library
Adafruit SS1306

Have fun !
Buzz






