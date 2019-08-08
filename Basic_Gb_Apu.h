
// Simplified Nintendo Game Boy PAPU sound chip emulator

// Gb_Snd_Emu 0.1.4. Copyright (C) 2003-2005 Shay Green. GNU LGPL license.

#pragma once

#include "gb_apu/Gb_Apu.h"
#include "gb_apu/Stereo_Buffer.h"

#include <vector>

class Basic_Gb_Apu {
public:
	Basic_Gb_Apu();
	~Basic_Gb_Apu();
	
	// Set output sample rate
	void set_sample_rate( long rate );
	
	// Pass reads and writes in the range 0xff10-0xff3f
	void write_register( unsigned, int data );
	int read_register( unsigned );
	
	// End a 1/60 sound frame and add samples to buffer
	void end_frame();
	
	// Samples are generated in stereo, left first. Sample counts are always
	// a multiple of 2.
	
	// Number of samples in buffer
	long samples_avail() const;
	
	// Read at most 'count' samples out of buffer and return number actually read
	long read_samples( std::vector<int16_t>& out );
	
private:
	Gb_Apu apu;
	Stereo_Buffer buf;
	long time;
	
	// faked CPU timing
	long clock() { return time += 4; }
};
