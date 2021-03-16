/*
 *  Measurements.h
 *
 *  Sean-Michael Woerner
 *  1001229459
 *
 *  3/15/2021
 */

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------
#include "tm4c123gh6pm.h"
#include <stdint.h>

#ifndef MEASURMENTS_H_
#define MEASURMENTS_H_

void initMeasure();
uint32_t measureResistance();
void disablePins();
void testBoard();

#endif /* MEASURMENTS_H_ */
