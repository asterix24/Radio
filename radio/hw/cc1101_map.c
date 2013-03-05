/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Custom settings for CC1101.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cc1101_map.h"

#include "cfg/cfg_cc1101.h"

#include <drv/cc1101.h>


// Product = CC1101
// Chip version = A   (VERSION = 0x04)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 541.666667 kHz
// Deviation = 127 kHz
// Datarate = 249.938965 kBaud
// Modulation = (1) GFSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 867.999939 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = Sensitivity
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (41) CHIP_RDY
const Setting ping_low_baud_868[] = {
	{ CC1101_FSCTRL1  ,  0x0C   /* Frequency synthesizer control. */},
	{ CC1101_FSCTRL0  ,  0x00   /* Frequency synthesizer control. */},
	{ CC1101_FREQ2    ,  0x21   /* Frequency control word, high byte. */},
	{ CC1101_FREQ1    ,  0x62   /* Frequency control word, middle byte. */},
	{ CC1101_FREQ0    ,  0x76   /* Frequency control word, low byte. */},
	{ CC1101_MDMCFG4  ,  0x2D   /* Modem configuration. */},
	{ CC1101_MDMCFG3  ,  0x3B   /* Modem configuration. */},
	{ CC1101_MDMCFG2  ,  0x13   /* Modem configuration. */},
	{ CC1101_MDMCFG1  ,  0x22   /* Modem configuration. */},
	{ CC1101_MDMCFG0  ,  0xF8   /* Modem configuration. */},
	{ CC1101_CHANNR   ,  0x00   /* Channel number. */},
	{ CC1101_DEVIATN  ,  0x62   /* Modem deviation setting (when FSK modulation is enabled). */},
	{ CC1101_FREND1   ,  0xB6   /* Front end RX configuration. */},
	{ CC1101_FREND0   ,  0x10   /* Front end TX configuration. */},
	{ CC1101_MCSM0    ,  0x18   /* Main Radio Control State Machine configuration. */},
	{ CC1101_FOCCFG   ,  0x1D   /* Frequency Offset Compensation Configuration. */},
	{ CC1101_BSCFG    ,  0x1C   /* Bit synchronization Configuration. */},
	{ CC1101_AGCCTRL2 ,  0xC7   /* AGC control. */},
	{ CC1101_AGCCTRL1 ,  0x00   /* AGC control. */},
	{ CC1101_AGCCTRL0 ,  0xB0   /* AGC control. */},
	{ CC1101_FSCAL3   ,  0xEA   /* Frequency synthesizer calibration. */},
	{ CC1101_FSCAL2   ,  0x2A   /* Frequency synthesizer calibration. */},
	{ CC1101_FSCAL1   ,  0x00   /* Frequency synthesizer calibration. */},
	{ CC1101_FSCAL0   ,  0x1F   /* Frequency synthesizer calibration. */},
	{ CC1101_FSTEST   ,  0x59   /* Frequency synthesizer calibration. */},
	{ CC1101_TEST2    ,  0x88   /* Various test settings. */},
	{ CC1101_TEST1    ,  0x31   /* Various test settings. */},
	{ CC1101_TEST0    ,  0x09   /* Various test settings. */},
	{ CC1101_FIFOTHR  ,  0x07   /* RXFIFO and TXFIFO thresholds. */},
	{ CC1101_IOCFG2   ,  0x29   /* GDO2 output pin configuration. */},
	{ CC1101_IOCFG0   ,  0x06   /* GDO0 output pin configuration. */},
	{ CC1101_PKTCTRL1 ,  0x04   /* Packet automation control. */},
	{ CC1101_PKTCTRL0 ,  0x05   /* Packet automation control. */},
	{ CC1101_ADDR     ,  0x00   /* Device address. */},
	{ CC1101_PKTLEN   ,  0xFF   /* Packet length. */},
	{ CC1101_PATABLE   ,  0xC0   /* Various Test Settings */ },

    { 0xff, 0xff },
};


#if 0
const Setting ping_low_baud_868[] =
{
  {CC1101_IOCFG0,      0x01 /* GDO0 Output Pin Configuration */},
  {CC1101_FIFOTHR,     0x47 /* RX FIFO and TX FIFO Thresholds */},
  {CC1101_PKTCTRL0,    0x05 /* Packet Automation Control */},
  {CC1101_PKTCTRL1,    0x00 /* Packet Automation Control */},
  {CC1101_CHANNR,      0x08 /* Channel Number */},

  {CC1101_PATABLE,     0xC0  /* Various Test Settings */ },

#if CONFIG_RADIO_CURRENT_OPT
  {CC1101_FSCTRL1,     0x06 /* Frequency Synthesizer Control */},
  {CC1101_FREQ2,       0x21 /* Frequency Control Word, High Byte */},
  {CC1101_FREQ1,       0x62 /* Frequency Control Word, Middle Byte */},
  {CC1101_FREQ0,       0x76 /* Frequency Control Word, Low Byte */},
  {CC1101_MDMCFG4,     0xF5 /* Modem Configuration */},
  {CC1101_MDMCFG3,     0x83 /* Modem Configuration */},
  {CC1101_MDMCFG2,     0x93 /* Modem Configuration */},
  {CC1101_DEVIATN,     0x15 /* Modem Deviation Setting */},
  {CC1101_MCSM0,       0x18 /* Main Radio Control State Machine Configuration */},
  {CC1101_FOCCFG,      0x16 /* Frequency Offset Compensation Configuration */},
  {CC1101_WORCTRL,     0xFB /* Wake On Radio Control */},
  {CC1101_FSCAL3,      0xE9 /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL2,      0x2A /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL1,      0x00 /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL0,      0x1F /* Frequency Synthesizer Calibration */},
  {CC1101_TEST2,       0x81 /* Various Test Settings */},
  {CC1101_TEST1,       0x35 /* Various Test Settings */},

#else

  {CC1101_FSCTRL1,     0x06 /* Frequency Synthesizer Control */},
  {CC1101_FREQ2,       0x21 /* Frequency Control Word, High Byte */},
  {CC1101_FREQ1,       0x62 /* Frequency Control Word, Middle Byte */},
  {CC1101_FREQ0,       0x76 /* Frequency Control Word, Low Byte */},
  {CC1101_MDMCFG4,     0xF5 /* Modem Configuration */},
  {CC1101_MDMCFG3,     0x83 /* Modem Configuration */},
  {CC1101_MDMCFG2,     0x13 /* Modem Configuration */},
  {CC1101_DEVIATN,     0x15 /* Modem Deviation Setting */},
  {CC1101_MCSM0,       0x18 /* Main Radio Control State Machine Configuration */},
  {CC1101_FOCCFG,      0x16 /* Frequency Offset Compensation Configuration */},
  {CC1101_WORCTRL,     0xFB /* Wake On Radio Control */},
  {CC1101_FSCAL3,      0xE9 /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL2,      0x2A /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL1,      0x00 /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL0,      0x1F /* Frequency Synthesizer Calibration */},
  {CC1101_TEST2,       0x81 /* Various Test Settings */},
  {CC1101_TEST1,       0x35 /* Various Test Settings */},
  {CC1101_TEST0,       0x09 /* Various Test Settings */},

#endif

  { 0xff, 0xff },
};
#endif
