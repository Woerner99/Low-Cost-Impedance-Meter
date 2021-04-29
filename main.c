/* Main.c for Low Cost Impedance Meter
 * (Embedded 2)
 *
 * Sean-Michael Woerner 1001229459
 * CSE 4342-001
 * 02/15/2021

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

Target Platform: EK-TM4C123GXL Evaluation Board
 Target uC:       TM4C123GH6PM
 System Clock:    40 MHz

 Hardware configuration:
  Red LED:
   PF1 drives an NPN transistor that powers the red LED
  Green LED:
   PF3 drives an NPN transistor that powers the green LED
 UART Interface:
   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
   Configured to 115,200 baud, 8N1
 Ethernet Module:
  ENC28J60 Module with HanRun HR911105A 19/15 Ethernet Port
  Pin usage is shown on ethernet.c
 */

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "gpio.h"
#include "project.h"
#include "spi0.h"
#include "uart0.h"
#include "wait.h"

#define RED_LED PORTF,1
#define BLUE_LED PORTF,2
#define GREEN_LED PORTF,3


//-----------------------------------------------------------------------------
// External methods
//-----------------------------------------------------------------------------

// Terminal interface methods
extern void initTerminal();
extern char getcUart0();
extern void getsUart0(USER_DATA* data);
extern void parseFields(USER_DATA* data);
extern bool isCommand(USER_DATA* data, const char strCommand[], uint8_t minArguments);
extern int32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber);
extern char* getFieldString(USER_DATA* data, uint8_t fieldNumber);

// Measurement methods
extern void initMeasurement();
extern uint32_t getResistance();
extern uint32_t getCapacitance();
extern double getESR();
extern uint32_t getInductance();
extern float getVoltage();
extern void auto_measure();
extern void groundPins();




//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    //Begin initialization of Hardware
    initTerminal();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initMeasurement();

    USER_DATA data;

    printMenu();            // print main menu on startup

    while(true)
    {

        valid2 = false;                               // make false to check for correct commands
        putsUart0("\t\r\n\n>");                        // Clear line and new line for next cmd
        getsUart0(&data);                        // Get string from user
        parseFields(&data);                      // Parse the fields from user input
        char *cmd = getFieldString(&data, 0);

        //-----------------------------------------------------------------------------
        // COMMANDS FOR USER
        //-----------------------------------------------------------------------------

        if(strCompare(cmd, "auto"))
        {
            putsUart0("\t\r\nDetecting Component Automatically...");
            putsUart0("\t\r\n-----------------------\t\r\n");
            auto_measure();

        }

        // "clear": clear the terminal screen
        if(strCompare(cmd, "clear"))
        {
            clearScreen();
            clearBuffer(&data);
            valid2 = true;
        }

        // "help": list available commands and their functions
        if(strCompare(cmd,"help"))
        {
            printHelp();
            clearBuffer(&data);
            valid2 = true;
        }

        if(strCompare(cmd, "capacitance") || strCompare(cmd, "cap"))
        {
            putsUart0("\t\r\nMeasuring Capacitance...");
            putsUart0("\t\r\n-----------------------\t\r\n");
            uint32_t cap = getCapacitance();
            char cap_str[150];

            putsUart0("Capacitor: ");
            sprintf(cap_str, "%d", cap);
            putsUart0(cap_str);
            putsUart0(" micro Farads");

            clearBuffer(&data);
            valid2 = true;
        }

        if(strCompare(cmd, "esr"))
        {
              putsUart0("\t\r\nMeasuring ESR...");
              putsUart0("\t\r\n-----------------------\t\r\n");
              double esr = 0.0;
              esr = getESR();
              char esr_str[150];

              putsUart0("ESR: ");
              sprintf(esr_str, "%f", esr);
              putsUart0(esr_str);
              putsUart0(" ohms");

              clearBuffer(&data);
              valid2 = true;
        }

        if(strCompare(cmd, "inductance") || strCompare(cmd, "in"))
        {
              putsUart0("\t\r\nMeasuring Inductance...");
              putsUart0("\t\r\n-----------------------\t\r\n");
              uint32_t inductance = getInductance();
              char ind_str[150];

              putsUart0("Inductance: ");
              sprintf(ind_str, "%d", inductance);
              putsUart0(ind_str);
              putsUart0(" micro Henries");

              clearBuffer(&data);
              valid2 = true;
        }

        if(strCompare(cmd, "resistor") || strCompare(cmd, "res"))
        {
            putsUart0("\t\r\nMeasuring Resistance...");
            putsUart0("\t\r\n-----------------------\t\r\n");
            uint32_t resistor = getResistance();
            char res_str[150];

            putsUart0("Resistor: ");
            sprintf(res_str,"%d",resistor);
            putsUart0(res_str);
            putsUart0(" ohms");

            clearBuffer(&data);
            valid2 = true;
        }

        if(strCompare(cmd, "reset") || strCompare(cmd, "reboot"))
        {
            putsUart0("\t\r\nRebooting System ...\t\r\n");
            waitMicrosecond(200000);
            reboot();
            clearBuffer(&data);
        }

        if(strCompare(cmd, "voltage"))
        {
            putsUart0("\t\r\nMeasuring Voltage...");
            putsUart0("\t\r\n--------------------\t\r\n");
            float voltage = getVoltage();
            char volt_str[150];

            putsUart0("Voltage: ");
            sprintf(volt_str,"%f",voltage);
            putsUart0(volt_str);
            putsUart0(" V");

            clearBuffer(&data);
        }


    }







    while(true);
}










