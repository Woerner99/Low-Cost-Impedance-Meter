/*
 * Pushbuttons.c
 *
 * Sean-Michael Woerner
 * 1001229459
 *
 * 5/1/2021
 */


//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "gpio.h"
#include "pushbuttons.h"
#include "wait.h"
#include "uart0.h"


// Function to initialize push buttons on PORT B
// We need to use interrupts on PORT B for this to work correctly
void initPushButtons()
{
    enablePort(PORTB);

    selectPinDigitalInput(RES_BUTTON);
    enablePinPullup(RES_BUTTON);
    selectPinInterruptBothEdges(RES_BUTTON);
    NVIC_EN0_R |= 1 << (INT_GPIOB-16); // Turn on interrupt 17 (GPIOB)
    enablePinInterrupt(RES_BUTTON);


}

// Interrupt function when button is pressed
// Depending on which pin is high, do that measurement
void onButtonPress()
{
    clearPinInterrupt(RES_BUTTON);
    disablePinInterrupt(RES_BUTTON);

    putsUart0("Button Pressed!\t\r\n");
    waitMicrosecond(250000);

    clearPinInterrupt(RES_BUTTON);
    enablePinInterrupt(RES_BUTTON);
}
