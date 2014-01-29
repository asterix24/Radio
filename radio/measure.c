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
 * \brief Measure.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "measure.h"

#include "hw/hw_adc.h"

#include <cfg/macros.h>

#include <io/stm32.h>

#include <cpu/types.h>

#include <drv/gpio_stm32.h>
#include <drv/clock_stm32.h>
#include <drv/rtc.h>

#include <string.h>

int measure_intTemp(uint8_t *data, size_t len)
{
	ASSERT(len >= sizeof(uint16_t));
	uint16_t t = hw_readIntTemp();
	memcpy(data, &t, sizeof(uint16_t));

	return 0;
}

int measure_intVref(uint8_t *data, size_t len)
{
	ASSERT(len >= sizeof(uint16_t));
	uint16_t v = hw_readVrefint();
	memcpy(data, &v, sizeof(uint16_t));

	return 0;
}

int measure_ntc(uint8_t *data, size_t len)
{
	(void)data;
	(void)len;
	uint32_t v_blk = adc_read(0);
	uint32_t v_rame = adc_read(1);

	float r_blk = (float)(10000 * v_blk) / (float)(4096 - v_blk);
	float r_rame = (float)(10000 * v_rame) / (float)(4096 - v_rame);

	uint32_t rb = r_blk * 1000;
	uint32_t rr = r_rame * 1000;

#if 0
	//uint32_t vn_blk = 10000000000 * v_blk;
	uint32_t rb = v_blk * (1/(4096 - v_blk)) * 10000;

	uint32_t vn_rame = 100000000000 * v_rame;
	uint32_t vd_rame = (4096 - v_rame) * 1000;
	uint32_t rr = vn_rame / vd_rame;
#endif

	kprintf("%ld %ld\n", rb, rr);

	return 0;
}

