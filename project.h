#ifndef __PROJECT__H
#define __PROJECT__H


//-----------------------------------------------------------------------------
// Serial Communication defines and functions
//-----------------------------------------------------------------------------
#define MAX_CHARS 80
#define MAX_FIELDS 6


void printMenu()
{
    putsUart0("=======================================================\t\r\n");
    putsUart0("Low Cost Impedance Meter\t\r\n");
    putsUart0("Author: Sean-Michael Woerner\t\r\n");
    putsUart0("=======================================================\t\r\n");
    putsUart0("for more information type 'help'\t\r\n");
}

void printHelp()
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
    putsUart0("\t\r\n");
    putsUart0("Terminal Commands: \t\r\n");
    putsUart0("-------------\t\r\n");
    putsUart0("(1)clear-------------Clears terminal screen\t\r\n");
    putsUart0("(2)help--------------Shows list of available commands\t\r\n");

}

// Reboots the system from the Terminal
void reboot()
{
    NVIC_APINT_R = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
}


typedef struct USER_DATA
{
    char buffer[MAX_CHARS+1];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char fieldType[MAX_FIELDS];

} USER_DATA;


// Cleans string buffer for next run in terminal interface loop
// prevents entering a wrong cmd that the user did not intend on entering
// such as set cmd---> status cmd being ran on the same entry
void clearBuffer(USER_DATA* data)
{
    uint8_t i = 0;
    for (i=0; i < MAX_CHARS; i++)
    {
        data->buffer[i] = 0;
    }
    for (i=0; i < MAX_FIELDS; i++)
    {
        data->fieldPosition[i] = 0;
        data->fieldType[i] = 0;
    }
        data->fieldCount = 0;

}


// Function that returns true if the entered strings are the same and false otherwise
bool strCompare(char *str1, char *str2)
{
    uint8_t count = 0;
    while (str1[count] != '\0' || str2[count] != '\0')
    {
        if (str1[count] != str2[count])
        {
            return false;
        }
        count++;
    }
    return true;
}

void clearScreen()
{
    uint8_t skip;
    for(skip = 0; skip<75; skip++)
    {
        // skip some lines on the terminal
        putsUart0("\t\r\n");
    }

}


// bool to check if a valid cmd
bool valid2 = false;


#endif
