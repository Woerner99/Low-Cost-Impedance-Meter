/*
 *  Measurements.h
 *
 *  Sean-Michael Woerner
 *  1001229459
 *
 *  5/1/2021
 */

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include <stdint.h>

#ifndef PUSHBUTTONS_H_
#define PUSHBUTTONS_H_


void initPushButtons();
void onButtonPress();

#endif /* PUSHBUTTONS_H_ */
