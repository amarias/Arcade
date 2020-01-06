#ifndef CURSIVE_H
#define CURSIVE_H

#include <stdint.h>

// Size is given first, then the character
const uint8_t cursiveChars[] =
{
    1, 0x00,                                                                 // Space
    7, 0x3c, 0x42, 0x41, 0x41, 0x3d, 0x41, 0x40,                             // A
    11, 0x02, 0x01, 0x7F, 0x02, 0x01, 0x01, 0x7E, 0x01, 0x01, 0x7E, 0x00,    // M
    5, 0x38, 0x44, 0x44, 0x3C, 0x40,                                         // a
    3, 0x40, 0x3A, 0x40,                                                     // i
    2, 0x40, 0x3F,                                                           // l
    5, 0x7C, 0x08, 0x04, 0x04, 0x78,                                         // n
    4, 0x38, 0x44, 0x4C, 0x38,                                               // o
    5, 0x7C, 0x08, 0x04, 0x04, 0x08,                                         // r
    4, 0x20, 0x10, 0x4C, 0x70                                                // s
};

#endif // CURSIVE_H
