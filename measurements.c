/*
 * Measurements.c
 *
 * Sean-Michael Woerner
 * 1001229459
 *
 * 3/15/2021
 */


//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------
#include "measurements.h"
#include "gpio.h"
#include "wait.h"

#define BLUE_LED PORTF,2
#define RED_LED PORTF,1

#define MEASURE_LR PORTA,3 //PA3
#define MEASURE_C PORTE,2 //PE2
#define INTEGRATE PORTE,5 //PE5
#define HIGHSIDE PORTA,2 //PA2
#define LOWSIDE PORTA,7 //PA7

#define ADC PORTE,4 //analog PE4 ADC
#define AC PORTC,7 //PC7 AC

#define VREF 2.469*57
#define CAP_CONS 0.000000186

// Inits the pins for measuring
void initMeasurement()
{
    // Enable clocks
    enablePort(PORTA);
    enablePort(PORTB);
    enablePort(PORTC);
    enablePort(PORTE);
    enablePort(PORTF);

    _delay_cycles(3);

    // Init LEDs
    selectPinPushPullOutput(BLUE_LED);
    selectPinPushPullOutput(RED_LED);

    // Init pins
    selectPinPushPullOutput(MEASURE_LR);
    selectPinPushPullOutput(MEASURE_C);
    selectPinPushPullOutput(INTEGRATE);
    selectPinPushPullOutput(HIGHSIDE);
    selectPinPushPullOutput(LOWSIDE);

    // Init ADC
    selectPinAnalogInput(ADC);

    // Init AC (Analog Comparator)
    selectPinAnalogInput(AC);


    // Config Wide Timer 0
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R0;                // turn-on timer
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;   // turn off counter before reconfiguring
    WTIMER0_CFG_R = 4;                   // configure as 32-bit counter (A only)
    WTIMER0_TAMR_R = TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP | TIMER_TAMR_TACDIR; // configure for edge time mode, count up
    WTIMER0_CTL_R = TIMER_CTL_TAEVENT_POS; // measure time from positive edge to positive edge

    WTIMER0_TAV_R = 0;          // zero counter for first period
    WTIMER0_TBV_R = 0;

    SYSCTL_RCGCACMP_R |= 1;     // enable clocks

    COMP_ACREFCTL_R |= 0xF | (1 << 9) | (0 << 8);    // Vdda = 3.3V, EN = 1, RNG = 0, Vref = 0xF --> gives the 2.469V

    COMP_ACCTL0_R |= (2 << 9) | (1 << 1);            // ASRCP = 0x2 (Internal voltage reference = 2.469 from above),
                                                     // CINV = 1 (Comparator is inverted)
    //wait 10us as stated in data sheet
    waitMicrosecond(10);

}

// Grounds all pins used in hardware
void groundPins()
{
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 0);
    setPinValue(LOWSIDE, 0);
    setPinValue(MEASURE_LR, 0);
    setPinValue(MEASURE_C, 0);
    setPinValue(INTEGRATE, 0);
    setPinValue(HIGHSIDE, 0);
//    setPinValue(ADC, 0);
//    setPinValue(AC, 0);

}
void testHighside()
{
    groundPins();
    setPinValue(HIGHSIDE, 1);

    waitMicrosecond(10e6);
    setPinValue(HIGHSIDE, 0);
}

// Gets resistance and returns the value
uint32_t getResistance()
{
    groundPins();                           // ground all pins first
    setPinValue(INTEGRATE, 1);
    setPinValue(LOWSIDE, 1);                // discharge
    waitMicrosecond(10e5);                  // wait for discharge

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;       // disable timer

    // Reset TAV and TBV
    WTIMER0_TAV_R = 0;


    // Turn on pins to measure resistance
    setPinValue(LOWSIDE, 0);
    setPinValue(MEASURE_LR, 1);

    //  Turn on timer
    WTIMER0_CTL_R |= TIMER_CTL_TAEN;

    // Do not commence until voltage reaches reference of 2.469V
    while (!(COMP_ACSTAT0_R & (1 << 1)));      // OVAL = 1 (VIN- < VIN+), with VIN- = charge and VIN+ = 2.469V

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Turn off counter
    groundPins();                              // Ground pins
    return ((WTIMER0_TAV_R/57)+1);             // Divide timer value with 57 to get Resistance value and return
}

// Gets Capacitance and returns value
uint32_t getCapacitance()
{
    groundPins();
    //setPinValue(INTEGRATE, 1);
    setPinValue(MEASURE_C, 1);
    setPinValue(LOWSIDE, 1);                 // discharge
    waitMicrosecond(10e5);                   // wait for discharge

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;        // disable timer

    // Reset TAV
    WTIMER0_TAV_R = 0;

    // Turn on timer
    WTIMER0_CTL_R |= TIMER_CTL_TAEN;

    // Turn on pins to measure capacitance
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 1);

    // Do not commence until voltage reaches reference of 2.469V
   // while (!(COMP_ACSTAT0_R & (1 << 1)));      // OVAL = 1 (VIN- < VIN+), with VIN- = charge and VIN+ = 2.469V
    while ((COMP_ACSTAT0_R == 0x00));

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Turn off counter
    groundPins();                              // Ground pins

    uint32_t test = (WTIMER0_TAV_R);

    return ((WTIMER0_TAV_R*CAP_CONS));             // Divide timer value with 57 to get Resistance value and return


}

void testBoard()
{
    //RIGHT SIDE

    //MEASURE_LR =0, MEASURE_C = 0
    setPinValue(BLUE_LED, 1);

    setPinValue(MEASURE_LR, 0);
    setPinValue(MEASURE_C, 0);
    waitMicrosecond(1e6);
    //expected output = floating (.9V~1.1V)
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    waitMicrosecond(1e6);
    groundPins();

    //MEASURE_LR =1, MEASURE_C = 0, DUT1 ~ 3.2+V (logical high)
    setPinValue(BLUE_LED, 1);
    setPinValue(MEASURE_LR, 1);
    setPinValue(MEASURE_C, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    groundPins();
    waitMicrosecond(1e6);

    //MEASURE_LR =0, MEASURE_C = 1, output ~ .02V (logical low)
    setPinValue(BLUE_LED, 1);
    setPinValue(MEASURE_LR, 0);
    setPinValue(MEASURE_C, 1);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    groundPins();
    waitMicrosecond(1e6);

    //RIGHT SIDE BOARD

    //Highside = 0, Lowside = 0, Integrate=0, output = floating (.9V~1.1V)
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 0);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    groundPins();
    waitMicrosecond(1e6);

    //Highside = 1, Lowside = 0, Integrate=0, output ~3V-3.15V
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 1);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    groundPins();
    waitMicrosecond(1e6);

    //Highside = 0, Lowside = 1, Integrate=0, output (.2V) logical low
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 1);
    setPinValue(HIGHSIDE, 0);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    groundPins();
    waitMicrosecond(1e6);

    //Highside = 1, Lowside = 1, Integrate=0, output = (.15-.25V)
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 1);
    setPinValue(HIGHSIDE, 1);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    groundPins();
    waitMicrosecond(1e6);

    //Highside = 1, Lowside = 0, Integrate=1, output = (3-3.15V)
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 1);
    setPinValue(INTEGRATE, 1);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    groundPins();
    waitMicrosecond(1e6);

    //dump the capacitor
    setPinValue(INTEGRATE, 1);
    setPinValue(LOWSIDE, 1); //discharge //ground both sides of capacitor
    waitMicrosecond(10e3); //wait a reasonable time

}


