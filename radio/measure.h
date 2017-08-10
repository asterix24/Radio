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

#include <cpu/types.h>

#define MEAS_INT_TEMP      BV(0)
#define MEAS_INT_VREF      BV(1)
#define MEAS_NTC_CH0       BV(2)
#define MEAS_NTC_CH1       BV(3)
#define MEAS_PHOTO_CH3     BV(4)
#define MEAS_PRESSURE      BV(5)
#define MEAS_PRESS_TEMP    BV(6)

#define MEAS_ALL           0x3F

typedef int (*measure_t)(uint8_t *data, size_t len);

int measure_intVref(uint8_t *data, size_t len);
int measure_intTemp(uint8_t *data, size_t len);
int measure_ntc0(uint8_t *data, size_t len);
int measure_ntc1(uint8_t *data, size_t len);
int measure_light(uint8_t *data, size_t len);

void measure_deInit(void);
void measure_init(void);

#endif /* RADIO_MEASURE_H */

