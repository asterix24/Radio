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
 *
 * -->
 *
 * \brief STM32 Indipendet Watchdog controlle.
 *
 * \author Daniele Basile <asterix@develer.com>
 */
#ifndef STM32_IWDT_H
#define STM32_IWDT_H

#include <io/stm32.h>

#define IWDG_WA_EN     ((uint16_t)0x5555)
#define IWDG_WA_DIS    ((uint16_t)0x0000)
#define IWDG_RESET_KEY ((uint16_t)0xAAAA)
#define IWDG_START_KEY ((uint16_t)0xCCCC)

/*
 * IWDG Prescaler
 */
#define IWDG_PRESCALER_4            ((uint8_t)0x00)
#define IWDG_PRESCALER_8            ((uint8_t)0x01)
#define IWDG_PRESCALER_16           ((uint8_t)0x02)
#define IWDG_PRESCALER_32           ((uint8_t)0x03)
#define IWDG_PRESCALER_64           ((uint8_t)0x04)
#define IWDG_PRESCALER_128          ((uint8_t)0x05)
#define IWDG_PRESCALER_256          ((uint8_t)0x06)

/*
 * IWDG Flag
 */
#define IWDG_FLAG_PVU               ((uint16_t)0x0001)
#define IWDG_FLAG_RVU               ((uint16_t)0x0002)

struct stm32_iwdt
{
	reg32_t KR;
	reg32_t PR;
	reg32_t RLR;
	reg32_t SR;
};

#endif /* STM32_IWDT_H */
