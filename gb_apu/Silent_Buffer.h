#pragma once

#include "Multi_Buffer.h"

// Silent_Buffer generates no samples, useful where no sound is wanted
class Silent_Buffer : public Multi_Buffer {
	channel_t chan;
public:
	Silent_Buffer();

	blargg_err_t set_sample_rate(long rate, int msec = Blip_Buffer::blip_default_length) {
		return Multi_Buffer::set_sample_rate(rate, msec);
	}

	void clock_rate(long) { }
	void bass_freq(int) { }
	void clear() { }
	channel_t channel(int) { return chan; }
	void end_frame(long, bool unused = true) { }
	long samples_avail() const { return 0; }
	long read_samples(int16_t*, long) { return 0; }
};

