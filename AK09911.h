
#ifndef AK09911_H
#define	AK09911_H

#ifdef	__cplusplus
extern "C" {
#endif

    
/*
Connections:
        3.3V <-> Vin
        Gnd <-> Gnd
        Master RD5 <-> SDA
        Master RD6 <-> SCL
        3.3V <-> RST
        3.3V or Gnd <-> CAD  (see below)
 */

    
    typedef struct {
        int xMag;
        int yMag;
        int zMag;
    } SensorValues;
    
    typedef struct {
        signed char xCoeff;
        signed char yCoeff;
        signed char zCoeff;
    } SensitivityCoefficients;
    
    typedef enum {MODE_POWER_DOWN = 0, MODE_SINGLE = 1, MODE_CONT_10HZ = 2,
            MODE_CONT_20HZ = 4, MODE_CONT_50HZ = 6, MODE_CONT_100HZ = 8,
            MODE_SELF_TEST = 16, MODE_FUSE_ROM = 31} OpMode;
    
#define STATUS_OK               0
#define STATUS_NO_DATA          1
#define STATUS_OVER_RUN         2
#define STATUS_OVERFLOW         8
#define STATUS_SELF_TEST_FAIL   128
    
//Uncomment one address define
#define I2C_ADDRESS 0x0C    //CAD = Gnd
//#define I2C_ADDRESS 0x0D  //CAD = 3.3V
            
    void InitAK09911(void);
    void ReadROM(SensitivityCoefficients *coefficients);
    char ReadCompass(SensorValues *values, SensitivityCoefficients *coefficients);
    void SetMode(OpMode mode);
    void SoftReset(void);
    char DataReady(void);
    char SelfTest(void);
    unsigned int ReadChipID(void);
    


#ifdef	__cplusplus
}
#endif

#endif	/* AK09911_H */

