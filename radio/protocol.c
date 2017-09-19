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
#include "radio_cfg.h"
#include "hw/hw_pwr.h"

#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   1
#define LOG_FORMAT  0

#include <cfg/log.h>

#include <drv/rtc.h>
#include <drv/bkp_stm32.h>

#include <algo/rotating_hash.h>

#include <string.h>

#define DECODE(msg, cfg, data, len, mask, T, index, fmt) \
	do { \
		if ((cfg) & (mask)) { \
			T tmp; \
			if ((index) + sizeof(T) > (len)) {  \
				LOG_ERR("MSGlong\n"); \
				return -1; \
			} \
			memcpy(&tmp, &(data)[(index)], sizeof(T)); \
			(index) += sizeof(T); \
			kprintf((fmt), tmp); \
		} \
	} while (0)

int protocol_decode(Radio *fd, Protocol *prt, uint8_t *data, size_t len)
{
	// Decode data from buffer
	memcpy(prt, data, sizeof(Protocol));

	int cfg = radio_cfg(prt->addr);
	if (cfg < 0) {
		LOG_ERR("Invalid addr[%d]\n", prt->addr);
		return -1;
	}
	LOG_INFO("Decode len[%d] addr[%d] cfg[%x]\n", prt->len, prt->addr, cfg);

	kprintf("$%d;%d;%d;%ld;", prt->addr, fd->lqi, fd->rssi, prt->timestamp);

	// Skip header to read raw data.
	size_t index = sizeof(Protocol);
	DECODE(prt, cfg, data, len, MEAS_INT_TEMP,   int16_t,  index, "%d;");
	DECODE(prt, cfg, data, len, MEAS_INT_VREF,   uint16_t, index, "%d;");
	DECODE(prt, cfg, data, len, MEAS_NTC_CH0,    int16_t,  index, "%d;");
	DECODE(prt, cfg, data, len, MEAS_NTC_CH1,    int16_t,  index, "%d;");
	DECODE(prt, cfg, data, len, MEAS_PHOTO_CH3,  uint16_t, index, "%d;");
	DECODE(prt, cfg, data, len, MEAS_PRESSURE,   int32_t,  index, "%ld;");
	DECODE(prt, cfg, data, len, MEAS_PRESS_TEMP, int16_t,  index, "%d;");

	kputs("\n");

	return 0;
}

#define ENCODE(msg, data, len, cfg, mask, callback, T, index, fmt) \
	do { \
		if ((cfg) & (mask)) { \
			T tmp; \
			(callback)((uint8_t *)&tmp, sizeof(T)); \
			if ((index) + sizeof(T) > (len)) {  \
				LOG_ERR("MSGlong\n"); \
				return -1; \
			} \
			memcpy(&(data)[index], (uint8_t *)&tmp,sizeof(T)); \
			(index) += sizeof(T); \
			(msg)->len += sizeof(T); \
			kprintf((fmt), tmp); \
		} \
	} while (0)

int protocol_encode(uint8_t id, uint8_t cfg, Protocol *prt, uint8_t *data, size_t len)
{

	prt->len = 0;
	prt->timestamp = rtc_time();
	prt->addr = id;

	kprintf("$%d;0;0;%ld;", id, prt->timestamp);

	/* Leave space for protocol header and the len of package for radio send */
	size_t index = sizeof(Protocol) + sizeof(uint8_t);

	ENCODE(prt, data, len, cfg, MEAS_INT_TEMP,   measure_intTemp,      int16_t,  index, "%d;");
	ENCODE(prt, data, len, cfg, MEAS_INT_VREF,   measure_intVref,      uint16_t, index, "%d;");
	ENCODE(prt, data, len, cfg, MEAS_NTC_CH0,    measure_ntc0,         int16_t,  index, "%d;");
	ENCODE(prt, data, len, cfg, MEAS_NTC_CH1,    measure_ntc1,         int16_t,  index, "%d;");
	ENCODE(prt, data, len, cfg, MEAS_PHOTO_CH3,  measure_light,        uint16_t, index, "%d;");
	ENCODE(prt, data, len, cfg, MEAS_PRESSURE,   measure_pressure,     int32_t,  index, "%ld;");
	ENCODE(prt, data, len, cfg, MEAS_PRESS_TEMP, measure_pressureTemp, int16_t,  index, "%d;");

	kputs("\n");
	data[0] = prt->len + sizeof(Protocol);
	memcpy(&data[1], prt, sizeof(Protocol));

	LOG_INFO("Encoded data len[%d]\n", data[0]);

	return index;
}

int protocol_encodeReply(uint8_t id, Protocol *prt, uint8_t *buf,size_t buf_len, \
		uint8_t *data, size_t len)
{
	prt->len = 0;
	prt->timestamp = rtc_time();
	prt->addr = id;
	size_t index = sizeof(Protocol) + sizeof(uint8_t);

	size_t size = MIN(buf_len, len);
	memcpy(&data[index], buf, size);
	prt->len += size;
	index += size;

	data[0] = prt->len + sizeof(Protocol);
	memcpy(&data[1], prt, sizeof(Protocol));
	return index;
}

int protocol_decodeReply(Protocol *prt, uint8_t *buf, size_t buf_len, \
		uint8_t *data, size_t len)
{
	(void)len;
	// Decode data from buffer
	memcpy(prt, data, sizeof(Protocol));
	size_t size = MIN(buf_len, (size_t)prt->len);
	memcpy(buf, &data[sizeof(Protocol)], size);

	return 0;
}


