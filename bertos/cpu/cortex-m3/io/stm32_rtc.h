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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief STM32F103xx RTC definition.
 */

#ifndef STM32_RTC_H
#define STM32_RTC_H

#include <cpu/types.h>



/* RTC control register */
#define RTC_CRH		(*(reg16_t *)(RTC_BASE + 0x00))
#define RTC_CRL		(*(reg16_t *)(RTC_BASE + 0x04))

#define RTC_CRL_SECIE         BV(0)
#define RTC_CRL_ALRIE         BV(1)
#define RTC_CRL_OWIE          BV(2)

#define RTC_CRL_SECF          BV(0)
#define RTC_CRL_ALRF          BV(1)
#define RTC_CRL_OWF           BV(2)
#define RTC_CRL_RSF           BV(3)
#define RTC_CRL_CNF           BV(4)
#define RTC_CRL_RTOFF         BV(5)

/* RTC prescaler load register */
#define RTC_PRLH	(*(reg16_t *)(RTC_BASE + 0x08))
#define RTC_PRLL	(*(reg16_t *)(RTC_BASE + 0x0c))

/* RTC prescaler divider register */
#define RTC_DIVH	(*(reg16_t *)(RTC_BASE + 0x10))
#define RTC_DIVL	(*(reg16_t *)(RTC_BASE + 0x14))

/* RTC counter register */
#define RTC_CNTH	(*(reg16_t *)(RTC_BASE + 0x18))
#define RTC_CNTL	(*(reg16_t *)(RTC_BASE + 0x1c))

/* RTC alarm register */
#define RTC_ALRH	(*(reg16_t *)(RTC_BASE + 0x20))
#define RTC_ALRL	(*(reg16_t *)(RTC_BASE + 0x24))

#endif /* STM32_RTC_H */
