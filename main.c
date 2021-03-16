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

// Measuremtns methods
extern void initMeasure();
extern uint32_t getResistance();
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
    initMeasure();


    USER_DATA data;

    putsUart0("=======================================================\t\r\n");
    putsUart0("Low Cost Impedance Meter\t\r\n");
    putsUart0("Author: Sean-Michael Woerner\t\r\n");
    putsUart0("=======================================================\t\r\n");
    putsUart0("for more information type 'help'\t\r\n");


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

        // "clear": clear the terminal screen
        if(strCompare(cmd, "clear"))
        {
            clearScreen();
            valid2 = true;
        }

        // "help": list available commands and their functions
        if(strCompare(cmd,"help"))
        {
            putsUart0("Showing list of available terminal commands:\t\r\n");
            putsUart0("--------------------------------------------\t\r\n\n");
            putsUart0("Debug Commands:\t\r\n");
            putsUart0("---------------\t\r\n");
            putsUart0("(1)reset-------------Resets the hardware\t\r\n");
            putsUart0("(2)voltage-----------Returns the voltage across DUT2-DUT1. (The voltage is limited to 0 to 3.3V)\t\r\n");
            putsUart0("\t\r\n");
            putsUart0("LCR Commands:\t\r\n");
            putsUart0("-------------\t\r\n");
            putsUart0("(1)resistor----------Returns the resistance of the DUT\t\r\n");
            putsUart0("(2)capacitance-------Returns the capacitance of the DUT\t\r\n");
            putsUart0("(3)inductance--------Returns the inductance of the DUT\t\r\n");
            putsUart0("(4)esr---------------Returns the ESR of the inductor under test\t\r\n");
            putsUart0("(5)auto--------------Returns the value of the DUT that is most predominant\t\r\n");



            valid2 = true;
        }

        if(strCompare(cmd, "resistor"))
        {
            putsUart0("\t\r\nMeasuring Resistance...");
            uint32_t resistor = getResistance();
            char buffer[150];

            putsUart0("\t\r\nResistor: ");
            sprintf(buffer,"%d",resistor);
            putsUart0(buffer);
            putsUart0(" ohms");

            valid2 = true;
        }



    }







    while(true);
}










