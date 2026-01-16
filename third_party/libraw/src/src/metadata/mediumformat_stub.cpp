/* Stub functions for LibRaw - replaces problematic mediumformat.cpp */

#define LIBRAW_LIBRARY_BUILD
#include "libraw/libraw.h"
#include "internal/dcraw_defs.h"

void LibRaw::parse_phase_one(int base)
{
    (void)base;
}

void LibRaw::parse_mos(INT64 offset)
{
    (void)offset;
}
