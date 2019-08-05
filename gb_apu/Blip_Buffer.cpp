
// Blip_Buffer 0.3.4. http://www.slack.net/~ant/libs/

#include "Blip_Buffer.h"

#include <string>
#include <cstddef>
#include <cmath>
#include <algorithm>

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

Blip_Buffer::Blip_Buffer()
{
	samples_per_sec = 44100;
	
	// try to cause assertion failure if buffer is used before these are set
	clocks_per_sec = 0;
	factor_ = ~0ul;
	offset_ = 0;
	buffer_size_ = 0;
	length_ = 0;
	
	bass_freq_ = 16;
}

void Blip_Buffer::clear( bool entire_buffer )
{
	long count = (entire_buffer ? buffer_size_ : samples_avail());
	offset_ = 0;
	reader_accum = 0;
	if ( !buffer_.empty() )
		std::fill_n(buffer_.begin(), count + widest_impulse_, sample_offset_);
}

blargg_err_t Blip_Buffer::set_sample_rate( long new_rate, int msec )
{
	unsigned new_size = (std::numeric_limits<unsigned>::max() >> BLIP_BUFFER_ACCURACY) + 1 - widest_impulse_ - 64;
	if ( msec != blip_default_length )
	{
		size_t s = (new_rate * (msec + 1) + 999) / 1000;
		if ( s < new_size )
			new_size = s;
		else
			require( false ); // requested buffer length exceeds limit
	}
	
	if ( buffer_size_ != new_size )
	{
		buffer_size_ = 0;
		offset_ = 0;
		
		int const count_clocks_extra = 2;
		buffer_.resize(new_size + widest_impulse_ + count_clocks_extra);
	}
	
	buffer_size_ = new_size;
	length_ = new_size * 1000 / new_rate - 1;
	if ( msec )
		assert( length_ == msec ); // ensure length is same as that passed in
	
	samples_per_sec = new_rate;
	if ( clocks_per_sec )
		clock_rate( clocks_per_sec ); // recalculate factor
	
	bass_freq( bass_freq_ ); // recalculate shift
	
	clear();
	
	return blargg_success;
}

unsigned long Blip_Buffer::clock_rate_factor( long clock_rate ) const
{
	unsigned long factor = (unsigned long) std::floor(
			(double) samples_per_sec / clock_rate * (1L << BLIP_BUFFER_ACCURACY) + 0.5 );
	require( factor > 0 ); // clock_rate/sample_rate ratio is too large
	return factor;
}

void Blip_Buffer::bass_freq( int freq )
{
	bass_freq_ = freq;
	if ( freq == 0 )
	{
		bass_shift = 31; // 32 or greater invokes undefined behavior elsewhere
		return;
	}
	bass_shift = 1 + (int) std::floor( 1.442695041 * std::log( 0.124 * samples_per_sec / freq ) );
	if ( bass_shift < 0 )
		bass_shift = 0;
	if ( bass_shift > 24 )
		bass_shift = 24;
}

void Blip_Buffer::remove_samples( long count )
{
	if ( !count ) // optimization
		return;
	
	remove_silence( count );
	
	// Allows synthesis slightly past time passed to end_frame(), as long as it's
	// not more than an output sample.
	// to do: kind of hacky, could add run_until() which keeps track of extra synthesis
	int const copy_extra = 1;
	
	// copy remaining samples to beginning and clear old samples
	long remain = samples_avail() + widest_impulse_ + copy_extra;
	if ( count >= remain )
		std::move(buffer_.cbegin() + count, buffer_.cbegin() + count + remain, buffer_.begin());
	else
		std::copy_n(buffer_.cbegin() + count, remain, buffer_.begin());
	std::fill_n(buffer_.begin() + remain, count, sample_offset_);
}

long Blip_Buffer::read_samples( int16_t* out, long max_samples, bool stereo )
{
	long count = samples_avail();
	if ( count > max_samples )
		count = max_samples;
	
	if ( !count )
		return 0; // optimization
	
	int sample_offset_ = this->sample_offset_;
	int bass_shift = this->bass_shift;
	auto buf = buffer_.cbegin();
	long accum = reader_accum;
	
	if ( !stereo )
	{
		for ( long n = count; n--; )
		{
			long s = accum >> accum_fract;
			accum -= accum >> bass_shift;
			accum += (long (*buf++) - sample_offset_) << accum_fract;
			*out++ = (int16_t) s;
			
			// clamp sample
			if ( (int16_t) s != s )
				out [-1] = int16_t (0x7FFF - (s >> 24));
		}
	}
	else
	{
		for ( long n = count; n--; )
		{
			long s = accum >> accum_fract;
			accum -= accum >> bass_shift;
			accum += (long (*buf++) - sample_offset_) << accum_fract;
			*out = (int16_t) s;
			out += 2;
			
			// clamp sample
			if ( (int16_t) s != s )
				out [-2] = int16_t (0x7FFF - (s >> 24));
		}
	}
	
	reader_accum = accum;
	
	remove_samples( count );
	
	return count;
}
