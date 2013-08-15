/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
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
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2013 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Radio config.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "radio_cfg.h"

#include <cfg/macros.h>

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

#define RADIO_HW_INIT() \
do { \
	RCC->APB2ENR |= RCC_APB2_GPIOB;			\
	stm32_gpioPinConfig(((struct stm32_gpio *)GPIOB_BASE), RADIO_ID, GPIO_MODE_IPU, GPIO_SPEED_50MHZ); \
} while (0)

/*
 * Get the device id
 */
uint8_t radio_cfg_id(void)
{
	return stm32_gpioPinRead(((struct stm32_gpio *)GPIOB_BASE), BV(5) | BV(6)) >> 5;
}

void radio_cfg_init(void)
{
	RADIO_HW_INIT();
}


