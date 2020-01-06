#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/**
 * ==================================================================
 * Instructions for the LCD (see PCD8544 Data Sheet)
 * ==================================================================
 */

typedef enum
{
    COMMAND_MODE = 0x00, DATA_MODE = 0x01
} InstructionMode;

typedef enum
{
    NORMAL_DISPLAY_MODE = (1 << 3) | (1 << 2) | (0 << 0),
    INVERSE_DISPLAY_MODE = (1 << 3) | (1 << 2) | (1 << 0),
    X_ADDRESS = (1 << 7),
    Y_ADDRESS = (1 << 6)
} BasicInstruction;

typedef enum
{
    BIAS_SYSTEM = (1 << 4), // Set Bias System instruction
    MUX_RATE_1_48 = (0 << 2) | (1 << 1) | (1 << 0), // MUX Rate 1:48
    SET_VOP = (1 << 7), // Set Contrast instruction
    VOP = 0x3F
} ExtendedInstruction;

/**
 * ==================================================================
 * Fonts
 * ==================================================================
 */

typedef enum
{
    REGULAR, CURSIVE, BOLD, numOfFonts
} TextFont;

typedef struct Fonts {
    TextFont fontType;
    const uint8_t *fontPtr;
    const uint8_t *textMapPtr;
} Font;


/* ================================================================== */

void reset();

void transmitData(InstructionMode mode, uint8_t data);
void transmitBasicInstruction(BasicInstruction instruction);
void transmitExtendedInstruction(ExtendedInstruction instruction);

void setAddress(uint8_t x, uint8_t y);

void clearDisplay();
void drawShape(uint8_t x, uint8_t y, uint8_t offset, const uint8_t shape[], int arraySize);
void drawPixel(uint8_t x, uint8_t y, uint8_t offset);
void drawText(uint8_t x, uint8_t y, uint8_t offset, char text[], int textSize, TextFont fontType);
void drawLogo();
void menu();

void setFont(TextFont fontType);
void initSSI();
void initLCD();

#endif // LCD_H
