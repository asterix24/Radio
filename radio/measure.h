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
 * \brief Measure.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#ifndef RADIO_MEASURE_H
#define RADIO_MEASURE_H

#include "hw/hw_adc.h"
#include "hw/hw_measure.h"

#include <cpu/types.h>

#include <string.h>

#define MEAS_INT_TEMP      BV(0)
#define MEAS_INT_VREF      BV(1)
#define MEAS_NTC_CH0       BV(2)
#define MEAS_NTC_CH1       BV(3)
#define MEAS_PHOTO_CH3     BV(4)
#define MEAS_PRESSURE      BV(5)
#define MEAS_PRESS_TEMP    BV(6)


#define MEAS_ADC   ( MEAS_INT_TEMP | \
                     MEAS_INT_VREF | \
                     MEAS_NTC_CH0  | \
                     MEAS_NTC_CH1  | \
                     MEAS_PHOTO_CH3)

#define MEAS_I2C   (  MEAS_PRESSURE | \
                      MEAS_PRESS_TEMP )

#define MEAS_ALL           0x7F

INLINE int measure_intTemp(uint8_t *data, size_t len)
{
	(void)len;
	uint16_t t = hw_readIntTemp();
	memcpy(data, &t, sizeof(uint16_t));
	return 0;
}

INLINE int measure_intVref(uint8_t *data, size_t len)
{
	(void)len;
	uint16_t v = hw_readVrefint();
	memcpy(data, &v, sizeof(uint16_t));
	return 0;
}

int measure_ntc(int ch, uint8_t *data, size_t len);

INLINE int measure_ntc0(uint8_t *data, size_t len)
{
	return measure_ntc(0, data, len);
}

INLINE int measure_ntc1(uint8_t *data, size_t len)
{
	return measure_ntc(1, data, len);
}


INLINE int measure_light(uint8_t *data, size_t len)
{
	(void)len;
	uint16_t lin = adc_read(3);
	memcpy(data, &lin, sizeof(uint16_t));
	return 0;
}


int measure_pressure(uint8_t *data, size_t len);
int measure_pressureTemp(uint8_t *data, size_t len);

INLINE void measure_disable(void)
{
	MEASURE_OFF();
}

void measure_enable(int cfg);

void measure_init(void);

#endif /* RADIO_MEASURE_H */

