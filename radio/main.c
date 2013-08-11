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
#include "protocol.h"
#include "cmd.h"

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

static Protocol proto;
int main(void)
{
	init();
	int id = radio_id();

	kprintf("%s [%d]\n", id == RADIO_MASTER ? "MASTER" : "SLAVE", id);
	if (id == RADIO_MASTER)
	{
		protocol_init(master_cmd);
		while(1)
		{
			kputs("Ready:\n");
			protocol_poll(&radio.fd);
		}
	}
	else
	{
		protocol_init(slave_cmd);
		while (1)
		{
			int sent = protocol_broadcast(&radio.fd, &proto, 1, (uint8_t *)"primo messaggio", sizeof("primo messaggio"));
			kprintf("Sent[%d]\n", sent);

			radio_timeout(&radio, 1000);

			memset(&proto, 0, sizeof(Protocol));
			int ret = 0;
			if ((ret = protocol_checkACK(&radio.fd, &proto)) == PROTO_OK)
				kprintf("ACK\n");
			else
				kprintf("err[%d]\n", ret);

			timer_delay(5000);
		}
	}

	return 0;
}

