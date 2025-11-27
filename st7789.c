#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "st7789.h"

#define STB_IMAGE_IMPLEMENTATION # png image file load lib
#include "stb_image.h"

// ST7789 Commands
#define CMD_SWRESET     0x01
#define CMD_SLPOUT      0x11
#define CMD_COLMOD      0x3A
#define CMD_MADCTL      0x36
#define CMD_INVON       0x21
#define CMD_NORON       0x13
#define CMD_DISPON      0x29
#define CMD_CASET       0x2A
#define CMD_RASET       0x2B
#define CMD_RAMWR       0x2C

// Buffer -> UpdateScreen()
static uint16_t frameBuffer[ST7789_WIDTH * ST7789_HEIGHT];


static void SelectDataMode(void) {
    bcm2835_gpio_write(ST7789_PIN_DC, HIGH);
}

static void SelectCommandMode(void) {
    bcm2835_gpio_write(ST7789_PIN_DC, LOW);
}

static void WriteByte(uint8_t data) {
    bcm2835_spi_transfer(data);
}

static void WriteCommand(uint8_t cmd) {
    SelectCommandMode();
    WriteByte(cmd);
}

static void WriteData(uint8_t data) {
    SelectDataMode();
    WriteByte(data);
}


void ST7789_Init(void) {
    bcm2835_gpio_fsel(ST7789_PIN_DC, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(ST7789_PIN_BLK, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_write(ST7789_PIN_BLK, HIGH);

    WriteCommand(CMD_SWRESET);
    bcm2835_delay(150);

    WriteCommand(CMD_SLPOUT);
    bcm2835_delay(255);

    WriteCommand(CMD_COLMOD);
    WriteData(0x55);

    WriteCommand(CMD_MADCTL);
    WriteData(0x00);

    WriteCommand(CMD_INVON);
    WriteCommand(CMD_NORON);
    WriteCommand(CMD_DISPON);
}

static void SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    WriteCommand(CMD_CASET);
    WriteData(x0 >> 8);
    WriteData(x0 & 0xFF);
    WriteData(x1 >> 8);
    WriteData(x1 & 0xFF);

    WriteCommand(CMD_RASET);
    WriteData(y0 >> 8);
    WriteData(y0 & 0xFF);
    WriteData(y1 >> 8);
    WriteData(y1 & 0xFF);

    WriteCommand(CMD_RAMWR);
}

void ST7789_ClearBuffer(uint16_t color) {
    for (int i = 0; i < ST7789_WIDTH * ST7789_HEIGHT; i++) {
        frameBuffer[i] = color;
    }
}

void ST7789_DrawRect(int x, int y, int w, int h, uint16_t color) {
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            if (i >= 0 && i < ST7789_WIDTH && j >= 0 && j < ST7789_HEIGHT) {
                frameBuffer[j * ST7789_WIDTH + i] = color;
            }
        }
    }
}

void ST7789_DrawImage(int x, int y, int w, int h, const uint16_t* data) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            int screenX = x + i;
            int screenY = y + j;

            if (screenX >= 0 && screenX < ST7789_WIDTH && screenY >= 0 && screenY < ST7789_HEIGHT) {
                uint16_t color = data[j * w + i];
                if (color != TRANSPARENT_COLOR) {
                    frameBuffer[screenY * ST7789_WIDTH + screenX] = color;
                }
            }
        }
    }
}

void ST7789_UpdateScreen(void) {
    SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
    SelectDataMode();
    bcm2835_spi_transfern((char*)frameBuffer, sizeof(frameBuffer));
}

uint16_t* ST7789_LoadImage(const char* filename, int* w, int* h) {
    int channels;
    unsigned char* img_data = stbi_load(filename, w, h, &channels, 4);

    if (!img_data) {
        printf("Error: Failed to load image %s\n", filename);
        return NULL;
    }

    uint16_t* converted_data = (uint16_t*)malloc(sizeof(uint16_t) * (*w) * (*h));
    if (!converted_data) {
        stbi_image_free(img_data);
        return NULL;
    }

    for (int i = 0; i < (*w) * (*h); i++) {
        uint8_t r = img_data[i * 4 + 0];
        uint8_t g = img_data[i * 4 + 1];
        uint8_t b = img_data[i * 4 + 2];
        uint8_t a = img_data[i * 4 + 3];

        if (a < 128) {
            converted_data[i] = TRANSPARENT_COLOR;
        } else {
            converted_data[i] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }
    }

    stbi_image_free(img_data);
    return converted_data;
}
