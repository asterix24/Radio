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

#include <struct/list.h>

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

#define CMD_SLAVE_STATUS_BROADCAST       0
#define CMD_SLAVE_STATUS_WAIT            1
#define CMD_SLAVE_STATUS_DATA            2
#define CMD_SLAVE_STATUS_SHUTDOWN        3

/*
 * Settings
 */
#define CMD_DEVICES                  5
#define CMD_TIMEOUT                 60 //s
#define CMD_WAKEUP_TIME             60 //s
#define CMD_MAX_RETRY                3

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
	uint32_t timestamp;
	uint32_t local_timestamp;
} Devices;



extern const Cmd master_cmd[];
extern const Cmd slave_cmd[];

void cmd_poll(uint8_t id, KFile *fd, struct Protocol *proto);
void cmd_init(void);

#endif /* RADIO_CMD_H */

