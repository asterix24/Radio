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
 * \brief CC1101 transceiver
 *
 * \author Daniele Basile <asterix@develer.com>
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

void radio_init(Radio *fd, const Setting * settings);

#endif /* RADIO_CC1101_H */
