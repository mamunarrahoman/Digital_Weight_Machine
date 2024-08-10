#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#include <Arduino.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
HX711 scale;

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
static const unsigned char PROGMEM logo16_glcd_bmp[] = { B00000000, B11000000,
                                                         B00000001, B11000000,
                                                         B00000001, B11000000,
                                                         B00000011, B11100000,
                                                         B11110011, B11100000,
                                                         B11111110, B11111000,
                                                         B01111110, B11111111,
                                                         B00110011, B10011111,
                                                         B00011111, B11111100,
                                                         B00001101, B01110000,
                                                         B00011011, B10100000,
                                                         B00111111, B11100000,
                                                         B00111111, B11110000,
                                                         B01111100, B11110000,
                                                         B01110000, B01110000,
                                                         B00000000, B00110000 };

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

void setup() {
  Serial.begin(9600);

  pinMode(8, OUTPUT);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 0);
  display.println("WEIGHT MACHINE");
  display.display();

  display.drawRect(0, 10, 128, 50, WHITE);
  display.display();

  display.drawFastVLine(50, 10, 50, WHITE);
  display.drawFastHLine(0, 25, 128, WHITE);
  display.drawFastHLine(50, 42, 78, WHITE);
  display.drawFastVLine(95, 25, 35, WHITE);
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(7, 15);
  display.print("WEIGHT");

  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.setCursor(19, 47);
  // display.print("");
  // display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(65, 15);
  display.print("C.STATUS");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(55, 30);
  display.print("Ob.Pr?");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(55, 47);
  display.print("METAL?");
  display.display();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-459.542);
  //scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();  // reset the scale to 0
}


void loop() {
  int obj;
  int metal;
  obj = digitalRead(7);
  metal = digitalRead(6);

  if (metal == 0) {
    digitalWrite(8, HIGH);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(103, 47);
    display.print("NO");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(103, 47);
    display.print("YES");
    display.display();
  } else {
    digitalWrite(8, LOW);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(103, 47);
    display.print("YES");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(103, 47);
    display.print("NO");
    display.display();
  }

  if (obj == 1) {
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(103, 30);
    display.print("YES");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(103, 30);
    display.print("NO");
    display.display();
  } else {
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(103, 30);
    display.print("NO");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(103, 30);
    display.print("YES");
    display.display();
  }

  display.fillRect(5, 30, 35, 15, BLACK);

  if (scale.get_units(10) < 5) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 35);
    display.print(0);
    display.display();
  } else {
    if (scale.get_units(10) < 1000) {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 35);
      display.print(scale.get_units(10), 1);
      display.display();
      display.fillRect(13, 42, 30, 15, BLACK);
      display.display();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(19, 47);
      display.print("gm");
      display.display();
    } else {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 35);
      display.print(scale.get_units(10)/1000, 1);
      display.display();
      display.fillRect(13, 42, 30, 15, BLACK);
      display.display();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(19, 47);
      display.print("Kg");
      display.display();
    }
  }
}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];

  // initialize
  for (uint8_t f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    display.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}


void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
}

void testdrawcircle(void) {
  for (int16_t i = 0; i < display.height(); i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, WHITE);
    display.display();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // alternate colors
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, color % 2);
    display.display();
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i = 0; i < min(display.width(), display.height()) / 2; i += 5) {
    display.drawTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, WHITE);
    display.display();
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i = min(display.width(), display.height()) / 2; i > 0; i -= 5) {
    display.fillTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawroundrect(void) {
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, WHITE);
    display.display();
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawrect(void) {
  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, WHITE);
    display.display();
  }
}

void testdrawline() {
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, WHITE);
    display.display();
  }
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(0, 0, display.width() - 1, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, display.height() - 1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = display.width() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, WHITE);
    display.display();
  }
  delay(250);
}