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

#include "cfg/cfg_rtc.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   RTC_LOG_LEVEL
#define LOG_FORMAT  RTC_LOG_FORMAT

#include <cfg/log.h>

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
	PWR->CR |= PWR_CR_DBP;
	while (!(RTC_CRL & RTC_CRL_RTOFF))
		cpu_relax();

	/* Enter configuration mode */
	RTC_CRL |= RTC_CRL_CNF;
}

static void rtc_exitConfig(void)
{
	/* Exit from configuration mode */
	RTC_CRL &= ~RTC_CRL_CNF;

	while (!(RTC_CRL & RTC_CRL_RTOFF))
		cpu_relax();

	PWR->CR &= ~PWR_CR_DBP;
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

void rtc_setAlarm(rtc_clock_t val)
{
	rtc_enterConfig();
	RTC_ALRH = (val >> 16) & 0xffff;
	RTC_ALRL = val & 0xffff;
	rtc_exitConfig();
}

/* Initialize the RTC clock */
int rtc_init(void)
{
#if CONFIG_KERN
	MOD_CHECK(proc);
#endif
	/* Enable clock for Power interface */
	RCC->APB1ENR |= RCC_APB1_PWR | RCC_APB1_BKP;

	/* Enable access to RTC registers */
	PWR->CR |= PWR_CR_DBP;

	/* Reset bkp domain to change clock source */
	RCC->BDCR |= RCC_BDCR_RTS;
	RCC->BDCR &= ~RCC_BDCR_RTS;

#if CFG_RTC_CLOCK_SRC==EXTERNAL // LSE
	/* Enable LSE */
	RCC->BDCR |= RCC_BDCR_LSEON;
	/* Wait for LSE ready */
	while (!(RCC->BDCR & RCC_BDCR_LSERDY))
		cpu_relax();
	/* Set clock source and enable RTC peripheral */
	RCC->BDCR |= RCC_BDCR_RTCSEL_LSE | RCC_BDCR_RTCEN;
#else // INTERNAL LSI

	/* Start LSI low speed internal oscillator */
	RCC->CSR |= BV(0);
	/* Whait for stable lsi clock */
	while(!(RCC->CSR & BV(1)));

	RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
	RCC->BDCR |= RCC_BDCR_RTCEN;

	/* wait syncro */
	RTC_CRL &= ~RTC_CRL_RSF;
	while (!(RTC_CRL & RTC_CRL_RSF))
		cpu_relax();

	rtc_exitConfig();

#endif

	rtc_enterConfig();

	/* Set prescaler */
	RTC_PRLH = ((CFG_RTC_PERIOD * CFG_RTC_CLOCK / 1000 - 1) >> 16) & 0xff;
	RTC_PRLL = ((CFG_RTC_PERIOD * CFG_RTC_CLOCK / 1000 - 1)) & 0xffff;

	rtc_exitConfig();

	/* Disable access to the RTC registers */
	PWR->CR &= ~PWR_CR_DBP;

	return 0;
}

