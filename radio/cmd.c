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

#include "protocol.h"
#include "iwdg.h"
#include "radio_cfg.h"

#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   3
#define LOG_FORMAT  0

#include <cfg/log.h>

#include <drv/timer.h>
#include <drv/rtc.h>

#include <string.h>

static int cmd_master_broadcast(KFile *_fd, Protocol *proto)
{

	LOG_INFO("Broadcast: type[%d]addr[%d]\n",
								proto->type, proto->addr);

	Radio *fd = RADIO_CAST(_fd);
	protocol_decode(fd, proto);

	return protocol_sendByte(_fd, proto, proto->addr, proto->type, PROTO_ACK);
}


static int cmd_master_sleep(KFile *fd, Protocol *proto)
{
	(void)fd;
	(void)proto;
	LOG_INFO("Sleep: type[%d]addr[%d]\n",
							proto->type, proto->addr);
	return 0;
}

const Cmd master_cmd[] = {
	{ CMD_BROADCAST, cmd_master_broadcast },
	{ CMD_SLEEP,     cmd_master_sleep     },
	{ 0   , NULL }
};



static uint8_t slave_status;
static uint8_t retry;
static uint32_t start_time;

static void slave_shutdown(void)
{
	rtc_setAlarm(rtc_time() + (uint32_t)CMD_WAKEUP_TIME);
	LOG_INFO("Go Standby, wakeup to %lds\n", rtc_time() + CMD_WAKEUP_TIME);
	go_standby();
}

static int cmd_slave_sleep(KFile *fd, Protocol *proto)
{
	LOG_INFO("Sleep: type[%d]addr[%d]\n",
							proto->type, proto->addr);

	protocol_sendByte(fd, proto, proto->addr, proto->type, PROTO_ACK);
	slave_shutdown();
	return 0;
}

static int cmd_slave_broadcast(KFile *fd, Protocol *proto)
{
	(void)fd;
	if (proto->data[0] == PROTO_ACK)
	{
		slave_status = CMD_SLAVE_STATUS_SHUTDOWN;
		LOG_INFO("Broadcast ACK\n");
	}
	else
		LOG_INFO("Broadcast NACK\n");

	return 0;
}


const Cmd slave_cmd[] = {
	{ CMD_BROADCAST, cmd_slave_broadcast },
	{ CMD_SLEEP,     cmd_slave_sleep      },
	{ 0     , NULL }
};

static void cmd_slavePoll(KFile *fd, Protocol *proto)
{
	if (slave_status == CMD_SLAVE_STATUS_WAIT)
	{
		LOG_INFO("wait..(%ld)\n", rtc_time());
		return;
	}

	if (retry == CMD_MAX_RETRY)
		slave_status = CMD_SLAVE_STATUS_SHUTDOWN;

	if (slave_status == CMD_SLAVE_STATUS_SHUTDOWN)
	{
		if ((rtc_time() - start_time) > CMD_TIMEOUT)
			slave_shutdown();

		return;
	}

	memset(proto, 0, sizeof(Protocol));
	protocol_encode(proto);
	int sent = protocol_send(fd, proto, radio_cfg_id(), CMD_BROADCAST);

	LOG_INFO("Broadcast sent[%d] %s[%d]\n",
				proto->type, sent < 0 ? "Error!":"Ok", sent);

	retry += 1;
}

void cmd_poll(uint8_t id, KFile *fd, struct Protocol *proto)
{
	if (id == RADIO_MASTER)
	{
		if ((rtc_time() - start_time) > (CMD_TIMEOUT + CMD_WAKEUP_TIME))
		{
			memset(proto, 0, sizeof(Protocol));
			kprintf("$0;0;0;%ld;", rtc_time());
			protocol_encode(proto);
			start_time = rtc_time();
		}
	}
	else
	{
		cmd_slavePoll(fd, proto);
	}
}

void cmd_init()
{
	start_time = rtc_time();
	slave_status = CMD_SLAVE_STATUS_BROADCAST;
}


