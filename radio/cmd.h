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
 * \brief Radio commands interface.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#ifndef RADIO_CMD_H
#define RADIO_CMD_H

#include "protocol.h"

#include <io/kfile.h>

/*
 * Message type
 */
#define CMD_BROADCAST        0xFF
#define CMD_WAIT             0x1
#define CMD_SLEEP            0x2

/*
 * Device and slave status
 */
#define CMD_NEW_DEV                  1
#define CMD_WAIT_DEV                 2
#define CMD_SLEEP_DEV                3

/*
 * Settings
 */
#define CMD_SLEEP_TIME          60*5 //s
#define CMD_MAX_RETRY               3


void cmd_poll(KFile *fd, Protocol *proto);
void cmd_init(uint8_t addr);

#endif /* RADIO_CMD_H */

