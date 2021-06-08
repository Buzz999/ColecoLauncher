// Coleco Launcher
// (c) Buzz - 27/03/2021
// electronique.fb@free.fr
//
// Arduino Mini Pro 5V
// 128x64 I2C OLED display
// 3 buttons
//
// Connections:

// Buttons
// Arduino A0 -> Select button -> GND
// Arduino A1 -> DOWN button -> GND
// Arduino A2 -> UP button -> GND

// Display
// Arduino A4 ->   OLED SDA
// Arduino A5 ->   OLED SCL
//                 OLED GND
//  Arduino VCC -> OLED VCC (5v)
//  Arduino GND -> OLED GND

// Z80CTRL (JP5)
// 1: GND      - Arduino GND
// 3: NC
// 5: VCC      - Arduino VCC
// 7: RX       - Arduino TX
// 9: TX       - Arduino RX
// 11: AVRRSTn - Arduino DTR

#include <U8x8lib.h>
#include <Wire.h>
#include "Control.h"
#include "Buttons.h"
#include "Menu.h"

// Buttons UP, DOWN and SELECT
#define PIN_UP A2
#define PIN_DOWN A1
#define PIN_SELECT A0

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
Buttons buttons(PIN_SELECT, PIN_UP, PIN_DOWN);
U8X8_SSD1306_128X64_NONAME_HW_I2C oled(/* reset=*/U8X8_PIN_NONE);
Menu menu;

void Control::init()
{
  Serial.begin(115200); // Default baudrate for z80ctrl
  oled.begin();

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.clear();

  oled.setCursor(1, 0);
  oled.inverse();
  oled.print("COLECO LOADER");
  oled.noInverse();
  oled.setCursor(4, 2);
  oled.print("(c) Buzz");
  oled.setCursor(3, 3);
  oled.print("27/03/2021");

  // Waiting 5s before sending commands to Z80CTRL
  oled.setCursor(4, 5);
  oled.print("waiting");
  for (int j = 4; j >= 0; j--)
    for (int i = 9; i >= 0; i--)
    {
      oled.setCursor(6, 6);
      oled.print(j);
      oled.print(".");
      oled.print(i);
      oled.print("s ");
      delay(100);
    }

  oled.clear();

  buttons.init();
  _state = MS_INIT;
}

void Control::update()
{
  buttons.update();

  switch (_state)
  {
  case MS_INIT:
    menu.reset();
    _state = MS_PROMPT;
    break;

  case MS_PROMPT:
    if (acceptPrompt(false))
    {
      changeBaudrate();
      sendDirCommand();
      _state = MS_DIR;
    }
    else
    {
      oled.clear();
      oled.setCursor(5, 0);
      oled.inverse();
      oled.print("TIMEOUT");
      oled.setCursor(0, 3);
      oled.noInverse();
      oled.print("Retrying ...");

      delay(2000);
      _state = MS_INIT;
    }
    break;

  case MS_DIR:
    oled.clear();
    acceptFilenames();
    menu.setNEntries(_nEntries);
    showEntries();
    _state = MS_SELECT;
    break;

  case MS_SELECT:

    if (buttons.upPressed())
    {
      if (menu.prev())
        showEntries();
      buttons.waitUnPressed();
    }
    else if (buttons.downPressed())
    {
      if (menu.next())
        showEntries();
      buttons.waitUnPressed();
    }
    else if (buttons.selectPressed())
    {
      buttons.waitUnPressed();
      _state = MS_RUN;
    }
    break;
  case MS_RUN:
    launch();
    _state = MS_HALT;

    break;
  case MS_HALT:
    while (1)
      ; // Need to reset the board to restart
    break;

  default:
    break;
  }
}

void Control::launch() const
{
  if (_nEntries == 0)
    return; // Guard

  oled.clear();
  oled.setCursor(6, 0);
  oled.inverse();
  oled.print("RUN");
  oled.setCursor(0, 3);
  oled.noInverse();
  oled.println("Launching ...");
  uint8_t idx = menu.getISel();
  oled.println(_filenames[idx]);

  Serial.println("loadbin 0 coleco.rom");
  delay(500);
  Serial.print("loadbin 8000 ");
  Serial.println(_filenames[idx]);
  delay(500);
  Serial.println("clkdiv 5 ");
  delay(500);
  Serial.println("run 0");
  delay(500);
  oled.clear();

  oled.setCursor(5, 0);
  oled.inverse();
  oled.print("RUN");
  oled.setCursor(0, 3);
  oled.noInverse();
  oled.println("Running ...");

  oled.println("Press RESET on");
  oled.println("Z80CTRL board");
  oled.println("to restart");
}

bool Control::acceptPrompt(bool waitPrompt)
{
  if (waitPrompt == false)
    return true; // Always synchronized

  oled.clear();
  oled.setCursor(6, 0);
  oled.inverse();
  oled.print("SYNC");
  oled.setCursor(0, 3);
  oled.noInverse();
  oled.print("Waiting ...");

  uint8_t state = 0;
  uint16_t count = 0;
  while (1)
  {
    if (Serial.available())
    {
      byte ch = Serial.read();
      switch (state)
      {
      case 0:
        if (ch == ':')
        {
          state = 1;
          break;
        }
      case 1:
        if (ch == '/')
          state = 2;
        else
          state = 0;
        break;
      case 2:
        if (ch == '>')
          return true; // Synchronized
        else
          state = 0;
        break;

      default:
        state = 0;
        break;
      }
      count++;
      if (count >= 1000)
        return false; // Timout
    }
  }

  return false;
}

void Control::changeBaudrate() const
{
  Serial.println("baud 0 9600");
  delay(200);
  Serial.end();
  Serial.begin(9600);
}

void Control::sendDirCommand() const
{
  Serial.println("dir");
}

void Control::acceptFilenames()
{
  _nEntries = 0;
  uint8_t cntc = 0;
  MS_DECODE state = IDLE;

  while (1)
  {
    if (Serial.available())
    {
      byte ch = Serial.read();

      switch (state)
      {
      case IDLE:
        if (ch != ' ' && ch != 10 && ch != 13)
        {
          state = ACQUIRE;
          _filenames[_nEntries][cntc] = ch;
          cntc++;
        }
        break;

      case ACQUIRE:
        if (cntc > FILENAME_SIZE)
        {
          state = SKIP;
          cntc = 0;
        }
        else if (ch == ' ' || ch == 10 || ch == 13)
        {
          state = IDLE;
          _filenames[_nEntries][cntc] = '\0'; // Storing the entry
          cntc = 0;

          if (strcmp(_filenames[_nEntries], "item(s)") != 0) // uses this marker to identify the end of the dir command execution
          {
            if (strcmp(_filenames[_nEntries], "dir") != 0)
            { // Skipping dir as it corresponds to the echo to the dir command
              _nEntries++;
              if (_nEntries >= N_FILENAMES) // Ignoring all other entries as there is no more space
              {
                flushInput();
                return;
              }
            }
          }
          else
          {
            if (_nEntries > 1)
            { // Ignoring the item before the end marker as it correesponds to the number of entries returned by the dir command
              _nEntries--;
            }
            flushInput();
            return;
          }
        }
        else
        {
          _filenames[_nEntries][cntc] = ch; // Storing a character
          cntc++;
        }
        break;

      case SKIP:
        if (ch == ' ' || ch == 10 || ch == 13)
          state = IDLE;
        break;

      default:
        state = IDLE;
        break;
      }
    }
  }
}

void Control::showEntries() const
{
  if (_nEntries == 0)
  {
    return;
  }

  oled.clear();

  uint8_t nDraw = menu.getNDraw();
  uint8_t iWin = menu.getIWin();
  uint8_t iSel = menu.getISel();

  for (uint8_t i = 0; i < nDraw; ++i)
  {
    if (iSel == iWin + i)
      oled.inverse(); // inverse video
    else
      oled.noInverse();

    if (i < 8)
      oled.setCursor(0, i);
    else
      oled.setCursor(8, (i - 8));

    const char *p = _filenames[iWin + i];
    uint8_t cnt = 0;
    while (*p != 0)
    {
      if (*p == '.')
        break;
      if (cnt > 7)
        break;
      oled.print(*p);
      ++cnt;
      ++p;
    }
  }
}

void Control::flushInput() const
{
  delay(200);
  while (Serial.available())
  {
    Serial.read();
  }
}