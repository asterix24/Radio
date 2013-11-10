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
 * Copyright 2013 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Radio commands.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "protocol.h"

#include <cfg/debug.h>

#include <drv/timer.h>

#include <string.h>

static Devices local_dev[CMD_DEVICES];

static int cmd_master_broadcast(KFile *fd, Protocol *proto)
{
	kprintf("Broadcast: ");
	uint8_t reply = PROTO_ACK;
	for (int i = 0; i < CMD_DEVICES; i++)
	{
		if (local_dev[i].addr == proto->addr)
		{
			break;
		}
		else if (!local_dev[i].addr)
		{
			local_dev[i].addr =  proto->addr;
			local_dev[i].status = CMD_NEW_DEV;
			local_dev[i].timeout = timer_clock();
			break;
		}
		else
		{
			reply = PROTO_NACK;
		}
	}

	//proto->data[proto->len] = '\0';
	kprintf("type[%d], addr[%d], data[%s]->", proto->type, proto->addr, proto->data);
	kprintf("reply[%d]\n", reply);

	return protocol_send(fd, proto, proto->addr, proto->type, &reply, sizeof(reply));
}

static int cmd_master_data(KFile *_fd, Protocol *proto)
{
	kprintf("Data\n");
	Radio *fd = RADIO_CAST(_fd);
	protocol_decode(fd, proto);
	return 0;
}

const Cmd master_cmd[] = {
	{ CMD_BROADCAST, cmd_master_broadcast },
	{ CMD_DATA,      cmd_master_data      },
	{ 0   , NULL }
};

static int cmd_slave_broadcast(KFile *fd, Protocol *proto)
{
	(void)fd;
	kprintf("Master broadcast->reply[%s]\n", proto->data[0] == PROTO_ACK ? "ACK" : "NACK");
	return 0;
}

static int cmd_slave_data(KFile *fd, Protocol *proto)
{
	(void)fd;
	(void)proto;
	return 0;
}

const Cmd slave_cmd[] = {
	{ CMD_BROADCAST, cmd_slave_broadcast },
	{ CMD_DATA,      cmd_slave_data      },
	{ 0     , NULL }
};

void cmd_poll(KFile *fd, Protocol *proto)
{
	memset(proto, 0, sizeof(Protocol));
	for (int i = 0; i < CMD_DEVICES; i++)
	{
		kprintf("%d: ", i);
		if (local_dev[i].addr)
		{
			if (ticks_to_ms(timer_clock() - local_dev[i].timeout) > CMD_TIMEOUT)
			{
				kprintf("Addr[%d] timeout remove it..\n", local_dev[i].addr);
				memset(&local_dev[i], 0, sizeof(Devices));
				continue;
			}

			kprintf("Addr[%d],status[%d],ticks[%ld]\n", local_dev[i].addr,
						local_dev[i].status, local_dev[i].timeout);

			if (local_dev[i].status == CMD_NEW_DEV)
			{
				kprintf("Get data..from[%d]\n", local_dev[i].addr);
				int ret = protocol_send(fd, proto, local_dev[i].addr, CMD_DATA,
						(const uint8_t *)"data_query", sizeof("data_query"));

				if (ret < 0)
					continue;

				local_dev[i].status = CMD_WAIT_DEV;
				continue;
			}
			if (local_dev[i].status == CMD_WAIT_DEV)
			{
			}
		}
		else
		{
			kprintf("Empty\n");
		}
	}
	kputs("-----\n");
}

