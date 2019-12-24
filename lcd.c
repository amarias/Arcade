/**
 * ~LCD Data Structure~
 * Horizontal Addressing:
 *  In the horizontal addressing mode (V = 0), the X address increments after each byte.
 *  After the last X address (X = 83), X wraps around to 0 and Y increments to address the next row.
 *  Data is displayed from LSB (top) to MSB (bottom).
 *  (see Fig.3, 4, 5, and 6 of the pcd8544 Data Sheet)
 */

#include "tm4c123gh6pm.h"
#include "lcd.h"
#include "timer.h"
#include "letters.h"
#include <stdint.h>

/**
 * PA2: Clock
 * PA3: SSI0Fss: CS on LCD
 * ~ PA4: MISO (Receive): Not used
 * PA5: MOSI (Transmit): DIN on LCD
 * PA6: GPIO: D/C (Data/Command) on LCD
 * PA7: GPIO: RST (Reset) on LCD
 */

#define CLK (1 << 2)
#define CS (1 << 3)
#define DIN (1 << 5)
#define DC (1 << 6)
#define RST (1 << 7)

#define PORT_A (1) // SSI0
#define GPIOPCTL (2)

/**
 * SCR: SSI Serial Clock Rate
 * SPH: Desired clock phase
 * SPO: Desired clock polarity
 * FRF: SSI Frame Format Select: Protocol Mode
 * DSS: SSI Data Size Select: 8-bit data
 * SSE: SSI Synchronous Serial Port Enable
 * MASTER_MODE: SSI Master Mode: can be modified only when SSE=0
 */
#define SCR (0x01 << 8)
#define SPH (0 << 7)
#define SPO (0 << 6)
#define FRF (0x0 << 4) // Freescale SPI Frame Format
#define DSS (0x7 << 0)
#define SSE (1 << 1)
#define MASTER_MODE (~(1 << 2))

// Instruction set for the LCD (see PCD8544 Data Sheet)
#define VERTICAL_ADDRESSING (1<<1)
#define COMMAND_MODE (0x00)
#define DATA_MODE (0x01)
#define BASIC_INSTRUCTION (1<<5)
#define EXTENDED_INSTRUCTION (1<<5 | 1<<0)
#define BIAS_SYSTEM (1<<4)
#define MUX_RATE_1_48 (0<<2 | 1<<1 | 1<<0) // MUX Rate 1:48
#define SET_VOP (1<<7) // Set Contrast
#define VOP (0x3F)
#define NORMAL_DISPLAY_MODE (1<<3 | 1<<2 | 0<<0)
#define INVERSE_DISPLAY_MODE (1<<3 | 1<<2 | 1<<0)
#define X_ADDRESS (1<<7)
#define Y_ADDRESS (1<<6)

#define FIFO_NOT_EMPTY ((SSI0_SR_R & (1<<0)) == 0) // The transmit FIFO is not empty

// SSI Transmit Data
void transmitData(uint8_t mode, uint8_t data)
{
    if (mode == COMMAND_MODE)
    {
        GPIO_PORTA_DATA_R &= ~DC;
    }
    else
    {
        GPIO_PORTA_DATA_R |= DC;
    }
    SSI0_DR_R = data;
    while (FIFO_NOT_EMPTY)
        ;
}

/**
 * RES pulse must be min 100ns within a maximum time of 100ms after VDD goes HIGH
 */
void reset()
{
    GPIO_PORTA_DATA_R &= ~(RST);
    wait_1ms(1);
    GPIO_PORTA_DATA_R |= RST;
}

void setAddress(uint8_t x, uint8_t y)
{
    if ((x <= 83) && (y <= 5))
    {
        transmitData(COMMAND_MODE, BASIC_INSTRUCTION);
        transmitData(COMMAND_MODE, X_ADDRESS | x);
        transmitData(COMMAND_MODE, Y_ADDRESS | y);
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
 */
void drawShape(uint8_t x, uint8_t y, uint8_t offset, uint8_t shape[],
               int arraySize)
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

void drawLogo()
{
    uint8_t offset = 4;
    uint32_t time = 200;
    uint8_t x = 33;
    uint8_t y = 2;
    drawShape(x, y, offset, CURSIVE_A, CURSIVE_A_SIZE);
    x += CURSIVE_A_SIZE;
    drawShape(x, y, offset, CURSIVE_M, CURSIVE_M_SIZE);
    x += CURSIVE_M_SIZE;

    wait_1s(2);
    clearDisplay();

    x = 29;
    y = 2;
    drawShape(x, y, offset, CURSIVE_A, CURSIVE_A_SIZE);
    x += CURSIVE_A_SIZE - 1;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    x++; // empty space
    drawShape(x, y, offset, CURSIVE_M, CURSIVE_M_SIZE);
    x += CURSIVE_M_SIZE;
    drawPixel(x, y + 1, 3); // Instead of space, connect the cursive letters
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_A, CURSIVE_LOWER_A_SIZE);

    wait_1ms(time);
    clearDisplay();

    x = 27;
    y = 2;
    drawShape(x, y, offset, CURSIVE_A, CURSIVE_A_SIZE);
    x += CURSIVE_A_SIZE - 1;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_I, CURSIVE_LOWER_I_SIZE);
    x += CURSIVE_LOWER_I_SIZE;
    x += 2; // empty spaces
    drawShape(x, y, offset, CURSIVE_M, CURSIVE_M_SIZE);
    x += CURSIVE_M_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_A, CURSIVE_LOWER_A_SIZE);
    x += CURSIVE_LOWER_A_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_R, CURSIVE_LOWER_R_SIZE);

    wait_1ms(time);
    clearDisplay();

    x = 22;
    y = 2;
    drawShape(x, y, offset, CURSIVE_A, CURSIVE_A_SIZE);
    x += CURSIVE_A_SIZE - 1;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_I, CURSIVE_LOWER_I_SIZE);
    x += CURSIVE_LOWER_I_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_S, CURSIVE_LOWER_S_SIZE);
    x += CURSIVE_LOWER_S_SIZE;
    x += 2; // empty spaces
    drawShape(x, y, offset, CURSIVE_M, CURSIVE_M_SIZE);
    x += CURSIVE_M_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_A, CURSIVE_LOWER_A_SIZE);
    x += CURSIVE_LOWER_A_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_R, CURSIVE_LOWER_R_SIZE);
    x += CURSIVE_LOWER_R_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_I_FULL, CURSIVE_LOWER_I_FULL_SIZE);

    wait_1ms(time);
    clearDisplay();

    x = 16;
    y = 2;
    drawShape(x, y, offset, CURSIVE_A, CURSIVE_A_SIZE);
    x += CURSIVE_A_SIZE - 1;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_I, CURSIVE_LOWER_I_SIZE);
    x += CURSIVE_LOWER_I_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_S, CURSIVE_LOWER_S_SIZE);
    x += CURSIVE_LOWER_S_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_O, CURSIVE_LOWER_O_SIZE);
    x += CURSIVE_LOWER_O_SIZE;
    x += 3; // empty spaces
    drawShape(x, y, offset, CURSIVE_M, CURSIVE_M_SIZE);
    x += CURSIVE_M_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_A, CURSIVE_LOWER_A_SIZE);
    x += CURSIVE_LOWER_A_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_R, CURSIVE_LOWER_R_SIZE);
    x += CURSIVE_LOWER_R_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_I_FULL, CURSIVE_LOWER_I_FULL_SIZE);
    x += CURSIVE_LOWER_I_FULL_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_A, CURSIVE_LOWER_A_SIZE);

    wait_1ms(time);
    clearDisplay();

    x = 10;
    y = 2;
    drawShape(x, y, offset, CURSIVE_A, CURSIVE_A_SIZE);
    x += CURSIVE_A_SIZE - 1;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_L, CURSIVE_LOWER_L_SIZE);
    x += CURSIVE_LOWER_L_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_I, CURSIVE_LOWER_I_SIZE);
    x += CURSIVE_LOWER_I_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_S, CURSIVE_LOWER_S_SIZE);
    x += CURSIVE_LOWER_S_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_O, CURSIVE_LOWER_O_SIZE);
    x += CURSIVE_LOWER_O_SIZE;
    drawPixel(x, y + 1, 0);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_N, CURSIVE_LOWER_N_SIZE);
    x += CURSIVE_LOWER_N_SIZE;
    x += 3; // empty spaces
    drawShape(x, y, offset, CURSIVE_M, CURSIVE_M_SIZE);
    x += CURSIVE_M_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_A, CURSIVE_LOWER_A_SIZE);
    x += CURSIVE_LOWER_A_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_R, CURSIVE_LOWER_R_SIZE);
    x += CURSIVE_LOWER_R_SIZE;
    drawShape(x, y, offset, CURSIVE_LOWER_I_FULL, CURSIVE_LOWER_I_FULL_SIZE);
    x += CURSIVE_LOWER_I_FULL_SIZE;
    drawPixel(x, y + 1, 2);
    x++;
    drawShape(x, y, offset, CURSIVE_LOWER_A, CURSIVE_LOWER_A_SIZE);
    x += CURSIVE_LOWER_A_SIZE - 2;
    drawShape(x, y, offset, CURSIVE_LOWER_S, CURSIVE_LOWER_S_SIZE);
}

// Not finished
void menu()
{

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
    reset();

    transmitData(COMMAND_MODE, EXTENDED_INSTRUCTION);
    transmitData(COMMAND_MODE, BIAS_SYSTEM | MUX_RATE_1_48);
    transmitData(COMMAND_MODE, SET_VOP | VOP);

    transmitData(COMMAND_MODE, BASIC_INSTRUCTION);
    transmitData(COMMAND_MODE, NORMAL_DISPLAY_MODE);

    clearDisplay();
    drawLogo();

    wait_1s(2);
    clearDisplay();
}
