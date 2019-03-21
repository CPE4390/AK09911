
#include <xc.h>
#include "AK09911.h"

#define _XTAL_FREQ  32000000L

void I2CWriteRegister(unsigned char reg, unsigned char byte);
void I2CReadData(unsigned char reg, unsigned char *buffer, char count);
unsigned char I2CReadRegister(unsigned char reg);

void InitAK09911(void) {
    TRISD = 0b01100000; //MMSP2 uses RD5 as SDA, RD6 as SCL, both set as inputs
    SSP2ADD = 0x19; //400kHz
    SSP2CON1bits.SSPM = 0b1000; //I2C Master mode
    SSP2CON1bits.SSPEN = 1; //Enable MSSP
}

void ReadROM(SensitivityCoefficients *coefficients) {
    SetMode(MODE_FUSE_ROM);
    I2CReadData(0x60, (unsigned char *)coefficients, 3);
    SetMode(MODE_POWER_DOWN);
}

char ReadCompass(SensorValues *values, SensitivityCoefficients *coefficients) {
    char result;
    result = I2CReadRegister(0x10);
    if ((result & 0x01) == 0) {
        return STATUS_NO_DATA;
    }
    result &= 0x02;
    I2CReadData(0x11, (unsigned char *) values, 6);
    result |= I2CReadRegister(0x18); //Read HOFL bit and clear data ready
    if (coefficients != NULL) {
        __int24 adjust;
        adjust = ((__int24)(values->xMag)) * coefficients->xCoeff;
        adjust /= 128;
        values->xMag += (int)adjust;
        adjust = ((__int24)(values->yMag)) * coefficients->yCoeff;
        adjust /= 128;
        values->yMag += (int)adjust;
        adjust = ((__int24)(values->zMag)) * coefficients->zCoeff;
        adjust /= 128;
        values->zMag += (int)adjust;
    }
    return result;
}

void SetMode(OpMode mode) {
    if (mode != MODE_POWER_DOWN) {
        I2CWriteRegister(0x31, MODE_POWER_DOWN);
        __delay_us(100);
    }
    I2CWriteRegister(0x31, mode);
    __delay_us(100);
}

void SoftReset(void) {
    I2CWriteRegister(0x32, 0x01);
    __delay_ms(1);
}

char DataReady(void) {
    return (I2CReadRegister(0x10) & 0x01);
}

char SelfTest(void) {
    SensitivityCoefficients c;
    SensorValues v;
    ReadROM(&c);
    SetMode(MODE_SELF_TEST);
    while (!DataReady());
    ReadCompass(&v, &c);
    SetMode(MODE_POWER_DOWN);
    if (v.xMag < -30 || v.xMag > 30) {
        return STATUS_SELF_TEST_FAIL;
    }
    if (v.yMag < -30 || v.yMag > 30) {
        return STATUS_SELF_TEST_FAIL;
    }
    if (v.zMag < -400 || v.zMag > -50) {
        return STATUS_SELF_TEST_FAIL;
    }
    return STATUS_OK;
}

unsigned int ReadChipID(void) {
    unsigned int value;
    I2CReadData(0x00, (unsigned char *)&value, 2);
    return value;
}

void I2CWriteRegister(unsigned char reg, unsigned char byte) {
    char data;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    data = SSP2BUF; //Read SSPxBUF to make sure BF is clear
    SSP2BUF = I2C_ADDRESS << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = byte; //Send byte
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
}

unsigned char I2CReadRegister(unsigned char reg) {
    unsigned char data;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    SSP2BUF = I2C_ADDRESS << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RSEN = 1; //Restart condition
    while (SSP2CON2bits.RSEN == 1); //Wait for restart to finish
    SSP2BUF = (I2C_ADDRESS << 1) + 1; //address with R/W set for read
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RCEN = 1; // enable master for 1 byte reception
    while (!SSP2STATbits.BF); // wait until byte received
    data = SSP2BUF;
    SSP2CON2bits.ACKDT = 1;
    SSP2CON2bits.ACKEN = 1; //Send ACK/NACK
    while (SSP2CON2bits.ACKEN != 0);
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
    return data;
}

void I2CReadData(unsigned char reg, unsigned char *buffer, char count) {
    char i;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    i = SSP2BUF; //Read SSPxBUF to make sure BF is clear
    SSP2BUF = I2C_ADDRESS << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RSEN = 1; //Restart condition
    while (SSP2CON2bits.RSEN == 1); //Wait for restart to finish
    SSP2BUF = (I2C_ADDRESS << 1) + 1; //address with R/W set for read
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