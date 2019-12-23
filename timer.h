#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

void initTimer();
void wait_1ms(uint32_t time);
void wait_1s(uint32_t time);

#endif // __TIMER_H__
