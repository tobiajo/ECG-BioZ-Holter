/*
 * File:    ads1292r.h
 * Project: ecgbiozholter.johansson.xyz
 * MCU:     Microchip PIC24FJ64GA002
 * Author:  Tobias Johansson
 * Version: 1.3, 23 Apr 2015
 */

#ifndef ADS1292R_H
#define	ADS1292R_H

//------------------------------------------------------------------------------
// Libraries

#define FOSC 32000000UL
#define FCY  FOSC/2
#include <xc.h> // MPLAB® XC16 C Compiler
#include <GenericTypeDefs.h>
#include <PPS.h>
#include <libpic30.h>

//------------------------------------------------------------------------------
// Constants

// Commands
#define ADS1292R_CMD_DUMMY      0x00 // Dummy for SPI read
#define ADS1292R_CMD_WAKEUP     0x02 // Wake-up from standby mode
#define ADS1292R_CMD_STANDBY    0x04 // Enter standby mode
#define ADS1292R_CMD_RESET      0x06 // Reset the device
#define ADS1292R_CMD_START      0x08 // Start or restart (sync) conversions
#define ADS1292R_CMD_STOP       0x0A // Stop conversion
#define ADS1292R_CMD_OFFSETCAL  0x1A // Channel offset calibration
#define ADS1292R_CMD_RDATAC     0x10 // Enable Read Data Continuous mode
#define ADS1292R_CMD_SDATAC     0x11 // Stop Read Data Continuously mode
#define ADS1292R_CMD_RDATA      0x12 // Read data by command
#define ADS1292R_CMD_RREG       0x20 // Read  reg at address r rrrr (001r rrr)
#define ADS1292R_CMD_WREG       0x40 // Write reg at address r rrrr (010r rrr)

// Registers
#define ADS1292R_REG_ID         0x00 // ID Control Register (Factory-Programmed)
#define ADS1292R_REG_CONFIG1    0x01 // Configuration Register 1
#define ADS1292R_REG_CONFIG2    0x02 // Configuration Register 2
#define ADS1292R_REG_LOFF       0x03 // Lead-Off Control Register
#define ADS1292R_REG_CH1SET     0x04 // Channel 1 Settings
#define ADS1292R_REG_CH2SET     0x05 // Channel 2 Settings
#define ADS1292R_REG_RLD_SENS   0x06 // Right Leg Drive Sense Selection
#define ADS1292R_REG_LOFF_SENS  0x07 // Lead-Off Sense Selection
#define ADS1292R_REG_LOFF_STAT  0x08 // Lead-Off Status
#define ADS1292R_REG_RESP1      0x09 // Respiration Control Register 1
#define ADS1292R_REG_RESP2      0x0A // Respiration Control Register 2
#define ADS1292R_REG_GPIO       0x0B // General-Purpose I/O Register

//------------------------------------------------------------------------------
// Prototypes

void  ads1292r_init    (void);
void  ads1292r_config  (void);
UINT8 ads1292r_transfer(UINT8);
void  ads1292r_cs_low  (void);
void  ads1292r_cs_high (void);
void  ads1292r_command (UINT8);
UINT8 ads1292r_rreg    (UINT8);
void  ads1292r_wreg    (UINT8, UINT8);

#endif	/* ADS1292R_H */
