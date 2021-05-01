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
#include "measurements.h"
#include "wait.h"
#include "uart0.h"

#define RES_BUTTON           PORTB,5  // Pushbutton for Resistance
#define CAP_BUTTON           PORTB,0  // Pushbutton for Capacitance
#define IND_BUTTON           PORTB,1  // Pushbutton for Inductance

#define ESR_BUTTON           PORTB,6  // Pushbutton for ESR
#define AUTO_BUTTON          PORTB,7  // Pushbutton for Auto
#define VOLTAGE_BUTTON       PORTB,2  // Pushbutton for Voltage



// Function to initialize push buttons on PORT B
// We need to use interrupts on PORT B for this to work correctly
void initPushButtons()
{
    enablePort(PORTB);

    selectPinDigitalInput(RES_BUTTON);
    enablePinPullup(RES_BUTTON);
    selectPinInterruptBothEdges(RES_BUTTON);
    enablePinInterrupt(RES_BUTTON);

    selectPinDigitalInput(CAP_BUTTON);
    enablePinPullup(CAP_BUTTON);
    selectPinInterruptBothEdges(CAP_BUTTON);
    enablePinInterrupt(CAP_BUTTON);

    selectPinDigitalInput(IND_BUTTON);
    enablePinPullup(IND_BUTTON);
    selectPinInterruptBothEdges(IND_BUTTON);
    enablePinInterrupt(IND_BUTTON);

    selectPinDigitalInput(ESR_BUTTON);
    enablePinPullup(ESR_BUTTON);
    selectPinInterruptBothEdges(ESR_BUTTON);
    enablePinInterrupt(ESR_BUTTON);

    selectPinDigitalInput(AUTO_BUTTON);
    enablePinPullup(AUTO_BUTTON);
    selectPinInterruptBothEdges(AUTO_BUTTON);
    enablePinInterrupt(AUTO_BUTTON);

    selectPinDigitalInput(VOLTAGE_BUTTON);
    enablePinPullup(VOLTAGE_BUTTON);
    selectPinInterruptBothEdges(VOLTAGE_BUTTON);
    enablePinInterrupt(VOLTAGE_BUTTON);

    NVIC_EN0_R |= 1 << (INT_GPIOB-16); // Turn on interrupt 17 (GPIOB)
}

// Interrupt function when button is pressed
// Depending on which pin is high, do that measurement
void onButtonPress()
{
    if(!getPinValue(RES_BUTTON))
    {
        //putsUart0("PB5\t\r\n");
        putsUart0("\t\r\nMeasuring Resistance...");
        putsUart0("\t\r\n-----------------------\t\r\n");
        uint32_t resistor = getResistance();
        char res_str[150];

        putsUart0("Resistor: ");
        sprintf(res_str,"%d",resistor);
        putsUart0(res_str);
        putsUart0(" ohms");

    }
    if(!getPinValue(CAP_BUTTON))
    {
         //putsUart0("PB0\t\r\n");
         putsUart0("\t\r\nMeasuring Capacitance...");
         putsUart0("\t\r\n-----------------------\t\r\n");
         uint32_t cap = getCapacitance();

         // if capacitor out of range or took too long don't print value
         if(cap != 0xCBAD)
         {
             char cap_str[150];
             putsUart0("Capacitor: ");
             sprintf(cap_str, "%d", cap);
             putsUart0(cap_str);
             putsUart0(" micro Farads");
         }
    }
    if(!getPinValue(IND_BUTTON))
    {
        //putsUart0("PB1\t\r\n");
        putsUart0("\t\r\nMeasuring Inductance...");
        putsUart0("\t\r\n-----------------------\t\r\n");
        uint32_t inductance = getInductance();

        // if inductor out of range or took too long don't print value
        if(inductance != 0xFBAD)
        {
            char ind_str[150];
            putsUart0("Inductance: ");
            sprintf(ind_str, "%d", inductance);
            putsUart0(ind_str);
            putsUart0(" micro Henries");
        }
    }
    if(!getPinValue(ESR_BUTTON))
    {
         //putsUart0("PB2\t\r\n");
          putsUart0("\t\r\nMeasuring ESR...");
          putsUart0("\t\r\n-----------------------\t\r\n");
          double esr = 0.0;
          esr = getESR();
          char esr_str[150];

          putsUart0("ESR: ");
          sprintf(esr_str, "%f", esr);
          putsUart0(esr_str);
          putsUart0(" ohms");
    }
    if(!getPinValue(AUTO_BUTTON))
    {
        //putsUart0("PB7\t\r\n");
        putsUart0("\t\r\nDetecting Component Automatically...");
        putsUart0("\t\r\n------------------------------------\t\r\n");
        auto_measure();
    }
    if(!getPinValue(VOLTAGE_BUTTON))
    {
         //putsUart0("PB6\t\r\n");
        putsUart0("\t\r\nMeasuring Voltage...");
        putsUart0("\t\r\n--------------------\t\r\n");
        float voltage = getVoltage();
        char volt_str[150];

        putsUart0("Voltage: ");
        sprintf(volt_str,"%f",voltage);
        putsUart0(volt_str);
        putsUart0(" V");
    }


    // Clear interrupts and turn off, wait, and then turn back on

    clearPinInterrupt(RES_BUTTON);
    disablePinInterrupt(RES_BUTTON);
    clearPinInterrupt(CAP_BUTTON);
    disablePinInterrupt(CAP_BUTTON);
    clearPinInterrupt(IND_BUTTON);
    disablePinInterrupt(IND_BUTTON);
    clearPinInterrupt(ESR_BUTTON);
    disablePinInterrupt(ESR_BUTTON);
    clearPinInterrupt(AUTO_BUTTON);
    disablePinInterrupt(AUTO_BUTTON);
    clearPinInterrupt(VOLTAGE_BUTTON);
    disablePinInterrupt(VOLTAGE_BUTTON);

    waitMicrosecond(250000);

    clearPinInterrupt(RES_BUTTON);
    enablePinInterrupt(RES_BUTTON);
    clearPinInterrupt(CAP_BUTTON);
    enablePinInterrupt(CAP_BUTTON);
    clearPinInterrupt(IND_BUTTON);
    enablePinInterrupt(IND_BUTTON);
    clearPinInterrupt(ESR_BUTTON);
    enablePinInterrupt(ESR_BUTTON);
    clearPinInterrupt(AUTO_BUTTON);
    enablePinInterrupt(AUTO_BUTTON);
    clearPinInterrupt(VOLTAGE_BUTTON);
    enablePinInterrupt(VOLTAGE_BUTTON);

    putsUart0("\t\r\n\n>");                        // Clear line and new line for next cmd
}
