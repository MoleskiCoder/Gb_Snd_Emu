#pragma once

#include "blip_eq_t.h"

#include <cstdint>

class Blip_Buffer;

class Blip_Impulse_ {
	blip_eq_t eq;
	double  volume_unit_;
	uint16_t*  impulses;
	uint16_t*  impulse;
	int     width;
	int     fine_bits;
	int     res;
	bool    generate;

	void fine_volume_unit();
	void scale_impulse(int unit, uint16_t*) const;
public:
	Blip_Buffer*    buf;
	uint32_t offset;

	void init(uint32_t* impulses, int width, int res, int fine_bits = 0);
	void volume_unit(double);
	void treble_eq(const blip_eq_t&);
};