/* 
 * File:   AK09911.h
 * Author: bmcgarvey
 *
 * Created on March 1, 2019, 10:44 AM
 */

#ifndef AK09911_H
#define	AK09911_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        int xMag;
        int yMag;
        int zmag;
    } SensorValues;
    
    typedef struct {
        signed char xCoeff;
        signed char yCoeff;
        signed char zCoeff;
    } SensitivityCoefficients;
    
    typedef enum {MODE_POWER_DOWN = 0; MODE_SINGLE = 1, MODE_CONT_10HZ = 2,
            MODE_CONT_20HZ = 4, MODE_CONT_50HZ = 6, MODE_CONT_100HZ = 8,
            MODE_SELF_TEST = 16, MODE_FUSE_ROM = 31} OpMode;
    
    void InitAK09911(void);
    void ReadROM(SensitivityCoefficients *coefficients);
    char ReadCompass(SensorValues *values);
    void SetMode(OpMode mode);
    void SoftReset(void);
    


#ifdef	__cplusplus
}
#endif

#endif	/* AK09911_H */

