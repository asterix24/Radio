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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief STM32 RTC driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "clock_stm32.h"

#include <cfg/compiler.h>
#include <cfg/module.h>
#include <cfg/debug.h>

#include <io/stm32.h>

#include <cpu/power.h> // cpu_relax()

#include <drv/rtc.h>

/* PWR registers base */
static struct stm32_pwr *PWR = (struct stm32_pwr *)PWR_BASE;


static void rtc_enterConfig(void)
{
	/* Enter configuration mode */
	RTC_CRL |= RTC_CRL_CNF;
}

static void rtc_exitConfig(void)
{
	/* Exit from configuration mode */
	RTC_CRL &= ~RTC_CRL_CNF;
	while (!(RTC_CRL & RTC_CRL_RTOFF))
		cpu_relax();
}

uint32_t rtc_time(void)
{
	return (RTC_CNTH << 16) | RTC_CNTL;
}

void rtc_setTime(uint32_t val)
{
	rtc_enterConfig();
	RTC_CNTH = (val >> 16) & 0xffff;
	RTC_CNTL = val & 0xffff;
	rtc_exitConfig();
}

/* Initialize the RTC clock */
int rtc_init(void)
{
#if CONFIG_KERN
	MOD_CHECK(proc);
#endif
	/* Enable clock for Power interface */
	RCC->APB1ENR |= RCC_APB1_PWR;

	/* Enable access to RTC registers */
	PWR->CR |= PWR_CR_DBP;

	/* Enable LSE */
	RCC->BDCR |= RCC_BDCR_LSEON;
	/* Wait for LSE ready */
	while (!(RCC->BDCR & RCC_BDCR_LSERDY))
		cpu_relax();

	/* Set clock source and enable RTC peripheral */
	RCC->BDCR |= RTC_CLKSRC | RCC_BDCR_RTCEN;

	rtc_enterConfig();

	/* Set prescaler */
	RTC_PRLH = ((RTC_PERIOD * RTC_CLOCK / 1000 - 1) >> 16) & 0xff;
	RTC_PRLL = ((RTC_PERIOD * RTC_CLOCK / 1000 - 1)) & 0xffff;

	rtc_exitConfig();

	/* Disable access to the RTC registers */
	PWR->CR &= ~PWR_CR_DBP;

	return 0;
}

