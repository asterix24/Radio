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

#ifndef RADIO_CFG_H
#define RADIO_CFG_H

#include "measure.h"

#include <cpu/types.h>

/*
 * Radio ids table.
 */
#define RADIO_MASTER    0
#define RADIO_DEBUG     15

typedef struct RadioCfg
{
	uint8_t fmt[10];
	size_t fmt_len;
	measure_t callbacks[10];
} RadioCfg;

const RadioCfg *radio_cfg(int id);
uint8_t radio_cfg_id(void);
void radio_cfg_init(void);

#endif /* RADIO_CFG_H */

