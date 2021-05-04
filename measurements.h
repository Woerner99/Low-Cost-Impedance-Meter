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


void test_thing();
void initMeasurement();
void groundPins();
float getVoltage();
uint32_t getResistance();
uint32_t getCapacitance();
uint32_t getInductance();
double getESR();
void auto_measure();
void discharge();

#endif /* MEASURMENTS_H_ */
