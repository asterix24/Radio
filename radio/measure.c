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
#include <math.h>

#define NTC_A 1.129241E-3
#define NTC_B 2.341077E-4
#define NTC_C 8.775468E-8


static uint32_t ntc_to_temp(uint16_t val)
{

    //float rntc = ;
    //float y = NTC_A + NTC_B * logf(rntc) + NTC_C * powf(logf(rntc), 3);
	//((1 / y - 273.15) * 1000)
	uint32_t rntc = ((val * 10000.0) / (4096.0 - val)) * 1000;

	//kprintf("r[%f]\n", rntc);
    return (uint32_t)rntc;
}

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

	kprintf("%ld %ld\n", ntc_to_temp(adc_read(0)), ntc_to_temp(adc_read(1)));

	return 0;
}

