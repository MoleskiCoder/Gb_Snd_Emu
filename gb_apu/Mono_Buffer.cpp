#include "Mono_Buffer.h"

/* Copyright (C) 2003-2005 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details. You should have received a copy of the GNU Lesser General
Public License along with this module; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

#include BLARGG_SOURCE_BEGIN

Mono_Buffer::Mono_Buffer() : Multi_Buffer(1) {
}

Mono_Buffer::~Mono_Buffer() {
}

blargg_err_t Mono_Buffer::set_sample_rate(long rate, int msec) {
	BLARGG_RETURN_ERR(buf.set_sample_rate(rate, msec));
	return Multi_Buffer::set_sample_rate(buf.sample_rate(), buf.length());
}

// Mono_Buffer

Mono_Buffer::channel_t Mono_Buffer::channel(int index) {
	channel_t ch;
	ch.center = &buf;
	ch.left = &buf;
	ch.right = &buf;
	return ch;
}

void Mono_Buffer::end_frame(long t, bool) {
	buf.end_frame(t);
}

