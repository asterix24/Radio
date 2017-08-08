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
 * $WIZ$ module_name = "mpl3115a2"
 * $WIZ$ module_depends = "i2c"
 * $WIZ$ module_configuration = ""
 * $WIZ$ module_hw = ""
 */

#ifndef DRV_MLP3115A2_H
#define DRV_MLP3115A2_H

#include <drv/i2c.h>

#define MPL3115A2_ADDR        0xC0

#define MPL3115A2_STATUS      0x00
#define MPL3115A2_OUT_P_MSB   0x01
#define MPL3115A2_OUT_P_CSB   0x02
#define MPL3115A2_OUT_P_LSB   0x03
#define MPL3115A2_OUT_T_MSB   0x04
#define MPL3115A2_OUT_T_LSB   0x05

#define MPL3115A2_PT_DATA_CFG 0x13

#define MPL3115A2_CTRL1       0x26

#define MPL3115A2_READ_TIMEOUT  500 //ms

void mpl3115a2_init(I2c *i2c);
int mlp3115a2_readPressure(I2c *i2c, int32_t *pressure, uint8_t *pressure_fract);
int mlp3115a2_readTemp(I2c *i2c, int16_t *temp, uint8_t *temp_fract);

#endif /* DRV_MPL3115A2 */
