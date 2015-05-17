/*
 * File:    main.c
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#include "main.h"

// Drives the device.
int main(void) {
    int i;
    init();

    // Transmission
    while (1) {
        // Start field
        hc05_put(0x33);

        // ADS1292R data
        for (i = 0; i < 10; i++) {
            while (PORTBbits.RB7 == 1); // wait for ADS_DRDY
            ads1292r_to_hc05();
        }

        // MPU-6050 data
        mpu6050_to_hc05();

        // Checksum
        hc05_put(0x55);
    }
}

// Initialization.
void init(void) {
    // FRC Postscaler, 000 = 8 MHz (divide by 1)
    CLKDIVbits.RCDIV2 = 0;
    CLKDIVbits.RCDIV1 = 0;
    CLKDIVbits.RCDIV0 = 0;

    // All ANx pins in Digital mode
    AD1PCFG = 0xFFFF;

    // Init modules
    ads1292r_init();
    mpu6050_init();
    hc05_init();
}

// Take data from ADS1292R and send to HC-05.
void ads1292r_to_hc05(void) {
    int i;
    ads1292r_cs_low();

    // Status data
    for (i = 0; i < 3; i++) {
        ads1292r_transfer(ADS1292R_CMD_DUMMY); // discard
    }
    // Bioimpedance data
    for (i = 0; i < 3; i++) {
        hc05_put(ads1292r_transfer(ADS1292R_CMD_DUMMY));

    }
    // ECG data
    for (i = 0; i < 3; i++) {
        hc05_put(ads1292r_transfer(ADS1292R_CMD_DUMMY));
    }

    ads1292r_cs_high();
}

// Take data from MPU-6050 and send to HC-05.
void mpu6050_to_hc05(void) {
    int i;

    // Accelerometer data
    for (i = 0; i < 6; i++) {
        hc05_put(mpu6050_rreg(MPU6050_REG_ACCEL_XOUT_H + i));
    }

    // Gyroscope data
    for (i = 0; i < 6; i++) {
        hc05_put(mpu6050_rreg(MPU6050_REG_GYRO_XOUT_H + i));
    }
}
