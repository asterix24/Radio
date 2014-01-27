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
 * \brief Radio protocol implementation.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "protocol.h"
#include "cmd.h"
#include "radio_cfg.h"

#include <cfg/debug.h>

#include <drv/rtc.h>

#include <string.h>

const Cmd *proto_cmd;

static cmd_t protocol_search(Protocol *proto)
{
	for (int i = 0; (proto_cmd[i].id != 0) &&
				(proto_cmd[i].callback != NULL); i++)
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

int protocol_send(KFile *fd, Protocol *proto, uint8_t addr, uint8_t type)
{

	proto->type = type;
	proto->addr = addr;

	kfile_write(fd, proto, sizeof(Protocol));
	kprintf("Send[%d] to [%d]\n", proto->type, proto->addr);
	return check_err(fd);
}

int protocol_sendByte(KFile *fd, Protocol *proto, uint8_t addr,
										    uint8_t type, uint8_t data)
{
	proto->len = sizeof(data);
	memcpy(proto->data, &data, sizeof(data));

	return protocol_send(fd, proto, addr, type);
}

int protocol_sendBuf(KFile *fd, Protocol *proto, uint8_t addr,
							uint8_t type, const uint8_t *data, size_t len)
{
	ASSERT(len < PROTO_DATALEN);
	proto->len = len;
	memcpy(proto->data, data, len);

	return protocol_send(fd, proto, addr, type);
}

int protocol_poll(KFile *fd, Protocol *proto)
{
	memset(proto, 0, sizeof(Protocol));
	kfile_read(fd, proto, sizeof(Protocol));
	int ret = check_err(fd);
	if (ret < 0)
		return ret;

	ASSERT(proto);

	uint8_t addr = radio_cfg_id();
	// If we are slave discard all message not for us.
	if (addr != RADIO_MASTER && proto->addr != addr)
		return PROTO_WRONG_ADDR;

	kprintf("poll recv[%d]\n", proto->type);
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
	kprintf("$%d;%d;%d;%ld;", proto->addr, fd->lqi, fd->rssi,
												proto->timestamp);

	size_t index = 0;
	for (size_t j = 0; j < cfg->fmt_len; j++)
	{
		if (index > proto->len)
		{
			//kprintf("Buffer overun..");
			break;
		}

		/* 'h': 2 byte signed */
		if (cfg->fmt[j] == 'h')
		{
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
		/* 'I': 4 byte unsigned */
		if (cfg->fmt[j] == 'I')
		{
			ASSERT(index <= proto->len);
			uint32_t d;
			memcpy(&d, &proto->data[index], sizeof(uint32_t));
			kprintf("%ld;", d);
			index += sizeof(uint32_t);
		}
	}
	kputs("\n");
}

void protocol_encode(Protocol *proto)
{
	kprintf("Encode data\n");
	uint8_t id = radio_cfg_id();
	const RadioCfg *cfg = radio_cfg(id);

	proto->timestamp = rtc_time();

	kputs("$");
	size_t index = 0;
	for (size_t i = 0; i < cfg->fmt_len; i++)
	{
		if (cfg->fmt[i] == 'h')
		{
			int16_t d;
			ASSERT(cfg->callbacks[i]);
			ASSERT(index < PROTO_DATALEN);

			cfg->callbacks[i]((uint8_t *)&d, sizeof(d));
			memcpy(&proto->data[index], (uint8_t *)&d, sizeof(d));
			index += sizeof(d);
			proto->len += sizeof(d);
			kprintf("%d;", d);
		}
		if (cfg->fmt[i] == 'H')
		{
			uint16_t d;
			ASSERT(cfg->callbacks[i]);
			ASSERT(index < PROTO_DATALEN);

			cfg->callbacks[i]((uint8_t *)&d, sizeof(d));
			memcpy(&proto->data[index], (uint8_t *)&d, sizeof(d));
			index += sizeof(d);
			proto->len += sizeof(d);
			kprintf("%d;", d);
		}
		if (cfg->fmt[i] == 'I')
		{
			uint32_t d;
			ASSERT(cfg->callbacks[i]);
			ASSERT(index < PROTO_DATALEN);

			cfg->callbacks[i]((uint8_t *)&d, sizeof(d));
			memcpy(&proto->data[index], (uint8_t *)&d, sizeof(d));
			index += sizeof(d);
			proto->len += sizeof(d);
			kprintf("%ld;", d);
		}
	}
	kputs("\n");
}

void protocol_init(const Cmd *table)
{
	proto_cmd = table;
}

