#pragma once

#include <vector>

#include "Multi_Buffer.h"

// Uses three buffers (one for center) and outputs stereo sample pairs.
class Stereo_Buffer : public Multi_Buffer {
public:
	Stereo_Buffer();
	~Stereo_Buffer();

	// Buffers used for all channels
	Blip_Buffer* center() { return &bufs[0]; }
	Blip_Buffer* left() { return &bufs[1]; }
	Blip_Buffer* right() { return &bufs[2]; }

	// See Multi_Buffer
	blargg_err_t set_sample_rate(long, int msec = Blip_Buffer::blip_default_length);
	void clock_rate(long);
	void bass_freq(int);
	void clear();
	channel_t channel(int index) { return chan; }
	void end_frame(long, bool added_stereo = true);

	long samples_avail() const { return bufs[0].samples_avail() * 2; }
	long read_samples(std::vector<int16_t>&);

private:
	enum { buf_count = 3 };
	Blip_Buffer bufs[buf_count];
	channel_t chan;
	bool stereo_added;
	bool was_stereo;

	void mix_stereo(std::vector<int16_t>& out);
	void mix_mono(std::vector<int16_t>& out);
};
