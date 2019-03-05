
#include <xc.h>
#include "AK09911.h"

void InitAK09911(void) {
    TRISD = 0b01100000; //MMSP2 uses RD5 as SDA, RD6 as SCL, both set as inputs
    SSP2ADD = 0x63; //100kHz
    SSP2CON1bits.SSPM = 0b1000; //I2C Master mode
    SSP2CON1bits.SSPEN = 1; //Enable MSSP
}

void ReadROM(SensitivityCoefficients *coefficients) {

}

char ReadCompass(SensorValues *values) {

}

void SetMode(OpMode mode) {

}

void SoftReset(void) {

}


void I2CWriteRegister(unsigned char reg, unsigned char byte) {
    char data;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    data = SSP2BUF; //Read SSPxBUF to make sure BF is clear
    SSP2BUF = 0x0D << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = byte; //Send byte
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
}

void I2CReadData(unsigned char reg, char count) {
    char i;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    i = SSP2BUF; //Read SSPxBUF to make sure BF is clear
    SSP2BUF = 0x0D << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RSEN = 1; //Restart condition
    while (SSP2CON2bits.RSEN == 1); //Wait for restart to finish
    SSP2BUF = (0x0D << 1) + 1; //address with R/W set for read
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    for (i = 0; i < count; ++i) {
        SSP2CON2bits.RCEN = 1; // enable master for 1 byte reception
        while (!SSP2STATbits.BF); // wait until byte received
        buffer[i] = SSP2BUF;
        if (i == count - 1) {
            SSP2CON2bits.ACKDT = 1;
        } else {
            SSP2CON2bits.ACKDT = 0;
        }
        SSP2CON2bits.ACKEN = 1; //Send ACK/NACK
        while (SSP2CON2bits.ACKEN != 0);
    }
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
}