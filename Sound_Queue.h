
// Simple sound queue for synchronous sound handling in SDL

// Copyright (C) 2005 Shay Green. MIT license.

#pragma once

#include <SDL.h>

#include <vector>

// Simple SDL sound wrapper that has a synchronous interface
class Sound_Queue {
public:
	Sound_Queue();
	~Sound_Queue();
	
	// Initialize with specified sample rate and channel count.
	// Returns NULL on success, otherwise error string.
	const char* start( long sample_rate, int chan_count = 1 );
	
	// Number of samples in buffer waiting to be played
	int sample_count() const;
	
	// Write samples to buffer and block until enough space is available
	void write( const int16_t*, int count );
	
	// Stop audio output
	void stop();
	
private:
	enum { buf_size = 4096 };
	enum { buf_count = 3 };
	std::vector<int16_t> bufs;
	SDL_sem* volatile free_sem;
	int16_t* volatile currently_playing_;
	int volatile read_buf;
	int write_buf;
	int write_pos;
	bool sound_open;
	
	int16_t* buf( int index );
	void fill_buffer( Uint8*, int );
	static void fill_buffer_( void*, Uint8*, int );
};
