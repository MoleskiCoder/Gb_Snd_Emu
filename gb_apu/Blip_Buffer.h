
// Buffer of sound samples into which band-limited waveforms can be synthesized
// using Blip_Wave or Blip_Synth.

// Blip_Buffer 0.3.4. Copyright (C) 2003-2005 Shay Green. GNU LGPL license.

#pragma once

#include "blargg_common.h"
#include "blip_eq_t.h"

#include <vector>

class Blip_Buffer {
public:
	// Make buffer as large as possible (currently about 65000 samples)
	enum { blip_default_length = 0 };
	enum { BLIP_BUFFER_ACCURACY = 16 };
	enum { blip_res_bits_ = 5 };

	static constexpr int max_res = 1 << Blip_Buffer::blip_res_bits_;

	// Construct an empty buffer.
	Blip_Buffer();
	
	// Set output sample rate and buffer length in milliseconds (1/1000 sec),
	// then clear buffer. If length is not specified, make as large as possible.
	// If there is insufficient memory for the buffer, sets the buffer length
	// to 0 and returns error string (or propagates exception if compiler supports it).
	blargg_err_t set_sample_rate( long samples_per_sec, int msec_length = blip_default_length );
	
	// Length of buffer, in milliseconds
	int length() const {
		return length_;
	}
	
	// Current output sample rate
	long sample_rate() const {
		return samples_per_sec;
	}
	
	// Number of source time units per second
	void clock_rate(long cps) {
		clocks_per_sec = cps;
		factor_ = clock_rate_factor(cps);
	}

	long clock_rate() const {
		return clocks_per_sec;
	}
	
	// Set frequency at which high-pass filter attenuation passes -3dB
	void bass_freq( int frequency );
	
	// Remove all available samples and clear buffer to silence. If 'entire_buffer' is
	// false, just clear out any samples waiting rather than the entire buffer.
	void clear( bool entire_buffer = true );
	
	// End current time frame of specified duration and make its samples available
	// (along with any still-unread samples) for reading with read_samples(). Begin
	// a new time frame at the end of the current frame. All transitions must have
	// been added before 'time'.
	void end_frame( long t ) {
		offset_ += t * factor_;
		assert(("Blip_Buffer::end_frame(): Frame went past end of buffer",
			samples_avail() <= (long)buffer_size_));
	}
	
	// Number of samples available for reading with read_samples()
	long samples_avail() const {
		return long(offset_ >> BLIP_BUFFER_ACCURACY);
	}
	
	// Remove 'count' samples from those waiting to be read
	void remove_samples( long count );
	
	// Number of samples delay from synthesis to samples read out
	int output_latency() const {
		return widest_impulse_ / 2;
	}
	
	// not documented yet
	
	void remove_silence( long count ) {
		assert(("Blip_Buffer::remove_silence(): Tried to remove more samples than available",
			count <= samples_avail()));
		offset_ -= unsigned long(count) << BLIP_BUFFER_ACCURACY;
	}
	
	unsigned long resampled_time( long t ) const
	{
		return t * unsigned long (factor_) + offset_;
	}
	
	unsigned long clock_rate_factor( long clock_rate ) const;
	
	unsigned long resampled_duration( int t ) const
	{
		return t * unsigned long (factor_);
	}
	
private:
	// noncopyable
	Blip_Buffer( const Blip_Buffer& );
	Blip_Buffer& operator = ( const Blip_Buffer& );

	// Don't use the following members. They are public only for technical reasons.
	public:
		enum { sample_offset_ = 0x7F7F }; // repeated byte allows memset to clear buffer
		enum { widest_impulse_ = 24 };
		
		unsigned long factor_;
		unsigned long offset_;
		std::vector<uint16_t> buffer_;
		unsigned buffer_size_;
	private:
		long reader_accum;
		int bass_shift;
		long samples_per_sec;
		long clocks_per_sec;
		int bass_freq_;
		int length_;
		
		enum { accum_fract = 15 }; // less than 16 to give extra sample range
		
		friend class Blip_Reader;
};

// End of public interface

#include "Blip_Synth.h"
