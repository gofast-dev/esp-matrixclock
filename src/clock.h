#ifndef _DISPLAY_H
#define _DISPLAY_H
#define NUM_MAX			4

#include <Arduino.h>
#include "fonts.h"
#include "MD_MAX72xx.h"
//#include "max7219.h"

// void draw_font4x7(uint8_t column, uint8_t number);
// void draw_font4x8(uint8_t column, uint8_t number);
void initMX(MD_MAX72XX* mxin);
void draw_clock(uint8_t hour,uint8_t minute,uint8_t second);
void drawReset();
void drawIP();
void drawChar(uint8_t col, const uint8_t *fontdata, const char ch);
void drawString(uint8_t col, const uint8_t *fontdata, const char *string);
void drawStringClr(uint8_t col, const uint8_t *fontdata, const char *string);

#endif
