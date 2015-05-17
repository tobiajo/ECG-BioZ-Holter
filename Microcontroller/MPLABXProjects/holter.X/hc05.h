/*
 * File:    hc05.h
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#ifndef HC05_H
#define	HC05_H

//------------------------------------------------------------------------------
// Libraries

#define FOSC 32000000UL
#define FCY  FOSC/2
#include <xc.h> // MPLAB® XC16 C Compiler
#include <GenericTypeDefs.h>
#include <PPS.h>

//------------------------------------------------------------------------------
// Prototypes

void  hc05_init(void);
UINT8 hc05_get (void);
void  hc05_put (UINT8);

#endif	/* HC05_H */
