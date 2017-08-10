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
#include <algo/rotating_hash.h>

#include <drv/bkp_stm32.h>

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


#define DECODE(msg, cfg, mask, T, index, fmt) \
	do { \
		if ((cfg) & (mask)) { \
			T tmp; \
			memcpy(&tmp, &(msg)->data[index], sizeof(T)); \
			(index) += sizeof(T); \
			kprintf((fmt), tmp); \
		} \
	} while (0)

void protocol_decode(Radio *fd, Protocol *proto)
{
	int cfg = radio_cfg(proto->addr);
	if (cfg < 0) {
		LOG_ERR("Unable to decode package, no valid cfg found for given address[%d]", \
				proto->addr);
		return;
	}

	LOG_INFO("Decode len[%d] addr[%d] cfg[%x]\n", proto->len, proto->addr, cfg);
	kprintf("$%d;%d;%d;%ld;", proto->addr, fd->lqi, fd->rssi, proto->timestamp);

	size_t index = 0;
	DECODE(proto, cfg, MEAS_INT_TEMP,  int16_t,  index, "%d;");
	DECODE(proto, cfg, MEAS_INT_VREF,  uint16_t, index, "%d;");
	DECODE(proto, cfg, MEAS_NTC_CH0,   int16_t,  index, "%d;");
	DECODE(proto, cfg, MEAS_NTC_CH1,   int16_t,  index, "%d;");
	DECODE(proto, cfg, MEAS_PHOTO_CH3, uint16_t, index, "%d;");

	kputs("\n");
}

#define ENCODE(msg, cfg, mask, callback, T, index, fmt) \
	do { \
		if ((cfg) & (mask)) { \
			T tmp; \
			(callback)((uint8_t *)&tmp, sizeof(T)); \
			memcpy(&(msg)->data[index], (uint8_t *)&tmp,sizeof(T)); \
			(index) += sizeof(T); \
			(msg)->len += sizeof(T); \
			kprintf((fmt), tmp); \
		} \
	} while (0)

void protocol_encode(Radio *fd, Protocol *proto)
{
	(void)fd;
	LOG_INFO("Encode data\n");
	uint8_t id = radio_cfg_id();
	int cfg = radio_cfg(id);

	memset(proto, 0, sizeof(Protocol));

	proto->timestamp = rtc_time();
	kprintf("$%d;0;0;%ld;", id, proto->timestamp);

	size_t index = 0;
	ENCODE(proto, cfg, MEAS_INT_TEMP,  measure_intTemp,  int16_t,  index, "%d;");
	ENCODE(proto, cfg, MEAS_INT_VREF,  measure_intVref,  uint16_t, index, "%d;");
	ENCODE(proto, cfg, MEAS_NTC_CH0,   measure_ntc0,     int16_t,  index, "%d;");
	ENCODE(proto, cfg, MEAS_NTC_CH1,   measure_ntc1,     int16_t,  index, "%d;");
	ENCODE(proto, cfg, MEAS_PHOTO_CH3, measure_light,    uint16_t, index, "%d;");

	kputs("\n");
}

void protocol_updateRot(Protocol *proto)
{
	// Save the rot of current encoded data.
	uint16_t rot;
	rotating_init(&rot);
	rotating_update(proto->data, proto->len, &rot);
	LOG_INFO("Update rot in bkp[%d]\n", rot);
	bkp_write(&rot, sizeof(uint16_t));
}

int protocol_isDataChage(Protocol *proto)
{
	uint16_t rot;
	uint16_t prev_rot;

	bkp_read(&prev_rot, sizeof(uint16_t));

	rotating_init(&rot);
	rotating_update(proto->data, proto->len, &rot);
	LOG_INFO("Prev[%d] Now[%d]\n", prev_rot, rot);

	if (rot == prev_rot)
		return 0;

	bkp_write(&rot, sizeof(uint16_t));
	return 1;
}

void protocol_init(const Cmd *table)
{
	proto_cmd = table;
}

