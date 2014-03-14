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

# if 1
static const Table t[] = {
	{ 100   ,  13721 },
	{ 200   ,  10975 },
	{ 300   ,  9485 },
	{ 400   ,  8465 },
	{ 500   ,  7689 },
	{ 600   ,  7060 },
	{ 700   ,  6529 },
	{ 800   ,  6068 },
	{ 900   ,  5658 },
	{ 1000  ,  5288 },
	{ 1100  ,  4949 },
	{ 1200  ,  4635 },
	{ 1300  ,  4341 },
	{ 1400  ,  4063 },
	{ 1500  ,  3798 },
	{ 1600  ,  3545 },
	{ 1700  ,  3301 },
	{ 1800  ,  3064 },
	{ 1900  ,  2833 },
	{ 2000  ,  2607 },
	{ 2100  ,  2385 },
	{ 2200  ,  2164 },
	{ 2300  ,  1946 },
	{ 2400  ,  1727 },
	{ 2500  ,  1507 },
	{ 2600  ,  1285 },
	{ 2700  ,  1060 },
	{ 2800  ,  830 },
	{ 2900  ,  594 },
	{ 3000  ,  349 },
	{ 3100  ,  94 },
	{ 3200  ,  -175 },
	{ 3300  ,  -461 },
	{ 3400  ,  -770 },
	{ 3500  ,  -1110 },
	{ 3600  ,  -1492 },
	{ 3700  ,  -1935 },
	{ 3800  ,  -2475 },
	{ 3900  ,  -3189 },
	{ 4000  ,  -4320 },
};
#else
static const Table t[] = {
	{ 100   ,  13721 },
	{ 300   ,  9485 },
	{ 500   ,  7689 },
	{ 700   ,  6529 },
	{ 900   ,  5658 },
	{ 1000  ,  5288 },
	{ 1100  ,  4949 },
	{ 1300  ,  4341 },
	{ 1400  ,  4063 },
	{ 1500  ,  3798 },
	{ 1600  ,  3545 },
	{ 1700  ,  3301 },
	{ 1800  ,  3064 },
	{ 1900  ,  2833 },
	{ 2000  ,  2607 },
	{ 2100  ,  2385 },
	{ 2200  ,  2164 },
	{ 2300  ,  1946 },
	{ 2400  ,  1727 },
	{ 2500  ,  1507 },
	{ 2600  ,  1285 },
	{ 2700  ,  1060 },
	{ 2800  ,  830 },
	{ 2900  ,  594 },
	{ 3000  ,  349 },
	{ 3100  ,  94 },
	{ 3200  ,  -175 },
	{ 3300  ,  -461 },
	{ 3400  ,  -770 },
	{ 3500  ,  -1110 },
	{ 3700  ,  -1935 },
	{ 3900  ,  -3189 },
	{ 4000  ,  -4320 },
};
#endif

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

static void  measure_ntc(int ch, uint8_t *data, size_t len)
{
	ASSERT(len >= sizeof(int32_t));
	int v = table_linearInterpolation(t, countof(t), adc_read(ch));
	memcpy(data, &v, sizeof(int32_t));
}

int measure_ntc0(uint8_t *data, size_t len)
{
	measure_ntc(0, data, len);
	return 0;
}

int measure_ntc1(uint8_t *data, size_t len)
{
	measure_ntc(1, data, len);
	return 0;
}

