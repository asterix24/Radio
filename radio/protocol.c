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
 * \brief Radio protocol implementation.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "protocol.h"
#include "cmd.h"

#include <cfg/debug.h>

#include <string.h>

const Cmd *proto_cmd;

static cmd_t *protocol_search(Protocol *proto)
{
	for (int i = 0; (proto_cmd[i].id != 0) && (proto_cmd[i].callback != NULL); i++)
		if (proto->type == proto_cmd[i].id)
			return proto_cmd[i].callback;

	return NULL;
}

static bool protocol_send(KFile *fd, Protocol *proto, uint8_t type, uint8_t addr, uint8_t *data, size_t len)
{
	ASSERT(len < PROTO_DATALEN);

	proto->type = type;
	proto->addr = addr;
	proto->len = len;
	memcpy(proto->data, data, len);

	if (kfile_write(fd, proto, sizeof(Protocol)) > 0)
		return true;

	return false;
}

int protocol_broadcast(KFile *fd, Protocol *proto, uint8_t addr, uint8_t *data, size_t len)
{
	return protocol_send(fd, proto, CMD_TYPE_BROADCAST, addr, data, len);
}

int protocol_reply(KFile *fd, Protocol *proto, uint8_t addr, uint8_t *data, size_t len)
{
	return protocol_send(fd, proto, CMD_TYPE_REPLY, addr, data, len);
}

int protocol_waitReply(KFile *fd, Protocol *proto)
{
	int len = kfile_read(fd, proto, sizeof(Protocol));

	int ret = kfile_error(fd);
	if (ret < 0)
	{
		kfile_clearerr(fd);
		if (ret == RADIO_RX_TIMEOUT)
			return PROTO_TIMEOUT;

		return PROTO_ERR;
	}

	ASSERT(proto);
	if (proto->type == CMD_TYPE_REPLY)
		return PROTO_OK;
	else
		return PROTO_WRONG_ADDR;

	return PROTO_ERR;
}

void protocol_poll(KFile *fd)
{
	Protocol proto;
	kfile_read(fd, &proto, sizeof(Protocol));
	cmd_t *callback = protocol_search(&proto);

	if (callback)
		callback(fd, &proto);
}

void protocol_init(const Cmd *table)
{
	proto_cmd = table;
}

