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
 * \brief BSM-RADIO main.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "radio_cc1101.h"
#include "radio_cfg.h"
#include "protocol.h"
#include "measure.h"

#include "cfg/cfg_i2c.h"

#include "hw/hw_cc1101.h"
#include "hw/hw_adc.h"

#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   3
#define LOG_FORMAT  0

#include <cfg/log.h>

#include <io/kfile.h>

#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/adc.h>
#include <drv/spi.h>
#include <drv/rtc.h>

#include <string.h>

static Radio radio;
static Protocol proto;
static uint8_t my_id;

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();
	rtc_init();

	spi_init();
	radio_cfg_init();

	radio_init(&radio, ping_low_baud_868);

	my_id = radio_addr();
	int cfg = radio_cfg(my_id);
	ASSERT(cfg >= 0);

	kprintf("\n\n <::== RADIO LOG SYSTEM ==::>\n\n");
	LOG_INFO("Module [%d] cfg[0x%x]\n", my_id, cfg);

	measure_init();
	measure_enable(cfg);
	protocol_init(&proto);

	if (my_id == RADIO_MASTER)
	{
		// We are master we trasmit with max power
		// and we wait until a slave write to us.
		radio_txPwr(&radio, 0xC0);
		radio_timeout(&radio, -1);
	}
	else
	{
		protocol_encode(my_id, cfg, &radio, &proto);
		// We already make sensors measures, so shutdown all
		measure_disable();

		// No data changes, go sleep.
		if (!protocol_isDataChage(&proto))
		{
			slave_shutdown();
			return;
		}
		// Last measure are changed, update checksum.
		protocol_updateRot(&proto);

		// We are slave to preserve a battery we trasmit with
		// low power and wait only the ack from master before to
		// go sleep.
		radio_txPwr(&radio, 0x03);
		radio_timeout(&radio, 0);
	}

}

int main(void)
{
	init();

	int tx_retry = 0;
	while (1)
	{
		if (my_id == RADIO_MASTER)
		{
			protocol_encode(my_id, radio_cfg(my_id), &radio, &proto);

			memset(&proto, 0, sizeof(Protocol));
			size_t rx_len = kfile_read(&radio.fd, &proto, sizeof(Protocol));
			int ret = kfile_error(&radio.fd);
			if (ret < 0)
			{
				LOG_ERR("Recv Err[%d]\n", ret);
				kfile_clearerr(&radio.fd);
				continue;
			}

			if(rx_len > 0)
			{
				LOG_INFO("Recv[%d]add[%d]\n", rx_len, proto.addr);
				proto.data[0] = PROTO_ACK;
				if (protocol_decode(&radio, &proto) != 0)
				{
					proto.data[0] = PROTO_NACK;
					LOG_ERR("Nack\n");
				}

				for (int i = 0; i < CMD_MAX_RETRY; i++)
				{
					kfile_write(&radio.fd, &proto, sizeof(Protocol));
					ret = kfile_error(&radio.fd);
					kfile_clearerr(&radio.fd);
					LOG_INFO("Send[%d] ret[%d]\n", proto.addr, ret);
					timer_delay(500);
				}
			}

		}
		else
		{
			Protocol rcv_proto;
			memset(&rcv_proto, 0, sizeof(Protocol));
			size_t rx_len = kfile_read(&radio.fd, &rcv_proto, sizeof(Protocol));
			int ret = kfile_error(&radio.fd);
			kfile_clearerr(&radio.fd);
			LOG_ERR("Recv[%d] len[%d]\n", ret, rx_len);
			radio_timeout(&radio, CMD_RETRY_TIME);

			if (rx_len > 0 && ret >= 0)
			{
				LOG_INFO("id[%d]->[%d]\n", rcv_proto.addr, rcv_proto.data[0]);

				if(rcv_proto.addr != my_id)
				{
					LOG_INFO("No for us\n");
					continue;
				}

				// Slave module, if package is sent correctly goes to sleep
				// otherwise retry.
				if (rcv_proto.data[0] == PROTO_ACK)
					slave_shutdown();
			}

			kfile_write(&radio.fd, &proto, sizeof(Protocol));
			ret = kfile_error(&radio.fd);
			kfile_clearerr(&radio.fd);

			LOG_INFO("Send[%d] ret[%d]\n", proto.addr, ret);

			if (tx_retry == CMD_MAX_RETRY)
				slave_shutdown();

			tx_retry++;
		}
	}

	return 0;
}
