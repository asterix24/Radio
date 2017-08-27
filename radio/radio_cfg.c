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
 * \brief Radio config.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "radio_cfg.h"
#include "measure.h"

#include <cfg/macros.h>
#include <cfg/debug.h>

#include <io/stm32.h>

#include <cpu/types.h>




const uint16_t radio_cfg_table[] =
{
	MEAS_INT_TEMP | MEAS_INT_VREF | MEAS_NTC_CH1,  // Id = 0 -> MASTER
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 1
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 2
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 3
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 4
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 5
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 6
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 7
	MEAS_INT_TEMP | MEAS_INT_VREF | MEAS_NTC_CH0 | MEAS_NTC_CH1 | MEAS_PHOTO_CH3 | MEAS_PRESSURE | MEAS_PRESS_TEMP, // Id = 8
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 9
	MEAS_INT_TEMP | MEAS_INT_VREF | MEAS_NTC_CH0 | MEAS_NTC_CH1 | MEAS_PHOTO_CH3 | MEAS_PRESSURE | MEAS_PRESS_TEMP, // Id = 10
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 11
	MEAS_INT_TEMP | MEAS_INT_VREF | MEAS_NTC_CH0 | MEAS_NTC_CH1, // Id = 12
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 13
	MEAS_INT_TEMP | MEAS_INT_VREF, // Id = 14
	MEAS_ADC                       // Id = 15 -> DEBUG MODE
};

int radio_cfg(uint8_t addr)
{
	if ((size_t)addr >= sizeof(radio_cfg_table))
		return -1;

	return radio_cfg_table[addr];
}



