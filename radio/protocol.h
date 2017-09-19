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
 * \brief Radio protocol interface.
 *
 * \author Daniele Basile <asterix24@gmail.com>
 *
 */

#ifndef RADIO_PROTOCOL_H
#define RADIO_PROTOCOL_H

#include "radio_cc1101.h"

#include <io/kfile.h>

#include <cpu/types.h>

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
#define CMD_SLEEP_TIME            24*60   //s
#define CMD_RETRY_TIME                500 //ms
#define CMD_MAX_RETRY                 3

/* Protocol constant define */
#define PROTO_ACK    0x06
#define PROTO_NACK   0x15

typedef struct PACKED Protocol
{
	uint8_t len;
	uint8_t addr;
	uint32_t timestamp;
} Protocol;

int protocol_encode(uint8_t id, uint8_t cfg, Protocol *prt, uint8_t *data, size_t len);
int protocol_decode(Radio *fd, Protocol *prt, uint8_t *data, size_t len);
int protocol_decodeReply(Protocol *prt, uint8_t *buf,size_t buf_len, uint8_t *data, size_t len);
int protocol_encodeReply(uint8_t id, Protocol *prt, uint8_t *buf,size_t buf_len, uint8_t *data, size_t len);

#endif /* RADIO_PROTOCOL_H */

