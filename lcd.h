#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>

// LCD Screen
#define COLS 84
#define ROWS 48

void transmitData(uint8_t mode, uint8_t data);

void reset();

void setAddress(uint8_t x, uint8_t y);

void clearDisplay();
void drawShape(uint8_t x, uint8_t y, uint8_t offset, uint8_t shape[], int arraySize);
void drawPixel(uint8_t x, uint8_t y, uint8_t offset);
void drawLogo();
void menu();

void initSSI();
void initLCD();

#endif // __LCD_H__
