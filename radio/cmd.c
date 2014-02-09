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
#include "radio_cfg.h"

#include "hw/hw_pwr.h"

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
static int elapse_time;

static void slave_shutdown(void)
{
	uint32_t wup = rtc_time() + (uint32_t)CMD_TIME_TO_WAKEUP;
	LOG_INFO("Go Standby, wakeup to %lds\n", wup);
	rtc_setAlarm(wup);
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
		LOG_INFO("Status Shutdown\n");
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

static uint16_t resend_time[] = { CMD_TIME_TO_STANDBY - 1, CMD_TIME_TO_STANDBY / 2, CMD_TIME_TO_STANDBY / 3 };

static void cmd_slavePoll(KFile *_fd, Protocol *proto)
{
	int time = elapse_time - rtc_time();

	// Time elapsed, we shutdown
	if ((time < 0) && (slave_status == CMD_SLAVE_STATUS_SHUTDOWN))
	{
		slave_shutdown();
		return;
	}

	if ((time <= resend_time[retry]) &&
				(slave_status == CMD_SLAVE_STATUS_BROADCAST))
	{

		Radio *fd = RADIO_CAST(_fd);
		protocol_encode(fd, proto);
		int sent = protocol_send(_fd, proto, radio_cfg_id(), CMD_BROADCAST);

		LOG_INFO("Broadcast sent[%d] %s[%d] time[%d]\n",
					proto->type, sent < 0 ? "Error!":"Ok", sent, resend_time[retry]);

		retry += 1;
		if (retry == CMD_MAX_RETRY)
		{
			slave_status = CMD_SLAVE_STATUS_SHUTDOWN;
			LOG_INFO("Max retry Shutdown\n");
		}
	}
}

void cmd_poll(uint8_t id, KFile *_fd, struct Protocol *proto)
{
	if (id == RADIO_MASTER)
	{
		int time = elapse_time - rtc_time();
		if (time < 0)
		{
			Radio *fd = RADIO_CAST(_fd);
			protocol_encode(fd, proto);
			elapse_time = rtc_time() + CMD_TIME_TO_STANDBY + CMD_TIME_TO_WAKEUP;
		}
	}
	else
	{
		cmd_slavePoll(_fd, proto);
	}
}

void cmd_init()
{
	elapse_time = rtc_time() + CMD_TIME_TO_STANDBY;
	slave_status = CMD_SLAVE_STATUS_BROADCAST;
}

