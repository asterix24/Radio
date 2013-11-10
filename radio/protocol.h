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
 * \brief Radio protocol interface.
 *
 * Simple protocol to manage master and radio slave. The low level of protocol is
 * managed from the radio ic, that compute crc, and manage start and stop of package.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef RADIO_PROTOCOL_H
#define RADIO_PROTOCOL_H

#include "cmd.h"
#include "radio_cc1101.h"

#include <io/kfile.h>

#include <cpu/types.h>

/* Settings */
#define PROTO_DATALEN  (RADIO_MAXPAYLOAD_LEN - 4) // See Protocol structure, we remove the other fields.

/* Protocol constant define */
#define PROTO_ACK    0x06
#define PROTO_NACK   0x15

/* Protocol errors */
#define PROTO_OK             0
#define PROTO_ERR           -1
#define PROTO_WRONG_ADDR    -2
#define PROTO_TIMEOUT       -3

/*
 * Protocol data format.
 * The data should be sent in packed format, the allow type are:
 *
 * c: char 8bit
 * b: signed char 8bit
 * B: unsigned char 8bit
 * h: short 16bit
 * H: unsigned short 16bit
 * i: int 32bit
 * I: unsigned int 32bit
 * ...
 */

typedef struct Protocol
{
	uint8_t type;
	uint8_t addr;
	uint16_t len;
	uint8_t data[PROTO_DATALEN];
} Protocol;

int protocol_send(KFile *fd, Protocol *proto, uint8_t addr, uint8_t type, const uint8_t *data, size_t len);
void protocol_decode(Radio *fd, Protocol *proto);
void protocol_encode(Protocol *proto, uint8_t *buf, size_t len);
int protocol_poll(KFile *fd, Protocol *proto);
void protocol_init(const Cmd *table);

INLINE int protocol_broadcast(KFile *fd, Protocol *proto, uint8_t addr, const uint8_t *data, size_t len)
{
	return protocol_send(fd, proto, addr, CMD_BROADCAST, data, len);
}

#endif /* RADIO_PROTOCOL_H */

