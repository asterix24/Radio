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
 * Copyright 2017 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief MPL3115A2 Pressure sensor whit altimeter
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "mpl3115a2.h"

/* Define logging setting (for cfg/log.h module). */
#define LOG_LEVEL         2
#define LOG_FORMAT        0
#include <cfg/log.h>
#include <cfg/debug.h>
#include <drv/timer.h>

static int mpl3115a2_writeReg(I2c *i2c, uint8_t reg, uint8_t data)
{
	i2c_start_w(i2c, MPL3115A2_ADDR, 2, I2C_STOP);
	i2c_putc(i2c, reg);
	i2c_putc(i2c, data);

	if (i2c_error(i2c))
		return -1;

	return 0;
}

static int mpl3115a2_readReg(I2c *i2c, uint8_t reg, uint8_t *data)
{
	i2c_start_w(i2c, MPL3115A2_ADDR, 1, I2C_NOSTOP);
	i2c_putc(i2c, reg);
	i2c_start_r(i2c, MPL3115A2_ADDR, 1, I2C_STOP);
	*data = i2c_getc(i2c);

	if (i2c_error(i2c))
		return -1;

	return 0;
}

int mlp3115a2_readPressure(I2c *i2c, int32_t *pressure)
{

	ticks_t start = timer_clock();
	do {
		uint8_t status = 0;
		if (mpl3115a2_readReg(i2c, MPL3115A2_STATUS, &status) < 0)
			continue;

		if (status && 0x8)
		{
			uint8_t p_msb = 0;
			if (mpl3115a2_readReg(i2c, MPL3115A2_OUT_P_MSB, &p_msb) < 0)
				continue;

			uint8_t p_csb = 0;
			if (mpl3115a2_readReg(i2c, MPL3115A2_OUT_P_CSB, &p_csb) < 0)
				continue;

			uint8_t p_lsb = 0;
			if (mpl3115a2_readReg(i2c, MPL3115A2_OUT_P_LSB, &p_lsb) < 0)
				continue;

			*pressure = ((((p_msb << 8 | p_csb) << 2) | p_lsb >> 6) * 10) | ((p_lsb >> 4) & 0x3);
			return 0;
		}
	} while ((timer_clock() - start) < ms_to_ticks(MPL3115A2_READ_TIMEOUT));

	LOG_ERR("Timeout! Unable to get pressure from sensor\n");
	return -1;
}

int mlp3115a2_readTemp(I2c *i2c, int16_t *temp)
{
	ticks_t start = timer_clock();
	do {
		uint8_t status = 0;
		if (mpl3115a2_readReg(i2c, MPL3115A2_STATUS, &status) < 0)
			continue;

		if (status && 0x8)
		{
			uint8_t t_msb = 0;
			if (mpl3115a2_readReg(i2c, MPL3115A2_OUT_T_MSB, &t_msb) < 0)
				continue;

			uint8_t t_lsb = 0;
			if (mpl3115a2_readReg(i2c, MPL3115A2_OUT_T_LSB, &t_lsb) < 0)
				continue;

			*temp = (t_msb * 100) | t_lsb >> 4;
			return 0;
		}
	} while ((timer_clock() - start) < ms_to_ticks(MPL3115A2_READ_TIMEOUT));

	LOG_ERR("Timeout! Unable to get temp from sensor\n");
	return -1;
}

struct MPL3115A2_CFG {
	uint8_t reg;
	uint8_t data;
};

static struct MPL3115A2_CFG mpl3115a2_cfg[] = {
	{ MPL3115A2_CTRL1,        0x38 },
	{ MPL3115A2_PT_DATA_CFG,  0x07 },
	{ MPL3115A2_CTRL1,        0x39 },
	{ 0x00,                   0x00 },
};

void mpl3115a2_init(I2c *i2c)
{
	LOG_INFO("Init pressure sensor.\n");

	for (int i = 0;; i++)
	{
		if (!mpl3115a2_cfg[i].reg && !mpl3115a2_cfg[i].data)
			break;

		int ret = mpl3115a2_writeReg(i2c, mpl3115a2_cfg[i].reg, mpl3115a2_cfg[i].data);
		if (ret < 0)
			LOG_ERR("Unable to write mpl3115a configuration [%i]\n", i);
	}

}
