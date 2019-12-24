#ifndef __LETTERS_H__
#define __LETTERS_H__

#include <stdint.h>

// SIZE: The width of each letter

/**
 * Cursive Letters: Upper Case
 */

#define CURSIVE_A (uint8_t[]){0x3C, 0x42, 0x41, 0x41, 0x3D, 0x41, 0x40}
#define CURSIVE_A_SIZE 7
#define CURSIVE_M (uint8_t[]){0x02, 0x01, 0x7F, 0x02, 0x01, 0x01, 0x7E, 0x01, 0x01, 0x7E}
#define CURSIVE_M_SIZE 10

/**
 * Cursive Letters: Lower Case
 */

#define CURSIVE_LOWER_A (uint8_t[]){0x38, 0x44, 0x44, 0x3C, 0x40, 0x20}
#define CURSIVE_LOWER_A_SIZE 6
#define CURSIVE_LOWER_I (uint8_t[]){0x40, 0x3A}
#define CURSIVE_LOWER_I_SIZE 2
#define CURSIVE_LOWER_I_FULL (uint8_t[]){0x7A}
#define CURSIVE_LOWER_I_FULL_SIZE 1
#define CURSIVE_LOWER_L (uint8_t[]){0x40, 0x3F}
#define CURSIVE_LOWER_L_SIZE 2
#define CURSIVE_LOWER_N (uint8_t[]){0x7C, 0x08, 0x04, 0x04, 0x78}
#define CURSIVE_LOWER_N_SIZE 5
#define CURSIVE_LOWER_O (uint8_t[]){0x38, 0x44, 0x4C, 0x38}
#define CURSIVE_LOWER_O_SIZE 4
#define CURSIVE_LOWER_R (uint8_t[]){0x7C, 0x08, 0x04, 0x04, 0x08}
#define CURSIVE_LOWER_R_SIZE 5
#define CURSIVE_LOWER_S (uint8_t[]){0x40, 0X20, 0X10, 0X4C, 0X70}
#define CURSIVE_LOWER_S_SIZE 5

#endif // __LETTERS_H__
