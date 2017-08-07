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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief MPL3115A2 Pressure sensor whit altimeter
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "mpl3115a2.h"

#include <drv/timer.h>

void mpl3115a2_init(I2c *i2c)
{
	i2c_start_w(i2c, MPL3115A2_ADDR, 2, I2C_STOP);
	i2c_putc(i2c, MPL3115A2_CTRL1);
	i2c_putc(i2c, 0x38);

	i2c_start_w(i2c, MPL3115A2_ADDR, 2, I2C_STOP);
	i2c_putc(i2c, MPL3115A2_PT_DATA_CFG);
	i2c_putc(i2c, 0x07);

	i2c_start_w(i2c, MPL3115A2_ADDR, 2, I2C_STOP);
	i2c_putc(i2c, MPL3115A2_CTRL1);
	i2c_putc(i2c, 0x39);


	while (1) {
		i2c_start_w(i2c, MPL3115A2_ADDR, 1, I2C_NOSTOP);
		i2c_putc(i2c, 0x0);
		i2c_start_r(i2c, MPL3115A2_ADDR, 1, I2C_STOP);
		uint8_t status = i2c_getc(i2c);
		if (status && 0x8)
		{
			int32_t pressure = 0;
			uint8_t frac_pressure = 0;
			int16_t temp = 0;
			uint8_t frac_temp = 0;

			i2c_start_w(i2c, MPL3115A2_ADDR, 1, I2C_NOSTOP);
			i2c_putc(i2c, 0x1);
			i2c_start_r(i2c, MPL3115A2_ADDR, 1, I2C_STOP);
			uint8_t p_msb = i2c_getc(i2c);

			i2c_start_w(i2c, MPL3115A2_ADDR, 1, I2C_NOSTOP);
			i2c_putc(i2c, 0x2);
			i2c_start_r(i2c, MPL3115A2_ADDR, 1, I2C_STOP);
			uint8_t p_csb = i2c_getc(i2c);

			i2c_start_w(i2c, MPL3115A2_ADDR, 1, I2C_NOSTOP);
			i2c_putc(i2c, 0x3);
			i2c_start_r(i2c, MPL3115A2_ADDR, 1, I2C_STOP);
			uint8_t p_lsb = i2c_getc(i2c);

			pressure = ((p_msb << 8 | p_csb) << 2) | p_lsb >> 6;
			frac_pressure = (p_lsb >> 4) & 0x3;

			i2c_start_w(i2c, MPL3115A2_ADDR, 1, I2C_NOSTOP);
			i2c_putc(i2c, 0x4);
			i2c_start_r(i2c, MPL3115A2_ADDR, 1, I2C_STOP);
			temp = i2c_getc(i2c);

			i2c_start_w(i2c, MPL3115A2_ADDR, 1, I2C_NOSTOP);
			i2c_putc(i2c, 0x5);
			i2c_start_r(i2c, MPL3115A2_ADDR, 1, I2C_STOP);
			frac_temp = i2c_getc(i2c) >> 4;

			kprintf("Temp[%d.%d] pressure[%ld.%d]\n", temp, frac_temp, pressure, frac_pressure);
		}
		timer_delay(300);
	}
}
