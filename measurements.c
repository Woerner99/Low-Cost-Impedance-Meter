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
#include <stdint.h>
#include <math.h>
#include "adc0.h"
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

#define V_REF 2.469
#define CAP_CONS 0.000000186
#define AIN9_MASK 2

#define NOT_CAP 0xCBAD
#define NOT_IND 0xFBAD


void test_thing()
{
    groundPins();

    /*
    setPinValue(MEASURE_LR,0);
    setPinValue(MEASURE_C,1);
    */


    setPinValue(HIGHSIDE,1);
    setPinValue(LOWSIDE,0);
    setPinValue(INTEGRATE,1);

}


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

    // Init ADC pin PE4
    selectPinAnalogInput(ADC);
    initAdc0();
    setAdc0Ss3Mux(9);
    //setAdc0Ss3Log2AverageCount(2);

    // Init AC (Analog Comparator)
    selectPinAnalogInput(AC);

    // Config Wide Timer 0
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R0;                // turn-on timer
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;   // turn off counter before reconfiguring
    WTIMER0_CFG_R = 4;                   // configure as 32-bit counter (A only)
    WTIMER0_TAMR_R = TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP | TIMER_TAMR_TACDIR; // configure for edge time mode, count up
    WTIMER0_CTL_R = TIMER_CTL_TAEVENT_POS; // measure time from positive edge to positive edge

    WTIMER0_TAV_R = 0;          // zero counter for first period

    SYSCTL_RCGCACMP_R |= 1;     // enable clocks

    COMP_ACREFCTL_R |= 0xF | (1 << 9) | (0 << 8);    // Vdda = 3.3V, EN = 1, RNG = 0, Vref = 0xF --> gives the 2.469V
    COMP_ACCTL0_R |= (2 << 9) | (1 << 1);            // ASRCP = 0x2 (Internal voltage reference = 2.469 from above),
                                                     // CINV = 1 (Comparator is inverted)
    // Wait 10us as stated in the data sheet
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
}

// Gets Voltage from DUT2 pin on PE4
float getVoltage()
{
    uint32_t raw = readAdc0Ss3();
    float voltage = 0.0;
    voltage = (raw /4096.0)*3.3;
    return voltage;
}

// Gets resistance and returns the value
uint32_t getResistance()
{
    groundPins();                           // ground all pins first
    setPinValue(INTEGRATE, 1);
    setPinValue(LOWSIDE, 1);                // discharge
    waitMicrosecond(10e5);                  // wait for discharge

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;       // disable timer

    WTIMER0_TAV_R = 0;                        // Reset TAV

    // Turn on pins to measure resistance
    setPinValue(LOWSIDE, 0);
    setPinValue(MEASURE_LR, 1);

    WTIMER0_CTL_R |= TIMER_CTL_TAEN;            //  Turn on timer

    // Do not commence until voltage reaches reference of 2.469V
    while (COMP_ACSTAT0_R == 0x00);

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Turn off counter
    groundPins();                              // Ground pins
    return ((WTIMER0_TAV_R/57)+1);             // Divide timer value with 57 to get Resistance value and return
}

// Gets Capacitance and returns value
uint32_t getCapacitance()
{
    groundPins();
    setPinValue(MEASURE_C, 1);
    setPinValue(LOWSIDE, 1);                 // discharge
    waitMicrosecond(10e5);                   // wait for discharge

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;        // disable timer

    WTIMER0_TAV_R = 0;                       // Reset TAV

    WTIMER0_CTL_R |= TIMER_CTL_TAEN;         // Turn on Timer

    // Turn on pins to measure capacitance
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 1);

    // Do not commence until voltage reaches reference of 2.469V
    while (COMP_ACSTAT0_R == 0x00)
    {
        // if timer goes on too long break out (breaks @ approx 150 micro)
        if(WTIMER0_TAV_R > 0x31ABA855)
        {
            putsUart0("capacitor took too long\t\r\n");
            WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Turn off counter
            groundPins();
            return(NOT_CAP);
        }
    }

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Turn off counter
    uint32_t test = WTIMER0_TAV_R;
    groundPins();                              // Ground pins
    return ((WTIMER0_TAV_R*CAP_CONS));         // Multiply timer value with capacitor constant and return
}

// Gets ESR and returns value
double getESR()
{
    groundPins();
    setPinValue(MEASURE_LR, 1);
    setPinValue(LOWSIDE, 1);         // discharge
    waitMicrosecond(10e5);           // wait for discharge

    double voltage = 0.0;
    voltage = getVoltage();          // get raw voltage on PE4

    // Calculate the ohms using voltage divider law:
    double ohms = 0.0;
    ohms = ((3.3*33.0 - voltage*33.0)/voltage);
    groundPins();
    return ohms;
}

// Gets Inductance and returns value
uint32_t getInductance()
{
    double t = 0.0;                 // time constant
    double r_in = 0.0;              // Rin = ESR + 33 ohms
    double esr = getESR();          // esr
    double i = 0.0;                 // current
    double inductance = 0.0;        // inductance (which will be returned)

    groundPins();
    setPinValue(MEASURE_C, 1);
    setPinValue(LOWSIDE, 1);        // discharge
    waitMicrosecond(10e5);          // wait for discharge

    setPinValue(MEASURE_C, 0);

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Disable Timer
    WTIMER0_TAV_R = 0;                        // Reset TAV

    WTIMER0_CTL_R |= TIMER_CTL_TAEN;        // Turn on timer

    setPinValue(MEASURE_LR, 1);              // Turn on pin to measure inductance

    // Do not commence until voltage reaches reference of 2.469V
      while (COMP_ACSTAT0_R == 0x00)
      {
          // if timer goes on too long break out (breaks @ approx 150 micro)
          if(WTIMER0_TAV_R > 0x31ABA855)
          {
              putsUart0("inductor took too long\t\r\n");
              WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Turn off counter
              groundPins();
              return(NOT_IND);
          }
      }


    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;          // Turn off counter

    t = ((double) (WTIMER0_TAV_R)) / (40000000);    // time constant = timer value / sys clock
    r_in = esr + 33.0;                              // Rin value
    i = V_REF / r_in;                               // 2.469V / Rin to get current

    inductance =  -(r_in * t) / (log(1- (r_in * i) / 3.3));

    groundPins();
    return (double) (inductance*1e6);
}

// Function to get measurement automatically. This function will determine if the connected component is
// either a Resistor, Capacitor, or an Inductor
void auto_measure()
{
    uint32_t res = getResistance();
    uint32_t cap = getCapacitance(); // 0xCBAD = NOT CAP value
    uint32_t ind = getInductance(); //  0xFBAD = NOT IND value

    // if NOT cap and NOT ind, print resistance
    if(cap == NOT_CAP && ind == NOT_IND)
    {
        char res_str[20];
        putsUart0("Resistor: ");
        sprintf(res_str,"%d",res);
        putsUart0(res_str);
        putsUart0(" ohms");
        return;
    }
    // print cap
    if(ind > 200 && res < 10)
    {
        char cap_str[20];
        putsUart0("Capacitor: ");
        sprintf(cap_str, "%d", cap);
        putsUart0(cap_str);
        putsUart0(" micro Farads");
        return;
    }
    // print ind
    if(cap == NOT_CAP && res < 100)
    {
        char ind_str[150];
        putsUart0("Inductance: ");
        sprintf(ind_str, "%d", ind);
        putsUart0(ind_str);
        putsUart0(" micro Henries");
        return;
    }

}


