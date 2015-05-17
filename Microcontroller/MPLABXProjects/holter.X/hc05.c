/*
 * File:    hc05.c
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#include "hc05.h"

// Initialization.
void hc05_init(void) {
    // PPS UART2
    PPSInput (PPS_U2RX, PPS_RP6);  // U2RX = RP6
    PPSOutput(PPS_RP15, PPS_U2TX); // U2TX = RP15

    // UART2 init
    U2BRG  = 34;     // Baud Rate = 115200 (Fcy = 16 MHz)
    U2MODE = 0x8008; // UART is enabled, BRGH = 1
    U2STA  = 0x0400; // Transmit enabled
}

// Get data from UART.
UINT8 hc05_get(void) {
    while (U2STAbits.URXDA == 0); // wait
    return U2RXREG;
}

// Put data to UART.
void hc05_put(UINT8 data) {
    while (U2STAbits.UTXBF == 1); // wait while TX buffer is full
    U2TXREG = data;
}
