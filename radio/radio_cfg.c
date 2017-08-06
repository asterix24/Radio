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
 * \brief Radio config.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "radio_cfg.h"
#include "measure.h"

#include <cfg/macros.h>
#include <cfg/debug.h>

#include <io/stm32.h>

#include <cpu/types.h>

#include <drv/gpio_stm32.h>
#include <drv/clock_stm32.h>

#define RADIO_CFG_ID0   BV(5)  // PB5 -> P16
#define RADIO_CFG_ID1   BV(6)  // PB6 -> P17
#define RADIO_CFG_ID2   BV(7)  // PB7 -> P18
#define RADIO_CFG_ID3   BV(8)  // PB8 -> P19

#define RADIO_ID   (RADIO_CFG_ID0 | \
                    RADIO_CFG_ID1 | \
                    RADIO_CFG_ID2 | \
                    RADIO_CFG_ID3)

const RadioCfg default_cfg = {
	"hH", 2,
	{
		measure_intTemp,
		measure_intVref,
	},
	"SLAVE DEFAULT",
};

const RadioCfg master_cfg = {
	"hHh", 3,
	{
		measure_intTemp,
		measure_intVref,
		measure_ntc1,
	},
	"MASTER 1ntc",
};

const RadioCfg module1_cfg = {
	"hHhhH", 5,
	{
		measure_intTemp,
		measure_intVref,
		measure_ntc0,
		measure_ntc1,
		measure_light,
	},
	"SLAVE 2ntc res",
};

const RadioCfg debug_cfg = {
	"hHhhH", 5,
	{
		measure_intTemp,
		measure_intVref,
		measure_ntc0,
		measure_ntc1,
		measure_light,
	},
	"DEBUG MODE",
};

RadioCfg const *radio_cfg_table[] =
{
	&master_cfg,  // Id = 0 -> MASTER
	&module1_cfg,  // Id = 1
	&default_cfg,  // Id = 2
	&default_cfg, // Id = 3
	&default_cfg, // Id = 4
	&default_cfg, // Id = 5
	&default_cfg, // Id = 6
	&default_cfg, // Id = 7
	&module1_cfg,  // Id = 8
	&default_cfg, // Id = 9
	&default_cfg, // Id = 10
	&default_cfg, // Id = 11
	&default_cfg, // Id = 12
	&default_cfg, // Id = 13
	&default_cfg, // Id = 14
	&debug_cfg,   // Id = 15
};

/*
 * Get the device id
 */
uint8_t radio_cfg_id(void)
{
	uint16_t id = stm32_gpioPinRead(((struct stm32_gpio *)GPIOB_BASE), RADIO_ID);
	return (0xF - (id >> 5));
}

const RadioCfg *radio_cfg(int id)
{

	if ((size_t)id >= countof(radio_cfg_table))
		return NULL;

	return radio_cfg_table[id];
}

void radio_cfg_init(void)
{
	RCC->APB2ENR |= RCC_APB2_GPIOB;
	stm32_gpioPinConfig(((struct stm32_gpio *)GPIOB_BASE), RADIO_ID, GPIO_MODE_IPU, GPIO_SPEED_50MHZ);
}


