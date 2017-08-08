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
 * Copyright 2014 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Measure sub-system manager
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_MEASURE_H
#define HW_MEASURE_H

#include <cfg/macros.h>
#include <io/stm32.h>

#define MEASURE_SWITCH   BV(13)         // PC13 --> P21


#define MEASURE_ON()   \
	do { \
		stm32_gpioPinWrite(((struct stm32_gpio *)GPIOC_BASE), MEASURE_SWITCH, 0); \
	} while(0)

#define MEASURE_OFF()  \
	do { \
		stm32_gpioPinWrite(((struct stm32_gpio *)GPIOC_BASE), MEASURE_SWITCH, 1); \
	} while(0)

#define MEASURE_INIT() \
	do { \
		RCC->APB2ENR |= RCC_APB2_GPIOC; \
		stm32_gpioPinConfig(((struct stm32_gpio *)GPIOC_BASE), MEASURE_SWITCH, GPIO_MODE_OUT_PP, GPIO_SPEED_50MHZ);\
	} while(0)

#endif /* HW_MEASURE_H */
