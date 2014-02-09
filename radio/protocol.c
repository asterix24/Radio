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

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   2
#define LOG_FORMAT  0

#include <cfg/log.h>

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

	LOG_INFO("Send[%d] to [%d]\n", proto->type, proto->addr);

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

	LOG_INFO("Poll cmd[%d]\n", proto->type);

	cmd_t callback = protocol_search(proto);
	if (callback)
		return callback(fd, proto);

	return PROTO_ERR;
}


#define DECODE(T, len, fmt) \
	do { \
		T tmp; \
		(len) = sizeof(T); \
		memcpy(&tmp, &proto->data[index], len); \
		kprintf((fmt), tmp); \
	} while (0)

void protocol_decode(Radio *fd, Protocol *proto)
{
	LOG_INFO("Decode len[%d]\n", proto->len);
	uint8_t id = radio_cfg_id();
	const RadioCfg *cfg = radio_cfg(id);

	kprintf("$%d;%d;%d;%ld;", proto->addr, fd->lqi, fd->rssi, proto->timestamp);

	size_t index = 0;
	for (size_t j = 0; j < cfg->fmt_len; j++)
	{
		if (index >= proto->len)
			break;

		size_t len = 0;

		if (cfg->fmt[j] == 'h')
			DECODE(int16_t, len, "%d;");
		else if (cfg->fmt[j] == 'H')
			DECODE(uint16_t, len, "%d;");
		else if (cfg->fmt[j] == 'I')
			DECODE(int32_t, len, "%ld;");
		else if (cfg->fmt[j] == 'i')
			DECODE(uint32_t, len, "%ld;");

		index += len;
	}
	kputs("\n");
}

#define ENCODE(T, len, fmt) \
	do { \
		T tmp; \
		(len) = sizeof(T); \
		cfg->callbacks[i]((uint8_t *)&tmp, (len)); \
		memcpy(&proto->data[index], (uint8_t *)&tmp,(len)); \
		kprintf((fmt), tmp); \
	} while (0)

void protocol_encode(Radio *fd, Protocol *proto)
{
	(void)fd;
	LOG_INFO("Encode data\n");
	uint8_t id = radio_cfg_id();
	const RadioCfg *cfg = radio_cfg(id);

	ASSERT(cfg);

	memset(proto, 0, sizeof(Protocol));

	proto->timestamp = rtc_time();
	kprintf("$%d;0;0;%ld;", id, proto->timestamp);

	size_t index = 0;
	for (size_t i = 0; i < cfg->fmt_len; i++)
	{
		ASSERT(cfg->callbacks[i]);
		ASSERT(index < PROTO_DATALEN);

		size_t len = 0;

		if (cfg->fmt[i] == 'h')
			ENCODE(int16_t, len, "%d;");   /* 'h': 2 byte signed */
		else if (cfg->fmt[i] == 'H')
			ENCODE(uint16_t, len, "%d;");   /* 'H': 2 byte unsigned */
		else if (cfg->fmt[i] == 'i')
			ENCODE(int32_t, len, "%ld;");   /* 'I': 4 byte unsigned */
		else if (cfg->fmt[i] == 'I')
			ENCODE(uint32_t, len, "%ld;");  /* 'i': 4 byte unsigned */

		index += len;
		proto->len += len;
	}

	kputs("\n");
}

void protocol_init(const Cmd *table)
{
	proto_cmd = table;
}

