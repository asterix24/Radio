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
 *
 * -->
 *
 * \brief STM32 Backup registers driver.
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "clock_stm32.h"

#include <cfg/compiler.h>
#include <cfg/module.h>
#include <cfg/debug.h>

#include <io/stm32.h>

#define BKP_WORD_LEN  10

/* PWR registers base */
INLINE void bkp_unlock(void)
{
	struct stm32_pwr *PWR = (struct stm32_pwr *)PWR_BASE;

	/* Enable clock for Power interface */
	RCC->APB1ENR |= RCC_APB1_PWR | RCC_APB1_BKP;
	/* Enable access to BKP registers */
	PWR->CR |= PWR_CR_DBP;
}

INLINE void bkp_read(uint16_t *buf, size_t len)
{
	bkp_unlock();
	struct stm32_bkp *BKP = (struct stm32_bkp *)BKP_BASE;
	reg16_t *ptr = &BKP->DR1;

	for (size_t i = 0; i < BKP_WORD_LEN; i++)
	{
		if (i >= len)
			break;

		buf[i] = *ptr;
		ptr += 2;
	}
}

INLINE void bkp_write(uint16_t *buf, size_t len)
{
	bkp_unlock();
	struct stm32_bkp *BKP = (struct stm32_bkp *)BKP_BASE;
	reg16_t *ptr = &BKP->DR1;

	for (size_t i = 0; i < BKP_WORD_LEN; i++)
	{
		if (i >= len)
			break;

		*ptr = buf[i];
		ptr += 2;
	}
}

