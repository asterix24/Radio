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


#include "radio_cc1101.h"

#include "hw/hw_cc1101.h"
#include "hw/hw_adc.h"

#include <cfg/debug.h>

#include <io/kfile.h>

#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/adc.h>
#include <drv/spi.h>

#include <string.h>

static Radio radio;

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();

	spi_init();
	adc_init();
	radio_init(&radio, ping_low_baud_868);
	radio_timeout(&radio, -1);
}

uint8_t tmp[100];
int main(void)
{
	init();
	int ret;
	int id = radio_id();

	memset(tmp,0x61,sizeof(tmp));

	kprintf("%s [%d]\n", id == RADIO_MASTER ? "MASTER" : "SLAVE", id);
	while (1)
	{
		if (id == RADIO_MASTER)
		{
			ret = kfile_read(&radio.fd, &tmp, sizeof(tmp));

			if (ret > 99)
				ret = 99;

			if (ret > 0)
			{
				tmp[ret + 1] = '\0';
				kprintf("%d [%s]\n", ret, tmp);
			}
		}
		else
		{
			for (int i = 1; i < 64; i++)
			{
				kprintf("%d\n", i);
				kfile_write(&radio.fd, &tmp, i);
				timer_delay(1000);
			}
		}
	}

	return 0;
}

