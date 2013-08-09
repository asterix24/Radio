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

#include "radio_cc1101.h"

#include "hw/hw_cc1101.h"

#include "cfg/cfg_cc1101.h"

// Define log settings for cfg/log.h.
#define LOG_LEVEL         CC1101_LOG_LEVEL
#define LOG_FORMAT        CC1101_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/debug.h>

#include <drv/cc1101.h>
#include <drv/timer.h>

#include <string.h>

#define CC1101_WAIT_IDLE_TIMEOUT   5000 // ms

const Setting ping_low_baud_868[] =
{
  {CC1101_IOCFG0,      0x01 /* GDO0 Output Pin Configuration */},
  {CC1101_FIFOTHR,     0x47 /* RX FIFO and TX FIFO Thresholds */},
  {CC1101_PKTCTRL0,    0x05 /* Packet Automation Control */},
  {CC1101_PKTCTRL1,    0x00 /* Packet Automation Control */},
  {CC1101_CHANNR,      0x08 /* Channel Number */},

  {CC1101_FSCTRL1,     0x06 /* Frequency Synthesizer Control */},
  {CC1101_FREQ0,       0x76 /* Frequency Control Word, Low Byte */},
  {CC1101_FREQ1,       0x62 /* Frequency Control Word, Middle Byte */},
  {CC1101_FREQ2,       0x21 /* Frequency Control Word, High Byte */},

  {CC1101_FSCAL3,      0xE9 /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL2,      0x2A /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL1,      0x00 /* Frequency Synthesizer Calibration */},
  {CC1101_FSCAL0,      0x1F /* Frequency Synthesizer Calibration */},
  {CC1101_FOCCFG,      0x16 /* Frequency Offset Compensation Configuration */},

  {CC1101_DEVIATN,     0x15 /* Modem Deviation Setting */},

  {CC1101_MCSM0,       0x18 /* Main Radio Control State Machine Configuration */},
  {CC1101_WORCTRL,     0xFB /* Wake On Radio Control */},

  {CC1101_TEST2,       0x81 /* Various Test Settings */},
  {CC1101_TEST1,       0x35 /* Various Test Settings */},

  {CC1101_PATABLE,     0xC0  /* Various Test Settings */ },

  {CC1101_MDMCFG1,     0x42 /* Modem Configuration */},
#if CONFIG_RADIO_CURRENT_OPT
  {CC1101_MDMCFG2,     0x93 /* Modem Configuration */},
#else
  {CC1101_MDMCFG2,     0x13 /* Modem Configuration */},
  {CC1101_TEST0,       0x09 /* Various Test Settings */},
#endif
  {CC1101_MDMCFG3,     0x83 /* Modem Configuration */},
  {CC1101_MDMCFG4,     0xF5 /* Modem Configuration */},

  { 0xff, 0xff },
};



static bool wait_fifoAvail(mtime_t timeout)
{
	uint8_t status;
	ticks_t start = timer_clock();
	do
	{
		status = cc1101_strobe(CC1101_SNOP);
		LOG_INFO("Idle: Rdy[%d] St[%d] FifoAvail[%d]\n", UNPACK_STATUS(status));

		if (timeout == -1)
		{
			cpu_relax();
		}
		else
		{
			if (timer_clock() - start > ms_to_ticks(timeout))
				return false;
		}

	} while (!stm32_gpioPinRead(GPIO_BASE_A, BV(11)));

	return true;
}


static bool radio_waitIdle(mtime_t timeout)
{
	uint8_t status;
	ticks_t start = timer_clock();
	do
	{
		status = cc1101_strobe(CC1101_SNOP);
		LOG_INFO("Idle: Rdy[%d] St[%d] FifoAvail[%d]\n", UNPACK_STATUS(status));

		if (timeout == -1)
		{
			cpu_relax();
		}
		else
		{
			if (timer_clock() - start > ms_to_ticks(timeout))
				return false;
		}

	} while (STATUS_STATE(status) != CC1101_STATUS_IDLE);

	return true;
}

static void radio_goIdle(void)
{
	// Put manually to IDLE state.
	uint8_t status = cc1101_strobe(CC1101_SIDLE);
	LOG_INFO("GoIdle: Rdy[%d] St[%d] FifoAvail[%d]\n", UNPACK_STATUS(status));
}


INLINE uint8_t radio_status(void)
{
	uint8_t status;
	//Flush the data in the fifo
	status = cc1101_strobe(CC1101_SNOP);
	LOG_INFO("Status: Rdy[%d] St[%d] FifoAvail[%d]\n", UNPACK_STATUS(status));

	return status;
}

INLINE int radio_lqi(void)
{
	uint8_t lqi = 0;
	cc1101_read(CC1101_LQI, &lqi, 1);

	return lqi;
}

INLINE int radio_rssi(void)
{
	uint8_t rssi = 0;
	cc1101_read(CC1101_RSSI, &rssi, 1);

	return cc1101_rssidBm(rssi, 74);
}

void radio_sleep(void)
{
	LOG_INFO("Sleep: Rdy[%d] St[%d] FifoAvail[%d]\n", UNPACK_STATUS(cc1101_strobe(CC1101_SPWD)));
}

static int radio_error(struct KFile *_fd)
{
	Radio *fd = RADIO_CAST(_fd);
	return fd->error;
}

static void radio_clearerr(struct KFile *_fd)
{
	Radio *fd = RADIO_CAST(_fd);
	fd->error = 0;
}

static uint8_t tmp_buf[65];
static size_t radio_send(struct KFile *_fd, const void *_buf, size_t size)
{
	/* The packet len is now fix to max 64 byte (see configuation)*/
	ASSERT(sizeof(tmp_buf) >= size);

	const uint8_t *data = (const uint8_t *) _buf;
	Radio *fd = RADIO_CAST(_fd);

	radio_waitIdle(-1);
	cc1101_strobe(CC1101_SFTX);
	uint8_t status = cc1101_strobe(CC1101_STX);

	memset(tmp_buf, 0, sizeof(tmp_buf));
	// We reserve one byte for package len
	size_t tx_len = MIN(sizeof(tmp_buf) - 1, size + 1);

	tmp_buf[0] = tx_len - 1;
	memcpy(&tmp_buf[1], data, tx_len);
	fd->status = cc1101_write(CC1101_TXFIFO, tmp_buf, tx_len);
	LOG_INFO("TxData[%d] s[%d]\n", tmp_buf[0], STATUS_STATE(fd->status));

	radio_waitIdle(-1);
	status = cc1101_strobe(CC1101_SFTX);
	LOG_INFO("Tx: s1[%d]\n", STATUS_STATE(status));

	if (STATUS_STATE(fd->status) == CC1101_STATUS_TX_FIFOUNFLOW)
	{
		fd->error = RADIO_TX_ERR;
		return RADIO_TX_ERR;
	}

	return tx_len;
}

static size_t radio_recv(struct KFile *_fd, void *buf, size_t size)
{
	uint8_t *data = (uint8_t *)buf;
	Radio *fd = RADIO_CAST(_fd);

	radio_waitIdle(-1);
	cc1101_strobe(CC1101_SFRX);
	uint8_t status = cc1101_strobe(CC1101_SRX);
	LOG_INFO("RxData: Rdy[%d] St[%d] FifoAvail[%d]\n", UNPACK_STATUS(status));

	// Waiting data from air..
	if (!wait_fifoAvail(fd->recv_timeout))
	{
		radio_goIdle();
		cc1101_strobe(CC1101_SFRX);
		LOG_ERR("Rx timeout..\n");
		fd->error = RADIO_RX_TIMEOUT;
		return RADIO_RX_TIMEOUT;
	}

	uint8_t rx_data;
	cc1101_read(CC1101_RXFIFO, &rx_data, 1);

	size_t rx_len = MIN((size_t)rx_data, size);
	status = cc1101_read(CC1101_RXFIFO, data, rx_len);
	LOG_INFO("RxLen[%d] [%d] s[%d]\n", rx_len, size, STATUS_STATE(status));

	radio_waitIdle(-1);
	fd->lqi = radio_lqi();
	fd->rssi = radio_rssi();
	fd->status = cc1101_strobe(CC1101_SFRX);
	LOG_INFO("Rx: s1[%d], lqi[%d] rssi[%d]\n", STATUS_STATE(fd->status), fd->lqi & ~CC1101_LQI_CRC_OK, fd->rssi);

	/* check if fifo data is valid packet */
	if (fd->lqi & CC1101_LQI_CRC_OK)
	{
		fd->lqi &= ~CC1101_LQI_CRC_OK;
		return rx_len;
	}

	fd->lqi = 0;
	fd->error = RADIO_RX_ERR;
	return RADIO_RX_ERR;
}

void radio_init(Radio *fd, const Setting * settings)
{
	ASSERT(fd);
	ASSERT(settings);

	cc1101_init(settings);

	 //Set kfile struct type as a generic kfile structure.
	DB(fd->fd._type = KFT_RADIO);

	// Set up data flash programming functions.
	fd->fd.read = radio_recv;
	fd->fd.write = radio_send;
	fd->fd.error = radio_error;
	fd->fd.clearerr = radio_clearerr;
	fd->error = 0;
}
