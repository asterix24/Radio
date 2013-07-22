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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief BSM-RADIO main.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "spi_stm32.h"

#include "hw/hw_spi.h"

#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/power.h>

#include <io/stm32.h>
#include <drv/clock_stm32.h>
#include <drv/spi.h>



uint8_t spi_hw_sendRecv(uint8_t c)
{
	uint8_t ch;
	SPI1->DR = (uint8_t)c;
	while (!(SPI1->SR & SPI_SR_TXE));

	while (!(SPI1->SR & SPI_SR_RXNE));
	ch = (uint8_t)SPI1->DR;

	return ch;
}

void spi_hw_init(void)
{
	SPI_HW_INIT();
	RCC->APB2ENR |= RCC_APB2_SPI1;

	/* MSB, Baudrate=6MHz MASTER, spi en */
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | (0x8) | SPI_CR1_SPE;
	SPI1->CR2 = SPI_CR2_SSOE;
	SPI1->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;
}
