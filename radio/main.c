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

#include "hw/hw_cc1101.h"
#include "hw/hw_spi.h"
#include "hw/hw_adc.h"

#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/adc.h>
#include <drv/spi_bitbang.h>
#include <drv/ser.h>

#include <net/nmea.h>

#include <string.h>
#include <stdio.h>

struct Beacon
{
	uint32_t count;
	uint32_t code;
	uint16_t temp;
	uint16_t vref;
	char aprs_lat[9];
	char aprs_lon[10];
	char aprs_time[8];
	bool gps_fix;
	time_t gps_clock;
	ticks_t last_heard;
};

static Serial ser;
static struct Beacon beacon;
static nmeap_context_t nmea;
static NmeaGga gps_gga;
static char tmp[255];

static void nmea_gpgga_callback(nmeap_context_t *context, void *data, void *userdata)
{
    (void)data;
    (void)userdata;

    beacon.last_heard = timer_clock();
    sprintf(beacon.aprs_time, "%.6sh", context->token[1]);
    sprintf(beacon.aprs_lat, "%.7s%c", context->token[2], *context->token[3]);
    sprintf(beacon.aprs_lon, "%.8s%c", context->token[4], *context->token[5]);

    if (gps_gga.quality > 0)
    {
        beacon.gps_fix = true;
        beacon.gps_clock = gps_gga.time;
    }
    else
        beacon.gps_fix = false;
}



static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();
	spi_init();
	adc_init();

	cc1101_init(ping_low_baud_868);

}

int main(void)
{
	init();
	int ret;
	int id = radio_id();
	beacon.code = 0xdbf1;
	beacon.count = 0;

	kprintf("%s [%d]\n", id == RADIO_MASTER ? "MASTER" : "SLAVE", id);

	//nmeap_init(&nmea, NULL);
	//nmeap_addParser(&nmea, "GPGGA", nmea_gpgga, nmea_gpgga_callback, &gps_gga);

	if (id != RADIO_MASTER)
	{
		ser_init(&ser, SER_UART3);
		ser_setbaudrate(&ser, 38400);
	}


	while (1)
	{
		if (id == RADIO_MASTER)
		{
			if ((ret = radio_recv(&tmp, sizeof(tmp), -1)) > 0)
			{
				uint8_t lqi = radio_lqi();
				if (lqi & BV(7))
					kprintf("ricevo: [%s]\n", tmp);
			}

			memset(&tmp, 0, sizeof(tmp));
		}
		else
		{
			size_t len = kfile_gets(&ser.fd, tmp, sizeof(tmp));
			kfile_write(&ser.fd, tmp, len);
			radio_send(&tmp, len);
			radio_sleep();
			timer_delay(500);
		}
	}

	return 0;
}

