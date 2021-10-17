/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2011-2014  Intel Corporation
 *  Copyright (C) 2002-2010  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdint.h>
#include <stdbool.h>

struct l2cap_frame {
	uint16_t index;
	bool in;
	uint16_t handle;
	uint16_t cid;
	uint16_t psm;
	uint16_t chan;
	uint8_t mode;
	const void *data;
	uint16_t size;
};

static inline void l2cap_frame_pull(struct l2cap_frame *frame,
				const struct l2cap_frame *source, uint16_t len)
{
	frame->index   = source->index;
	frame->in      = source->in;
	frame->handle  = source->handle;
	frame->cid     = source->cid;
	frame->psm     = source->psm;
	frame->chan    = source->chan;
	frame->mode    = source->mode;
	frame->data    = source->data + len;
	frame->size    = source->size - len;
}

void l2cap_packet(uint16_t index, bool in, uint16_t handle, uint8_t flags,
					const void *data, uint16_t size);
