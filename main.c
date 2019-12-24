/**
 * TM4C123 Launchpad
 * Nokia 5110/3310 Monochrome LCD (Blue)
 *
 * At 3.3V the back light draws 80 mA. For a dimmer back light, the 3.3V can be connected to a 100 ohm resistor,
 * and the other end of the resistor to the LED/BL pin.
 */

#include "tm4c123gh6pm.h"
#include "timer.h"
#include "lcd.h"
#include <stdint.h>

#define PORT_F (1 << 5) // Green LED
#define LED_GREEN (1 << 3)

/**
 * Turns the launchpad LED green
 */
void greenLED(){
    GPIO_PORTF_DIR_R |= LED_GREEN; // Output
    GPIO_PORTF_DEN_R |= LED_GREEN; // Enable digital functions
    GPIO_PORTF_DATA_R |= LED_GREEN;
}

void setup(){
    SYSCTL_RCGCGPIO_R |= PORT_F;
    initTimer();
    greenLED();
    initLCD();
}

int main(void)
{
    setup();

    wait_1s(2);
    menu();
    while(1){
    }
	return 0;
}
