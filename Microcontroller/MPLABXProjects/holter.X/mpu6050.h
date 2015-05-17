/*
 * File:    mpu6050.h
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#ifndef MPU6050_H
#define	MPU6050_H

//------------------------------------------------------------------------------
// Libraries

#define FOSC 32000000UL
#define FCY  FOSC/2
#include <xc.h> // MPLAB® XC16 C Compiler
#include <GenericTypeDefs.h>
#include <i2c.h>

//------------------------------------------------------------------------------
// Constants

// I2C address
#define MPU6050_ADDR_AD0_LOW        0x68 // AD0 = 0
#define MPU6050_ADDR_AD0_HIGH       0x69 // AD0 = 1

// Registers - Read / Write
#define MPU6050_REG_SMPRT_DIV       0x19 // Sample Rate Divider
#define MPU6050_REG_CONFIG          0x1A // Configuration
#define MPU6050_REG_GYRO_CONFIG     0x1B // Gyroscope Configuration
#define MPU6050_REG_ACCEL_CONFIG    0x1C // Accelerometer Configuration
#define MPU6050_REG_PWR_MGMT_1      0x6B // Power Management 1

// Registers - Read Only
#define MPU6050_REG_ACCEL_XOUT_H    0x3B // Accelerometer Measurements
#define MPU6050_REG_ACCEL_XOUT_L    0x3C // -"-
#define MPU6050_REG_ACCEL_YOUT_H    0x3D // -"-
#define MPU6050_REG_ACCEL_YOUT_L    0x3E // -"-
#define MPU6050_REG_ACCEL_ZOUT_H    0x3F // -"-
#define MPU6050_REG_ACCEL_ZOUT_L    0x40 // -"-
#define MPU6050_REG_GYRO_XOUT_H     0x43 // Gyroscope Measurements
#define MPU6050_REG_GYRO_XOUT_L     0x44 // -"-
#define MPU6050_REG_GYRO_YOUT_H     0x45 // -"-
#define MPU6050_REG_GYRO_YOUT_L     0x46 // -"-
#define MPU6050_REG_GYRO_ZOUT_H     0x47 // -"-
#define MPU6050_REG_GYRO_ZOUT_L     0x48 // -"-
#define MPU6050_REG_WHO_AM_I        0x75 // Who Am I

//------------------------------------------------------------------------------
// Prototypes

void  mpu6050_init  (void);
void  mpu6050_config(void);
UINT8 mpu6050_rreg  (UINT8);
void  mpu6050_wreg  (UINT8, UINT8);

#endif	/* MPU6050_H */
