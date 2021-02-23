#ifndef __PROJECT__H
#define __PROJECT__H


//-----------------------------------------------------------------------------
// Serial Communication defines and functions
//-----------------------------------------------------------------------------
#define MAX_CHARS 80
#define MAX_FIELDS 6


typedef struct USER_DATA
{
    char buffer[MAX_CHARS+1];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char fieldType[MAX_FIELDS];

} USER_DATA;


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
