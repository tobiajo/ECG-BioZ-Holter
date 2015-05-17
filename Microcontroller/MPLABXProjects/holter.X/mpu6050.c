/*
 * File:    mpu6050.c
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#include "mpu6050.h"

// Initialization.
void mpu6050_init(void) {
    // Pin directions
    TRISBbits.TRISB5 = 1; // MPU_INT = in
    TRISBbits.TRISB4 = 0; // MPU_AD0 = out

    // Address pin low
    LATBbits.LATB4 = 0;

    // I2C2 init
    I2C2BRG = 37;     // I2C clock = 400 kHz (Fcy = 16 MHz)
    I2C2CON = 0x8200; // Enables the I2C module, Slew rate control disabled
    IdleI2C2();

    mpu6050_config();
}

// Configuration settings.
void mpu6050_config(void) {
    // The device will come up in sleep mode upon power-up. Therefore must the
    // Power Management 1 register must be configured first.
    mpu6050_wreg(MPU6050_REG_PWR_MGMT_1,   0x01); // Sleep off, PLL with X-axis
    mpu6050_wreg(MPU6050_REG_SMPRT_DIV,    0x09); // Sample Rate = 100 Hz
    mpu6050_wreg(MPU6050_REG_CONFIG,       0x06); // DLPF_CFG = 6 (delay 19 ms)
    mpu6050_wreg(MPU6050_REG_GYRO_CONFIG,  0x08); // ± 500 °/s
    mpu6050_wreg(MPU6050_REG_ACCEL_CONFIG, 0x08); // ± 4g
}

// Reads from a register.
UINT8 mpu6050_rreg(UINT8 reg) {
    UINT8 value;

    // Start command sequence
    StartI2C2();
    IdleI2C2();

    // Select address
    MasterWriteI2C2((MPU6050_ADDR_AD0_LOW << 1) & 0xFE); // device
    IdleI2C2();
    MasterWriteI2C2(reg); // register
    IdleI2C2();

    // Read command
    RestartI2C2();
    IdleI2C2();
    MasterWriteI2C2((MPU6050_ADDR_AD0_LOW << 1) | 0x01);
    IdleI2C2();

    // Read
    value = MasterReadI2C2();
    IdleI2C2();
    NotAckI2C2();
    IdleI2C2();

    // Terminate command sequence
    StopI2C2();
    IdleI2C2();

    return value;
}

// Writes to a register.
void mpu6050_wreg(UINT8 reg, UINT8 value) {
    // Start command sequence
    StartI2C2();
    IdleI2C2();

    // Select address
    MasterWriteI2C2((MPU6050_ADDR_AD0_LOW << 1) & 0xFE); // device
    IdleI2C2();
    MasterWriteI2C2(reg); // register
    IdleI2C2();

    // Write
    MasterWriteI2C2(value);
    IdleI2C2();

    // Terminate command sequence
    StopI2C2();
    IdleI2C2();
}
