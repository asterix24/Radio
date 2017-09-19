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

#include "buildrev.h"

#include "radio_cc1101.h"
#include "radio_cfg.h"
#include "protocol.h"
#include "measure.h"

#include "cfg/cfg_i2c.h"

#include "hw/hw_cc1101.h"
#include "hw/hw_adc.h"
#include "hw/hw_pwr.h"

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


#define REPLY_BUF_SIZE (sizeof(Protocol)+sizeof(uint8_t))


static Radio radio;
static Protocol proto;
static uint8_t payload[RADIO_MAXPAYLOAD_LEN];
static uint8_t my_id;
static int tx_len;


static void slave_shutdown(void)
{
	uint32_t wup = rtc_time() + (uint32_t)CMD_SLEEP_TIME;
	LOG_WARN("Bye! wakeup to %lds\n", wup);
	rtc_setAlarm(wup);
	radio_sleep();
	go_standby();
}

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

	kprintf("\n\n::== RADIO LOG SYSTEM ==::\n");
	kprintf("Buildver %d\n", VERS_BUILD);
	kprintf("Module[%d] cfg[0x%x]\n", my_id, cfg);

	measure_init();
	measure_enable(cfg);

	if (my_id == RADIO_MASTER)
	{
		// We are master we trasmit with max power
		// and we wait until a slave write to us.
		radio_timeout(&radio, -1);
		radio_txPwr(&radio, 0xC0);
	}
	else
	{
		timer_delay(500);
		tx_len = protocol_encode(my_id, cfg, &proto, payload, RADIO_MAXPAYLOAD_LEN);
		// We already make sensors measures, so shutdown all
		measure_disable();

		//// No data changes, go sleep.
		//if (!measure_isDataChage(payload, proto.len))
		//{
		//	slave_shutdown();
		//	return;
		//}
		//// Last measure are changed, update checksum.
		//measure_updateRot(payload, proto.len);

		// We are slave to preserve a battery we trasmit with
		// low power and wait only the ack from master before to
		// go sleep.
		radio_txPwr(&radio, 0x50);
		radio_timeout(&radio, 0);
	}

}

int main(void)
{
	init();

	int tx_retry = 0;
	int ret = 0;
	while (1)
	{
		if (my_id == RADIO_MASTER)
		{
			tx_len = protocol_encode(my_id, radio_cfg(my_id), &proto, payload, RADIO_MAXPAYLOAD_LEN);

			memset(payload, 0, RADIO_MAXPAYLOAD_LEN);
			size_t rx_len = kfile_read(&radio.fd, payload, RADIO_MAXPAYLOAD_LEN);
			kfile_error(&radio.fd);
			kfile_clearerr(&radio.fd);
			if(rx_len > 0)
			{
				LOG_INFO("RAddr[%d]l[%d]\n", proto.addr, rx_len);
				uint8_t reply = PROTO_ACK;
				if (protocol_decode(&radio, &proto, payload, RADIO_MAXPAYLOAD_LEN) != 0)
					reply = PROTO_NACK;

				memset(payload, 0, RADIO_MAXPAYLOAD_LEN);
				tx_len = protocol_encodeReply(proto.addr, &proto, &reply,sizeof(uint8_t), \
						payload, RADIO_MAXPAYLOAD_LEN);

				for (int i = 0; i < CMD_MAX_RETRY; i++)
				{
					ret = kfile_write(&radio.fd, payload, tx_len);
					kfile_error(&radio.fd);
					kfile_clearerr(&radio.fd);
					LOG_INFO("Send[%d] ret[%d]\n", proto.addr, ret);
					timer_delay(500);
				}
			}

		}
		else
		{

			Protocol rx_proto;
			uint8_t read_buf[REPLY_BUF_SIZE];
			memset(read_buf, 0, REPLY_BUF_SIZE);
			int rx_len = kfile_read(&radio.fd, read_buf, REPLY_BUF_SIZE);

			kfile_error(&radio.fd);
			kfile_clearerr(&radio.fd);
			radio_timeout(&radio, CMD_RETRY_TIME);

			if (rx_len > 0)
			{
				uint8_t reply = 0;
				protocol_decodeReply(&rx_proto, &reply, sizeof(reply), read_buf, REPLY_BUF_SIZE);
				if(rx_proto.addr != my_id)
				{
					LOG_INFO("Skip pkg\n");
					continue;
				}

				// Slave module, if package is sent correctly goes to sleep
				// otherwise retry.
				if (reply == PROTO_ACK)
					slave_shutdown();
			}

			ret = kfile_write(&radio.fd, payload, tx_len);
			kfile_error(&radio.fd);
			kfile_clearerr(&radio.fd);

			LOG_WARN("Send[%d]\n", ret);

			if (tx_retry == CMD_MAX_RETRY)
				slave_shutdown();

			tx_retry++;
		}
	}

	return 0;
}
