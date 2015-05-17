/*
 * File:    main.h
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#ifndef MAIN_H
#define	MAIN_H

//------------------------------------------------------------------------------
// Pragma

#pragma config FNOSC  = FRCPLL // Fast RC Oscillator with PLL module (FRCPLL)
#pragma config JTAGEN = OFF    // JTAG port is disabled
#pragma config FWDTEN = OFF    // Watchdog Timer is disabled

//------------------------------------------------------------------------------
// Libraries

#define FOSC 32000000UL
#define FCY  FOSC/2
#include <xc.h> // MPLAB® XC16 C Compiler
#include <GenericTypeDefs.h>
#include "ads1292r.h"
#include "mpu6050.h"
#include "hc05.h"

//------------------------------------------------------------------------------
// Prototypes

void init            (void);
void ads1292r_to_hc05(void);
void mpu6050_to_hc05 (void);

#endif	/* MAIN_H */
