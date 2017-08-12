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

#ifndef RADIO_CFG_H
#define RADIO_CFG_H

#include "measure.h"

#include <cpu/types.h>

#include <drv/gpio_stm32.h>
#include <drv/clock_stm32.h>

/*
 * Radio ids table.
 */
#define RADIO_MASTER    0
#define RADIO_DEBUG     15

#define RADIO_CFG_ID0   BV(5)  // PB5 -> P16
#define RADIO_CFG_ID1   BV(6)  // PB6 -> P17
#define RADIO_CFG_ID2   BV(7)  // PB7 -> P18
#define RADIO_CFG_ID3   BV(8)  // PB8 -> P19

#define RADIO_ID   (RADIO_CFG_ID0 | \
                    RADIO_CFG_ID1 | \
                    RADIO_CFG_ID2 | \
                    RADIO_CFG_ID3)


INLINE uint8_t radio_addr(void)
{
	uint16_t id = stm32_gpioPinRead(((struct stm32_gpio *)GPIOB_BASE), RADIO_ID);
	return (0xF - (id >> 5));
}

int radio_cfg(uint8_t addr);

INLINE void radio_cfg_init(void)
{
	RCC->APB2ENR |= RCC_APB2_GPIOB;
	stm32_gpioPinConfig(((struct stm32_gpio *)GPIOB_BASE), RADIO_ID, GPIO_MODE_IPU, GPIO_SPEED_50MHZ);
}

#endif /* RADIO_CFG_H */

