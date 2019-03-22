#include <xc.h>
#include "LCD.h"
#include "../AK09911.h"

#pragma config FOSC=HSPLL
#pragma config WDTEN=OFF
#pragma config XINST=OFF

void InitPins(void);
void ConfigInterrupts(void);
void ConfigPeriph(void);

SensitivityCoefficients coef;

void main(void) {
    OSCTUNEbits.PLLEN = 1;
    LCDInit();
    lprintf(0, "AK09911");
    __delay_ms(750);
    InitPins();
    ConfigPeriph();
    ConfigInterrupts();
    InitAK09911();
    SoftReset();
    if (SelfTest() != STATUS_OK) {
        lprintf(1, "Self Test Fail");
        while (1);
    } else {
        lprintf(1, "Self Test Pass");
        __delay_ms(500);
    }
    ReadROM(&coef);
    //unsigned id = ReadChipID();
    //lprintf(1, "0x%04x", id);
    while (1) {
        SensorValues mag;
        SetMode(MODE_SINGLE);
        while (!DataReady());
        int status = ReadCompass(&mag, &coef);
        lprintf(0, "x:%.1f y:%.1f", mag.xMag * 0.6, mag.yMag * 0.6);
        lprintf(1, "z:%.1f", mag.zMag * 0.6);
        __delay_ms(500);
      
    }
}

void InitPins(void) {
    LATD = 0; //LED's are outputs
    TRISD = 0; //Turn off all LED's
    LATB = 0;
    TRISB = 0b00000001; //Button0 is input;
}

void ConfigInterrupts(void) {

    RCONbits.IPEN = 0; //no priorities.  This is the default.

    //Configure your interrupts here

    //set up INT0 to interrupt on falling edge
    INTCON2bits.INTEDG0 = 0; //interrupt on falling edge
    INTCONbits.INT0IE = 1; //Enable the interrupt
    //note that we don't need to set the priority because we disabled priorities (and INT0 is ALWAYS high priority when priorities are enabled.)
    INTCONbits.INT0IF = 0; //Always clear the flag before enabling interrupts

    INTCONbits.GIE = 1; //Turn on interrupts
}

void ConfigPeriph(void) {

    //Configure peripherals here

    
}


void __interrupt(high_priority) HighIsr(void) {
    //Check the source of the interrupt
    if (INTCONbits.INT0IF == 1) {
        //source is INT0
        
        INTCONbits.INT0IF = 0; //must clear the flag to avoid recursive interrupts
    }
}


