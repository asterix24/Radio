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
#include "radio_cfg.h"

#include <cfg/debug.h>

#include <string.h>

const Cmd *proto_cmd;

static cmd_t protocol_search(Protocol *proto)
{
	for (int i = 0; (proto_cmd[i].id != 0) && (proto_cmd[i].callback != NULL); i++)
		if (proto->type == proto_cmd[i].id)
			return proto_cmd[i].callback;

	return NULL;
}
INLINE int check_err(KFile *fd)
{
	int ret = kfile_error(fd);
	if (ret < 0)
	{
		kfile_clearerr(fd);
		if (ret == RADIO_RX_TIMEOUT)
			return PROTO_TIMEOUT;

		return PROTO_ERR;
	}
	return ret;
}

int protocol_send(KFile *fd, Protocol *proto, uint8_t addr, uint8_t type, const uint8_t *data, size_t len)
{
	ASSERT(len < PROTO_DATALEN);

	proto->type = type;
	proto->addr = addr;
	proto->len = len;
	memcpy(proto->data, data, len);

	kfile_write(fd, proto, sizeof(Protocol));

	return check_err(fd);
}


int protocol_poll(KFile *fd, Protocol *proto)
{
	kfile_read(fd, proto, sizeof(Protocol));
	int ret = check_err(fd);
	if (ret < 0)
		return ret;

	ASSERT(proto);

	uint8_t addr = radio_cfg_id();
	// If we are slave discard all message not for us.
	if (addr != RADIO_MASTER && proto->addr != addr)
		return PROTO_WRONG_ADDR;

	cmd_t callback = protocol_search(proto);
	if (callback)
		return callback(fd, proto);

	return PROTO_ERR;
}


void protocol_decode(Radio *fd, Protocol *proto)
{
	kprintf("Decode data:len[%d]\n", proto->len);
	uint8_t id = radio_cfg_id();
	const RadioCfg *cfg = radio_cfg(id);
	kprintf("$%d;%d;%d;", proto->addr, fd->lqi, fd->rssi);

	size_t index = 0;
	for (size_t j = 0; j < cfg->fmt_len; j++)
	{
		/* 'h': 2 byte signed */
		if (cfg->fmt[j] == 'h')
		{
			ASSERT(index <= proto->len);
			int16_t d;
			memcpy(&d, &proto->data[index], sizeof(int16_t));
			kprintf("%d;", d);
			index += sizeof(int16_t);
		}
		/* 'H': 2 byte unsigned */
		if (cfg->fmt[j] == 'H')
		{
			ASSERT(index <= proto->len);
			uint16_t d;
			memcpy(&d, &proto->data[index], sizeof(uint16_t));
			kprintf("%d;", d);
			index += sizeof(uint16_t);
		}
	}
	kputs("\n");
}


void protocol_encode(Protocol *proto, uint8_t *buf, size_t len)
{
	kprintf("encode data:len[%d]\n", proto->len);
	uint8_t id = radio_cfg_id();
	const RadioCfg *cfg = radio_cfg(id);
	kputs("$");
	size_t index = 0;
	for (size_t i = 0; i < cfg->fmt_len; i++)
	{
		if (cfg->fmt[i] == 'h')
		{
			int16_t d;
			ASSERT(cfg->callbacks[i]);
			ASSERT(index < len);

			cfg->callbacks[i]((uint8_t *)&d, sizeof(d));
			memcpy(&buf[index], (uint8_t *)&d, sizeof(d));
			index += sizeof(d);
			kprintf("%d;", d);
		}
		if (cfg->fmt[i] == 'H')
		{
			uint16_t d;
			ASSERT(cfg->callbacks[i]);
			ASSERT(index < len);

			cfg->callbacks[i]((uint8_t *)&d, sizeof(d));
			memcpy(&buf[index], (uint8_t *)&d, sizeof(d));
			index += sizeof(d);
			kprintf("%d;", d);
		}
	}
	kputs("\n");
}

void protocol_init(const Cmd *table)
{
	proto_cmd = table;
}

