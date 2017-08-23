/**
 * \file
 * <!--
 * This program is free software; you can redistribute it and/or modify
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
 * Copyright 2013 Daniele Basile <asterix@develer.com>
 *
 * -->
 *
 * \brief CC1101 transceiver
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#ifndef RADIO_CC1101_H
#define RADIO_CC1101_H

#include <cfg/macros.h>

#include <io/stm32.h>
#include <io/kfile.h>

#include <cpu/types.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/gpio_stm32.h>
#include <drv/cc1101.h>

/**
 * Radio KFile context structure.
 */
typedef struct Radio
{
	KFile fd; ///< File descriptor.
	mtime_t recv_timeout;
	uint8_t status;
	int error;
	uint8_t lqi;  ///< Link quality descriptor.
	uint8_t rssi; ///< Received Signal Strength Indication.
} Radio;

/**
 * ID for dataflash.
 */
#define KFT_RADIO MAKE_ID('R', 'A', 'D', 'I')


/**
 * Convert + ASSERT from generic KFile to Radio.
 */
INLINE Radio * RADIO_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_RADIO);
	return (Radio *)fd;
}

extern const Setting ping_low_baud_868[];


/*
 * Radio errors
 */
#define RADIO_TX_ERR        -1
#define RADIO_RX_ERR        -2
#define RADIO_RX_TIMEOUT    -3

#define RADIO_MAXPAYLOAD_LEN  64

void radio_sleep(void);
INLINE void radio_timeout(Radio *fd, mtime_t timeout)
{
	fd->recv_timeout = timeout;
}

INLINE void radio_txPwr(Radio *fd, uint8_t pwr)
{
	fd->status = cc1101_write(CC1101_PATABLE, (const uint8_t *)&pwr, sizeof(uint8_t));
}

void radio_init(Radio *fd, const Setting * settings);

#endif /* RADIO_CC1101_H */
