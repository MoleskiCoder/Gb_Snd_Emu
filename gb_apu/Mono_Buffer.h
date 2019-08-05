#pragma once

#include "Multi_Buffer.h"

// Uses a single buffer and outputs mono samples.
class Mono_Buffer : public Multi_Buffer {
	Blip_Buffer buf;
public:
	Mono_Buffer();
	~Mono_Buffer();

	// Buffer used for all channels
	Blip_Buffer* center() { return &buf; }

	// See Multi_Buffer
	blargg_err_t set_sample_rate(long rate, int msec = Blip_Buffer::blip_default_length);
	void clock_rate(long rate) { buf.clock_rate(rate); }
	void bass_freq(int freq) { buf.bass_freq(freq); }
	void clear() { buf.clear(); }
	channel_t channel(int);
	void end_frame(long, bool unused = true);
	long samples_avail() const { return buf.samples_avail(); }
};
