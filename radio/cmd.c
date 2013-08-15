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

#include <string.h>

static Devices local_dev[CMD_DEVICES];

static int cmd_broadcast(KFile *fd, Protocol *proto)
{
	uint8_t reply = 0;
	for (int i = 0; i < 5; i++)
	{
		if (local_dev[i].addr == proto->addr)
		{
			reply = PROTO_ACK;
			break;
		}
		else if (!local_dev[i].addr)
		{
			local_dev[i].addr =  proto->addr;
			local_dev[i].len =  proto->len;
			memcpy(local_dev[i].data, proto->data, proto->len);
			reply = PROTO_ACK;
			break;
		}
		else
			reply = PROTO_NACK;
	}

	//proto->data[proto->len] = '\0';
	//kprintf("type[%d], addr[%d], data[%s]\n", proto->type, proto->addr, proto->data);

	if (protocol_reply(fd, proto, proto->addr, &reply, sizeof(reply)))
		return 0;

	return -1;
}

static int cmd_data(KFile *fd, Protocol *proto)
{
	(void)fd;
	kprintf("type[%d], addr[%d]\n", proto->type, proto->addr);

	kprintf("Decode data:len[%d]\n", proto->len);
	for (int i = 0; i < CMD_DEVICES; i++)
	{
		if (local_dev[i].addr == proto->addr)
		{
			size_t index = 0;
			kprintf("l[%d],d[%s]\n", local_dev[i].len, local_dev[i].data);
			for (size_t j = 0; j < local_dev[i].len; j++)
			{
				if (local_dev[i].data[j] == 'h')
				{
					ASSERT(index <= proto->len);
					kprintf("%d;", (int16_t)proto->data[index]);
					index += sizeof(int16_t);
				}
				if (local_dev[i].data[j] == 'H')
				{
					ASSERT(index <= proto->len);
					kprintf("%d;", (uint16_t)proto->data[index]);
					index += sizeof(uint16_t);
				}
			}

			break;
		}
	}
	kputs("\n");

	return 0;
}

const Cmd master_cmd[] = {
	{ CMD_TYPE_BROADCAST, cmd_broadcast },
	{ CMD_TYPE_DATA,      cmd_data      },
	{ 0   , NULL }
};

const Cmd slave_cmd[] = {
	{ 0     , NULL }
};

void cmd_poll(void)
{
	for (int i = 0; i < CMD_DEVICES; i++)
	{
		kprintf("%d: ", i);
		if (local_dev[i].addr)
			kprintf("Addr[%d],data[%s],len[%d]\n", local_dev[i].addr, local_dev[i].data, local_dev[i].len);
		else
			kprintf("Empty\n");
	}
	kputs("-----\n");
}

