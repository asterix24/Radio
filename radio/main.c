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
 * \brief BSM-RADIO main.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#include "radio_cc1101.h"
#include "radio_cfg.h"
#include "protocol.h"
#include "cmd.h"
#include "measure.h"

#include "cfg/cfg_i2c.h"

#include "hw/hw_cc1101.h"
#include "hw/hw_adc.h"

#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   3
#define LOG_FORMAT  0

#include <cfg/log.h>

#include <io/kfile.h>

#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/adc.h>
#include <drv/spi.h>
#include <drv/rtc.h>

#include <string.h>

static Radio radio;
static Protocol proto;

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();
	rtc_init();

	spi_init();
	radio_cfg_init();

	radio_init(&radio, ping_low_baud_868);
	radio_timeout(&radio, 0);

	uint8_t id = radio_addr();
	int cfg = radio_cfg(id);
	ASSERT(cfg >= 0);

	measure_init();
	cmd_init(id);

	kprintf("\n\n <::== RADIO LOG SYSTEM ==::>\n\n");
	LOG_INFO("Module [%d] cfg[0x%x]\n", id, cfg);
}

int main(void)
{
	init();

	while (1)
		cmd_poll(&radio.fd, &proto);

	return 0;
}
