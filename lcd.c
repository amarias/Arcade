/**
 * ==================================================================
 * LCD Data Structure
 * ==================================================================
 *
 * Horizontal Addressing:
 * In the horizontal addressing mode (V = 0), the X address increments after each byte.
 * After the last X address (X = 83), X wraps around to 0 and Y increments to address the next row.
 * Data is displayed from LSB (top) to MSB (bottom).
 * (see Fig.3, 4, 5, and 6 of the pcd8544 Data Sheet)
 *
 * ==================================================================
 * ==================================================================
 */

#include "tm4c123gh6pm.h"

// Add desired fonts here
#include "Fonts/regular.h"
#include "Fonts/cursive.h"
#include "Fonts/bold.h"

#include "lcd.h"
#include "timer.h"

#define PORT_A (1) // SSI0
#define GPIOPCTL (2)
#define FIFO_NOT_EMPTY ((SSI0_SR_R & (1<<0)) == 0) // The transmit FIFO is not empty

Font currentFont;

enum
{
    COLS = 84, ROWS = 48
};
// LCD Screen Size

/**
 * ==================================================================
 * Function Set Instructions for the LCD
 * ==================================================================
 */
enum
{
    BASIC_INSTRUCTION = (1 << 5),
    EXTENDED_INSTRUCTION = (1 << 5) | (1 << 0),
    VERTICAL_ADDRESSING = (1 << 1)
};

/**
 * ==================================================================
 * LCD Pins
 * ==================================================================
 *
 * PA2: Clock
 * PA3: SSI0Fss: CS on LCD
 * ~ PA4: MISO (Receive): Not used
 * PA5: MOSI (Transmit): DIN on LCD
 * PA6: GPIO: D/C (Data/Command) on LCD
 * PA7: GPIO: RST (Reset) on LCD
 *
 */
enum
{
    CLK = (1 << 2), CS = (1 << 3), DIN = (1 << 5), DC = (1 << 6), RST = (1 << 7)
};

/**
 * ==================================================================
 * SSI Config
 * ==================================================================
 *
 * SCR: SSI Serial Clock Rate
 * SPH: Desired clock phase
 * SPO: Desired clock polarity
 * FRF: SSI Frame Format Select: Protocol Mode
 * DSS: SSI Data Size Select: 8-bit data
 * SSE: SSI Synchronous Serial Port Enable
 * MASTER_MODE: SSI Master Mode: can be modified only when SSE=0
 *
 */
enum
{
    SCR = (0x01 << 8), SPH = (0 << 7), SPO = (0 << 6), FRF = (0x0 << 4), // Freescale SPI Frame Format
    DSS = (0x7 << 0),
    SSE = (1 << 1),
    MASTER_MODE = (~(1 << 2))
};

/* ================================================================== */

/**
 * Maps the index for each character per fontType
 */
const uint8_t textMap[][numOfFonts] = { { 0, 0, 0 }, // Space
        { 0, 0, 0 }, // !
        { 0, 0, 0 }, // "
        { 0, 0, 0 }, // #
        { 0, 0, 0 }, // $
        { 0, 0, 0 }, // %
        { 0, 0, 0 }, // &
        { 0, 0, 0 }, // '
        { 0, 0, 0 }, // (
        { 0, 0, 0 }, // )
        { 0, 0, 0 }, // *
        { 0, 0, 0 }, // +
        { 0, 0, 0 }, // ,
        { 0, 0, 0 }, // -
        { 0, 0, 0 }, // .
        { 0, 0, 0 }, // /
        { 0, 0, 0 }, // 0
        { 0, 0, 0 }, // 1
        { 0, 0, 0 }, // 2
        { 0, 0, 0 }, // 3
        { 0, 0, 0 }, // 4
        { 0, 0, 0 }, // 5
        { 0, 0, 0 }, // 6
        { 0, 0, 0 }, // 7
        { 0, 0, 0 }, // 8
        { 0, 0, 0 }, // 9
        { 0, 0, 0 }, // :
        { 0, 0, 0 }, // ;
        { 0, 0, 0 }, // <
        { 0, 0, 0 }, // =
        { 0, 0, 0 }, // >
        { 0, 0, 0 }, // ?
        { 0, 0, 0 }, // @
        { 0, 2, 0 }, // A
        { 0, 0, 0 }, // B
        { 0, 0, 0 }, // C
        { 0, 0, 0 }, // D
        { 0, 0, 0 }, // E
        { 0, 0, 0 }, // F
        { 0, 0, 0 }, // G
        { 0, 0, 0 }, // H
        { 0, 0, 0 }, // I
        { 0, 0, 0 }, // J
        { 0, 0, 0 }, // K
        { 0, 0, 0 }, // L
        { 0, 10, 0 }, // M
        { 0, 0, 0 }, // N
        { 0, 0, 0 }, // O
        { 0, 0, 0 }, // P
        { 0, 0, 0 }, // Q
        { 0, 0, 0 }, // R
        { 0, 0, 0 }, // S
        { 0, 0, 0 }, // T
        { 0, 0, 0 }, // U
        { 0, 0, 0 }, // V
        { 0, 0, 0 }, // W
        { 0, 0, 0 }, // X
        { 0, 0, 0 }, // Y
        { 0, 0, 0 }, // Z
        { 0, 0, 0 }, // [
        { 0, 0, 0 }, // '\'
        { 0, 0, 0 }, // ]
        { 0, 0, 0 }, // ^
        { 0, 0, 0 }, // _
        { 0, 0, 0 }, // `
        { 0, 22, 0 }, // a
        { 0, 0, 0 }, // b
        { 0, 0, 0 }, // c
        { 0, 0, 0 }, // d
        { 0, 0, 0 }, // e
        { 0, 0, 0 }, // f
        { 0, 0, 0 }, // g
        { 0, 0, 0 }, // h
        { 0, 28, 0 }, // i
        { 0, 0, 0 }, // j
        { 0, 0, 0 }, // k
        { 0, 32, 0 }, // l
        { 0, 0, 0 }, // m
        { 0, 35, 0 }, // n
        { 0, 41, 0 }, // o
        { 0, 0, 0 }, // p
        { 0, 0, 0 }, // q
        { 0, 46, 0 }, // r
        { 0, 52, 0 }, // s
        { 0, 0, 0 }, // t
        { 0, 0, 0 }, // u
        { 0, 0, 0 }, // v
        { 0, 0, 0 }, // w
        { 0, 0, 0 }, // x
        { 0, 0, 0 }, // y
        { 0, 0, 0 }, // z
        { 0, 0, 0 }, // {
        { 0, 0, 0 }, // |
        { 0, 0, 0 }, // }
        { 0, 0, 0 }  // ~
};

/* ================================================================== */

/**
 * RES pulse must be min 100ns within a maximum time of 100ms after VDD goes HIGH
 */
void reset()
{
    GPIO_PORTA_DATA_R &= ~(RST);
    wait_1ms(1);
    GPIO_PORTA_DATA_R |= RST;
}

/* SSI Transmit Data */
void transmitData(InstructionMode mode, uint8_t data)
{
    if (mode)
    {
        GPIO_PORTA_DATA_R |= DC;
    }
    else
    {
        GPIO_PORTA_DATA_R &= ~DC;
    }
    SSI0_DR_R = data;
    while (FIFO_NOT_EMPTY)
        ;
}

void transmitBasicInstruction(BasicInstruction instruction)
{
    transmitData(COMMAND_MODE, BASIC_INSTRUCTION);
    transmitData(COMMAND_MODE, instruction);
}

void transmitExtendedInstruction(ExtendedInstruction instruction)
{
    transmitData(COMMAND_MODE, EXTENDED_INSTRUCTION);
    transmitData(COMMAND_MODE, instruction);
}

void setAddress(uint8_t x, uint8_t y)
{
    if ((x <= 83) && (y <= 5))
    {
        transmitBasicInstruction((BasicInstruction) (X_ADDRESS | x));
        transmitBasicInstruction((BasicInstruction) (Y_ADDRESS | y));
    }
}

void clearDisplay()
{
    setAddress(0, 0);
    int i;
    for (i = 0; i < COLS * (ROWS / 8); ++i)
    {
        transmitData(DATA_MODE, 0x00);
    }
}

/**
 * Draws a single dot on a column
 * Requires an address
 * offset (0-7): which of the 8 bits to draw
 */
void drawPixel(uint8_t x, uint8_t y, uint8_t offset)
{
    setAddress(x, y);
    transmitData(DATA_MODE, 1 << offset);
}

/**
 * Create data for a vertical line
 * offset (0-7): where to (vertically) start drawing the line
 */
uint8_t getVerticalData(uint8_t offset, uint8_t width)
{
    uint8_t data = 0x00;
    int i;
    for (i = 0; i < width; i++)
    {
        data |= 1 << offset;
    }
    return data;
}

/**
 * Draws the given shape from left to right
 * Requires an address
 * offset (0-7): where to start drawing in the y-address
 * Returns x-address
 */
void drawShape(uint8_t x, uint8_t y, uint8_t offset, const uint8_t shape[],
               int arraySize)
{
    if ((x <= 83) && (y <= 5))
    {
        setAddress(x, y);

        int i;
        uint8_t data;

        for (i = 0; i < arraySize; i++)
        {
            data = shape[i] << offset;

            if (data != shape[i])
            { // check if the drawing spills over into the next y-address
                setAddress(x, y + 1);
                transmitData(DATA_MODE, shape[i] >> offset);
                setAddress(x, y);
            }

            transmitData(DATA_MODE, data);
            x++;
        }
    }
}

void drawText(uint8_t x, uint8_t y, uint8_t offset, char str[], int textSize,
              TextFont fontType)
{
    if ((x <= 83) && (y <= 5))
    {
        setAddress(x, y);
        int i, index, size;
        for (i = 0; i < textSize; i++)
        {
            index = textMap[str[i] - 0x20][fontType]; // textMap[char][fontType]
            size = *(currentFont.fontPtr + index);
            drawShape(x, y, offset, currentFont.fontPtr + index + 1, size);
            x += size;
        }
    }
}

void drawLogo()
{
    uint8_t offset = 4;
    uint32_t time = 200;
    uint8_t y = 2;

    drawText(33, y, offset, "AM", 2, CURSIVE);
    wait_1s(2);
    clearDisplay();

    drawText(31, y, offset, "AlMa", 4, CURSIVE);
    wait_1ms(time);
    clearDisplay();

    drawText(29, y, offset, "All Ma", 6, CURSIVE);
    wait_1ms(time);
    clearDisplay();

    drawText(27, y, offset, "Alli Mar", 8, CURSIVE);
    wait_1ms(time);
    clearDisplay();

    drawText(23, y, offset, "Allis Mari", 10, CURSIVE);
    wait_1ms(time);
    clearDisplay();

    drawText(19, y, offset, "Alliso Maria", 12, CURSIVE);
    wait_1ms(time);
    clearDisplay();

    drawText(11, y, offset, "Allison  Marias", 15, CURSIVE);
}

// Not finished
void menu()
{

}

void setFont(TextFont fontType)
{
    switch (fontType)
    {
    case CURSIVE:
        currentFont.fontPtr = cursiveChars;
        break;
    case BOLD:
        currentFont.fontPtr = boldChars;
        break;
    default:
        currentFont.fontPtr = regularChars;
        break;
    }
    currentFont.fontType = fontType;
}

/*
 * Initialization and Configuration of SSI Module 0 (see page 965 of the data sheet)
 */
void initSSI()
{
    SYSCTL_RCGCSSI_R |= 0x01;               // Enable the SSI module
    SYSCTL_RCGCGPIO_R |= PORT_A; // Enable the clock to Port A

    GPIO_PORTA_AFSEL_R |= (CLK | CS | DIN); // Enable alternate function
    GPIO_PORTA_AFSEL_R &= ~(DC | RST);      // Disable alternate function
    GPIO_PORTA_PCTL_R |=      // Assign the SSI signals to the appropriate pins
            ((GPIOPCTL << 8) | (GPIOPCTL << 12) | (GPIOPCTL << 20));
    GPIO_PORTA_PCTL_R &= ~(DC | RST);     // Assign as GPIO
    GPIO_PORTA_DIR_R |= (DC | RST);                     // Set Outputs
    GPIO_PORTA_DEN_R |= (CLK | CS | DIN | DC | RST); // Enable the pin's digital function
    GPIO_PORTA_DATA_R |= RST;                           // Set RST (HIGH)

    /* LCD Serial interface maximum 4.0 Mbits/s */
    /* SSInClk = SysClk / (CPSDVSR * (1 + SCR)) */
    SSI0_CR1_R &= ~SSE; // SSE bit is clear before making any configuration changes
    SSI0_CR1_R &= MASTER_MODE;    // Select whether the SSI is a master or slave
    SSI0_CC_R = 0x0;                           // Configure the SSI clock source
    SSI0_CPSR_R = 0x02; // Configure the clock prescale divisor; value must be an even number from 2 to 254
    SSI0_CR0_R = (SCR | SPH | SPO | FRF | DSS);     // Write SSI configuration
    SSI0_CR1_R |= SSE; // Enable the SSI by setting the SSE bit
}

void initLCD()
{
    initSSI();
    setFont(CURSIVE); // Assuming we start the program with the logo
    reset();

    transmitExtendedInstruction(
            (ExtendedInstruction) (BIAS_SYSTEM | MUX_RATE_1_48));
    transmitExtendedInstruction((ExtendedInstruction) (SET_VOP | VOP));
    transmitBasicInstruction(NORMAL_DISPLAY_MODE);

    clearDisplay();
    drawLogo();

    wait_1s(2);
    clearDisplay();

    setFont(REGULAR);
}
