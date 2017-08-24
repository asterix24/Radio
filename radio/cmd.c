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
 * \brief Radio commands.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "cmd.h"
#include "protocol.h"
#include "radio_cfg.h"
#include "measure.h"
#include "radio_cc1101.h"

#include "hw/hw_pwr.h"

#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   3
#define LOG_FORMAT  0

#include <cfg/log.h>

#include <drv/timer.h>
#include <drv/rtc.h>

#include <string.h>


static int cmd_addr;
static uint8_t tx_retry = 0;


static void slave_shutdown(void)
{
	uint32_t wup = rtc_time() + (uint32_t)CMD_SLEEP_TIME;
	LOG_INFO("Go Standby, wakeup to %lds\n", wup);
	rtc_setAlarm(wup);
	radio_sleep();
	go_standby();
}

static int cmd_broadcast(KFile *_fd, Protocol *proto)
{
	Radio *fd = RADIO_CAST(_fd);
	int ret;
	if (cmd_addr == RADIO_MASTER)
	{
		ret = protocol_sendByte(_fd, proto, proto->addr, proto->type, PROTO_ACK);
		protocol_decode(fd, proto);
	}
	else
	{
		ret = (proto->data[0] == PROTO_ACK);
		if (ret)
			slave_shutdown();
	}

	LOG_INFO("Broadcast: addr[%d] ret[%d]\n", proto->addr, ret);
	return 0;
}

static const ProtoCmd cmd_table[] = {
	{ CMD_BROADCAST, cmd_broadcast  },
	{ 0,             NULL           }
};

void cmd_poll(KFile *_fd, struct Protocol *proto)
{
	// Get message from air..
	protocol_poll(_fd, proto);

	Radio *fd = RADIO_CAST(_fd);

	if (cmd_addr == RADIO_DEBUG)
	{
		protocol_encode(fd, proto);
		radio_timeout(fd, 1000);
		return;
	}

	if (cmd_addr == RADIO_MASTER)
	{
		protocol_encode(fd, proto);
		radio_timeout(fd, -1);
		return;
	}


	if (tx_retry == 0)
	{
		protocol_encode(fd, proto);
		if (!protocol_isDataChage(proto))
			slave_shutdown();

	}
	else
	{
		protocol_updateRot(proto);
		int sent = protocol_send(_fd, proto, cmd_addr, CMD_BROADCAST);
		radio_timeout(fd, CMD_RETRY_TIME);
		LOG_INFO("Sent[%d] %s[%d]\n", \
				proto->type, sent < 0 ? "Error!":"Ok", sent);
	}

	if (tx_retry == CMD_MAX_RETRY)
		slave_shutdown();

	tx_retry++;
}

void cmd_init(uint8_t addr)
{
	cmd_addr = addr;
	protocol_init(addr, cmd_table);
}

