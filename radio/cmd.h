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
 * Copyright 2013 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Radio commands interface.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef RADIO_CMD_H
#define RADIO_CMD_H

#include "protocol.h"

#include <io/kfile.h>

#include <struct/list.h>

/*
 * Message type
 */
#define CMD_BROADCAST        0xFF
#define CMD_WAIT             0x1
#define CMD_SLEEP            0x2
#define CMD_DATA             0x3

/*
 * Device and slave status
 */
#define CMD_NEW_DEV                  1
#define CMD_WAIT_DEV                 2

#define CMD_SLAVE_STATUS_BROADCAST   0
#define CMD_SLAVE_STATUS_WAIT        1
#define CMD_SLAVE_STATUS_DATA        2
#define CMD_SLAVE_STATUS_SHUTDOWN    3

/*
 * Settings
 */
#define CMD_DEVICES                  5
#define CMD_TIMEOUT               5000 //ms

struct Protocol;
typedef int (*cmd_t)(KFile *fd, struct Protocol *proto);

typedef struct Cmd
{
	uint8_t id;
	cmd_t callback;
} Cmd;

typedef struct Devices
{
	uint8_t addr;
	uint8_t status;
	ticks_t timeout;
} Devices;



extern const Cmd master_cmd[];
extern const Cmd slave_cmd[];

//void cmd_init();
void cmd_poll(KFile *fd, struct Protocol *proto);
void cmd_slavePoll(KFile *fd, struct Protocol *proto);

#endif /* RADIO_CMD_H */

