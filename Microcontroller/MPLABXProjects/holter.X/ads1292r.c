/*
 * File:    ads1292r.c
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#include "ads1292r.h"

#define RIGHT_LEG_DRIVE 1 // set 1 for enable RLD, otherwise disabled

// Initialization.
void ads1292r_init(void) {
    // Pin directions
    TRISBbits.TRISB14 = 0; // ADS_CLKSEL = out
    TRISBbits.TRISB13 = 0; // ADS_RESET  = out
    TRISBbits.TRISB12 = 0; // ADS_START  = out
    TRISBbits.TRISB11 = 0; // ADS_CS     = out
    TRISBbits.TRISB7  = 1; // ADS_DRDY   = in

    // Power-Up Settings
    LATBbits.LATB14 = 1; // internal clock
    LATBbits.LATB13 = 1; // no: reset
    LATBbits.LATB12 = 1; // start
    LATBbits.LATB11 = 1; // no: chip select

    // PPS SPI2
    PPSInput (PPS_SDI2, PPS_RP8);     // ADS_DOUT
    PPSOutput(PPS_RP9,  PPS_SCK2OUT); // ADS_SCLK
    PPSOutput(PPS_RP10, PPS_SDO2);    // ADS_DIN

    // SPI2 init
    // "If SCLK is 1 MHz, one byte is transferred in 8 µs. Because this transfer
    // time exceeds the t_SDECODE specification, the processor can send
    // subsequent bytes without delay." - Data sheet p. 37
    SPI2CON1 = 0x003D; // Master Mode, SPI clock = 1000 kHz (Fcy = 16 MHz)
    SPI2STAT = 0x8000; // Enables module

    // Delay for Power-On Reset and Oscillator Start-Up
    __delay_ms(1000);

    // "The internal oscillator generates a 512-kHz clock." - Data sheet p. 16
    // "f_MOD = f_CLK / 4 (default, use when f_CLK = 512 kHz)" - Data sheet. 47
    // => t_MOD = 7.8125 us, t_CLK = 1.953125 us

    // Issue Reset Pulse then Wait for 18 t_CLKs
    LATBbits.LATB13 = 0;
    __delay_us(8);  // Reset low width min = 1 t_MOD
    LATBbits.LATB13 = 1;
    __delay_us(36); // Recommended wait after reset = 18 t_CLK
	
    // "After releasing RESET, the configuration register must be programmed..."
    // - Data sheet p. 62

    // Device Wakes Up in RDATAC Mode, so Send
    // SDATAC Command so Registers can be Written
    ads1292r_command(ADS1292R_CMD_SDATAC);

    ads1292r_config();

    // Activate Conversion
    LATBbits.LATB12 = 1; // START = 1

    // Put the Device Back in RDATAC Mode
    ads1292r_command(ADS1292R_CMD_RDATAC);
}

// Configuration settings.
void ads1292r_config(void) {
    // Continuous conversion, Data Rate = 1 kSPS
    ads1292r_wreg(ADS1292R_REG_CONFIG1, 0x03);

    // Reference buffer is enabled
    ads1292r_wreg(ADS1292R_REG_CONFIG2, 0xA0);

    // Channel 1 PGA gain = 4, Normal electrode input
    ads1292r_wreg(ADS1292R_REG_CH1SET, 0x40);

    // Channel 2 PGA gain = 4, Normal electrode input
    ads1292r_wreg(ADS1292R_REG_CH2SET, 0x40);

    // Chop frequency = f_MOD/16, RLD buffer is enabled if RIGHT_LEG_DRIVE == 1
    #if RIGHT_LEG_DRIVE == 1
    #define RLD_BIT 0x20
    #else
    #define RLD_BIT 0x00
    #endif
    ads1292r_wreg(ADS1292R_REG_RLD_SENS, 0x00 | RLD_BIT);

    // CLK_DIV bit of LOFF_STAT register = 0 by default

    // Enables respiration circuits, Respiration phase = 112.5°
    // "It can be noted that gain = 4 and phase = 112.5° yield the best
    // performance..." - Data sheet p. 59
    ads1292r_wreg(ADS1292R_REG_RESP1, 0xEA);

    // Resp frequency, 32 kHz, RLDREF signal (AVDD-AVSS)/2 generated internally
    ads1292r_wreg(ADS1292R_REG_RESP2, 0x03);
}

//------------------------------------------------------------------------------
// Basic Functions

// Transfer data bidirectional.
UINT8 ads1292r_transfer(UINT8 data) {
    SPI2BUF = data;               // write to buffer
    while (!SPI2STATbits.SPIRBF); // wait transfer
    return SPI2BUF;               // read received
}

// Set chip select low.
void ads1292r_cs_low(void) {
    LATBbits.LATB11 = 0;
}

// Set chip select high.
void ads1292r_cs_high(void) {
    // "Always wait four or more t_CLK cycles before taking CS high."
    // - Data sheet p. 28
    __delay_us(8);
    LATBbits.LATB11 = 1;
}

//------------------------------------------------------------------------------
// Putting it Together

// Sends a command.
void ads1292r_command(UINT8 cmd) {
    ads1292r_cs_low();
    ads1292r_transfer(cmd);
    ads1292r_cs_high();
}

// Reads from a register.
UINT8 ads1292r_rreg(UINT8 reg) {
    UINT8 value;
    ads1292r_cs_low();

    ads1292r_transfer(ADS1292R_CMD_RREG | reg);
    ads1292r_transfer(0x00); // [number of registers to be read]-1 = 0x00
    value = ads1292r_transfer(ADS1292R_CMD_DUMMY);

    ads1292r_cs_high();
    return value;
}

// Writes to a register.
void ads1292r_wreg(UINT8 reg, UINT8 value) {
    ads1292r_cs_low();

    ads1292r_transfer(ADS1292R_CMD_WREG | reg);
    ads1292r_transfer(0x00); // [number of registers to be written]-1 = 0x00
    ads1292r_transfer(value);

    ads1292r_cs_high();
}
