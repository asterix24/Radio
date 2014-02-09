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

#include <algo/table.h>

#include <string.h>

static const Table t[] = {
	{ 100   ,  137212 },
	{ 200   ,  109753 },
	{ 300   ,  94853 },
	{ 400   ,  84653 },
	{ 500   ,  76888 },
	{ 600   ,  70599 },
	{ 700   ,  65291 },
	{ 800   ,  60679 },
	{ 900   ,  56584 },
	{ 1000  ,  52883 },
	{ 1100  ,  49492 },
	{ 1200  ,  46348 },
	{ 1300  ,  43405 },
	{ 1400  ,  40626 },
	{ 1500  ,  37982 },
	{ 1600  ,  35449 },
	{ 1700  ,  33008 },
	{ 1800  ,  30641 },
	{ 1900  ,  28333 },
	{ 2000  ,  26073 },
	{ 2100  ,  23847 },
	{ 2200  ,  21644 },
	{ 2300  ,  19454 },
	{ 2400  ,  17266 },
	{ 2500  ,  15068 },
	{ 2600  ,  12850 },
	{ 2700  ,  10599 },
	{ 2800  ,  8300 },
	{ 2900  ,  5937 },
	{ 3000  ,  3492 },
	{ 3100  ,  941 },
	{ 3200  ,  -1747 },
	{ 3300  ,  -4610 },
	{ 3400  ,  -7702 },
	{ 3500  ,  -11100 },
	{ 3600  ,  -14919 },
	{ 3700  ,  -19353 },
	{ 3800  ,  -24752 },
	{ 3900  ,  -31893 },
	{ 4000  ,  -43198 },
};

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

int measure_ntc0(uint8_t *data, size_t len)
{
	ASSERT(len >= sizeof(int32_t));
	int v = table_linearInterpolation(t, countof(t), adc_read(0));
	memcpy(data, &v, sizeof(int32_t));

	return 0;
}

int measure_ntc1(uint8_t *data, size_t len)
{
	ASSERT(len >= sizeof(int32_t));
	int v = table_linearInterpolation(t, countof(t), adc_read(1));
	memcpy(data, &v, sizeof(int32_t));

	return 0;
}

