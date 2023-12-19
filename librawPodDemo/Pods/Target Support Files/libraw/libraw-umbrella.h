#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#else
#ifndef FOUNDATION_EXPORT
#if defined(__cplusplus)
#define FOUNDATION_EXPORT extern "C"
#else
#define FOUNDATION_EXPORT extern
#endif
#endif
#endif

#import "internal/libraw.h"
#import "internal/libraw_alloc.h"
#import "internal/libraw_const.h"
#import "internal/libraw_datastream.h"
#import "internal/libraw_internal.h"
#import "internal/libraw_types.h"
#import "internal/libraw_version.h"

FOUNDATION_EXPORT double internalVersionNumber;
FOUNDATION_EXPORT const unsigned char internalVersionString[];

