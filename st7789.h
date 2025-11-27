#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

#define ST7789_WIDTH  240
#define ST7789_HEIGHT 240

#define ST7789_PIN_DC   25
#define ST7789_PIN_BLK  26

#define COLOR_BLACK   0x0000
#define COLOR_BLUE    0x001F
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_WHITE   0xFFFF
#define COLOR_YELLOW  0xFFE0

#define TRANSPARENT_COLOR 0xF81F

void ST7789_Init(void);
void ST7789_ClearBuffer(uint16_t color);
void ST7789_DrawRect(int x, int y, int w, int h, uint16_t color);
void ST7789_DrawImage(int x, int y, int w, int h, const uint16_t* data);
void ST7789_UpdateScreen(void);

uint16_t* ST7789_LoadImage(const char* filename, int* w, int* h);

#endif