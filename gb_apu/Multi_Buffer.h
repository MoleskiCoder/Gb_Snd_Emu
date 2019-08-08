
// Multi-channel sound buffer interface, and basic mono and stereo buffers

// Blip_Buffer 0.3.4. Copyright (C) 2003-2005 Shay Green. GNU LGPL license.

#pragma once

#include <vector>

#include "Blip_Buffer.h"

// Interface to one or more Blip_Buffers mapped to one or more channels
// consisting of left, center, and right buffers.
class Multi_Buffer {
public:
	Multi_Buffer( int samples_per_frame );
	virtual ~Multi_Buffer() { }
	
	// Get indexed channel, from 0 to channel count - 1
	struct channel_t {
		Blip_Buffer* center;
		Blip_Buffer* left;
		Blip_Buffer* right;
	};
	virtual channel_t channel( int index ) = 0;
	
	// See Blip_Buffer.h
	virtual void set_sample_rate(long rate, int msec = Blip_Buffer::blip_default_length) {
		sample_rate_ = rate;
		length_ = msec;
	}

	virtual void clock_rate( long ) = 0;
	virtual void bass_freq( int ) = 0;
	virtual void clear() = 0;
	long sample_rate() const { return sample_rate_; }
	
	// Length of buffer, in milliseconds
	int length() const { return length_; }
	
	// See Blip_Buffer.h. For optimal operation, pass false for 'added_stereo'
	// if nothing was added to the left and right buffers of any channel for
	// this time frame.
	virtual void end_frame( long, bool added_stereo = true ) = 0;
	
	// Number of samples per output frame (1 = mono, 2 = stereo)
	int samples_per_frame() const { return samples_per_frame_; }
	
	// Count of changes to channel configuration. Incremented whenever
	// a change is made to any of the Blip_Buffers for any channel.
	unsigned channels_changed_count() { return channels_changed_count_; }
	
	// See Blip_Buffer.h
	virtual long read_samples( std::vector<int16_t>& ) = 0;
	virtual long samples_avail() const = 0;
	
protected:
	void channels_changed() { channels_changed_count_++; }
private:
	// noncopyable
	Multi_Buffer( const Multi_Buffer& );
	Multi_Buffer& operator = ( const Multi_Buffer& );
	
	unsigned channels_changed_count_;
	long sample_rate_;
	int length_;
	int const samples_per_frame_;
};


// End of public interface