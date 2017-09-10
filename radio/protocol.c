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
#define LOG_LEVEL   3
#define LOG_FORMAT  0

#include <cfg/log.h>

#include <drv/rtc.h>
#include <algo/rotating_hash.h>

#include <drv/bkp_stm32.h>

#include <string.h>

#define DECODE(msg, cfg, mask, T, index, fmt) \
	do { \
		if ((cfg) & (mask)) { \
			T tmp; \
			memcpy(&tmp, &(msg)->data[index], sizeof(T)); \
			(index) += sizeof(T); \
			kprintf((fmt), tmp); \
		} \
	} while (0)

int protocol_decode(Radio *fd, Protocol *prt)
{
	int cfg = radio_cfg(prt->addr);
	if (cfg < 0) {
		LOG_ERR("No decode, unvalid addr[%d]", \
				prt->addr);
		return -1;
	}

	LOG_INFO("Decode len[%d] addr[%d] cfg[%x]\n", prt->len, prt->addr, cfg);
	kprintf("$%d;%d;%d;%ld;", prt->addr, fd->lqi, fd->rssi, prt->timestamp);

	size_t index = 0;
	DECODE(prt, cfg, MEAS_INT_TEMP,   int16_t,  index, "%d;");
	DECODE(prt, cfg, MEAS_INT_VREF,   uint16_t, index, "%d;");
	DECODE(prt, cfg, MEAS_NTC_CH0,    int16_t,  index, "%d;");
	DECODE(prt, cfg, MEAS_NTC_CH1,    int16_t,  index, "%d;");
	DECODE(prt, cfg, MEAS_PHOTO_CH3,  uint16_t, index, "%d;");
	DECODE(prt, cfg, MEAS_PRESSURE,   int32_t,  index, "%ld;");
	DECODE(prt, cfg, MEAS_PRESS_TEMP, int16_t,  index, "%d;");

	kputs("\n");

	return 0;
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

void protocol_encode(uint8_t id, uint8_t cfg, Radio *fd, Protocol *prt)
{
	(void)fd;
	LOG_INFO("Encode data\n");
	prt->len = 0;
	prt->timestamp = rtc_time();
	prt->addr = id;
	kprintf("$%d;0;0;%ld;", id, prt->timestamp);

	size_t index = 0;
	ENCODE(prt, cfg, MEAS_INT_TEMP,   measure_intTemp,      int16_t,  index, "%d;");
	ENCODE(prt, cfg, MEAS_INT_VREF,   measure_intVref,      uint16_t, index, "%d;");
	ENCODE(prt, cfg, MEAS_NTC_CH0,    measure_ntc0,         int16_t,  index, "%d;");
	ENCODE(prt, cfg, MEAS_NTC_CH1,    measure_ntc1,         int16_t,  index, "%d;");
	ENCODE(prt, cfg, MEAS_PHOTO_CH3,  measure_light,        uint16_t, index, "%d;");
	ENCODE(prt, cfg, MEAS_PRESSURE,   measure_pressure,     int32_t,  index, "%ld;");
	ENCODE(prt, cfg, MEAS_PRESS_TEMP, measure_pressureTemp, int16_t,  index, "%d;");

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

void slave_shutdown(void)
{
	uint32_t wup = rtc_time() + (uint32_t)CMD_SLEEP_TIME;
	LOG_INFO("Go Standby, wakeup to %lds\n", wup);
	rtc_setAlarm(wup);
	radio_sleep();
	go_standby();
}


void protocol_init(Protocol *proto)
{
	ASSERT(proto);
	memset(proto, 0, sizeof(Protocol));
}

