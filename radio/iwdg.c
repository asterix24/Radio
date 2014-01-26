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
 * \brief Radio watchdog.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "iwdg.h"

#include <cfg/compiler.h>

#include <drv/timer.h>
#include <drv/clock_stm32.h>
#include <drv/gpio_stm32.h>

#include <cpu/irq.h>

#include <io/stm32.h>

#define __WFI() asm volatile ("wfi")

static struct stm32_pwr *PWR = (struct stm32_pwr *)PWR_BASE;
static struct stm32_iwdt *IWDT = (struct stm32_iwdt *)IWDG_BASE;
static struct stm32_gpio *GPIOA = (struct stm32_gpio *)GPIOA_BASE;
static struct stm32_gpio *GPIOB = (struct stm32_gpio *)GPIOB_BASE;

void go_standby(void)
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


/* 13sec */
void iwdt_reset(void)
{
	/* enable write to PR, RLR */
	IWDT->KR  = 0x5555;
	/* Init prescaler */
	IWDT->PR  = IWDG_PRESCALER_256;
	/* Init RLR */
	IWDT->RLR = 0xFFF;
	/* Reload the watchdog */
	IWDT->KR  = 0xAAAA;
	IWDT->KR  = 0xCCCC;
}

/*
 * ticks = 0xfff -> 4095 tick
 * 1 tick is ms = (1/(40KHz)) = 0,025ms
 * 1 wdg reset each = (0,025 * 4) * 4095 = 409,5 ms (prescaler = 4) => 0,4sec
 */
void iwdt_start(void)
{
	/* enable write to PR, RLR */
	IWDT->KR  = 0x5555;
	/* Init prescaler */
	IWDT->PR  = IWDG_PRESCALER_8;
	IWDT->KR  = 0xCCCC;
}

