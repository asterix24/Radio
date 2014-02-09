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
 * Copyright 2014 Daniele Basile <asterix@develer.com>
 *
 * -->
 *
 * \brief borad power manager.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */


#include <cfg/compiler.h>

#include <drv/clock_stm32.h>
#include <drv/gpio_stm32.h>

#include <cpu/irq.h>

#include <io/stm32.h>

#define __WFI() asm volatile ("wfi")

static struct stm32_pwr *PWR = (struct stm32_pwr *)PWR_BASE;
static struct stm32_gpio *GPIOA = (struct stm32_gpio *)GPIOA_BASE;
static struct stm32_gpio *GPIOB = (struct stm32_gpio *)GPIOB_BASE;

INLINE void go_standby(void)
{
	PWR->CSR = 0;
	/* Shut down all clocks. */
	stm32_gpioPinConfig(GPIOA, 0xFFFF, GPIO_MODE_AIN, GPIO_SPEED_50MHZ);
	stm32_gpioPinConfig(GPIOB, 0xFFFF, GPIO_MODE_AIN, GPIO_SPEED_50MHZ);
	//RCC->APB2ENR = 0;
	//RCC->APB1ENR = 0;

	/* Clear Wake-up flag */
	PWR->CR |= BV(2); //PWR_CR_CWUF;
	/* Select STANDBY mode */
	PWR->CR |= BV(1); //PWR_CR_PDDS;

	/* Set SLEEPDEEP bit of Cortex System Control Register */
	*((reg32_t *)0xE000ED10) |= BV(2);
	 /* Request Wait For Interrupt */
	__WFI();
}

