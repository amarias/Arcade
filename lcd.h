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
void drawSpace();
void drawA(uint8_t x, uint8_t y);
void drawa(uint8_t x, uint8_t y);
void drawl(uint8_t x, uint8_t y);
void drawM(uint8_t x, uint8_t y);
void drawLogo();
void menu();

void initSSI();
void initLCD();

#endif // __LCD_H__
