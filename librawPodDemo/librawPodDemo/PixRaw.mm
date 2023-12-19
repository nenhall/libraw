//
//  PixRaw.m
//  librawPodDemo
//
//  Created by meitu@nenhall on 2023/12/18.
//

#import "PixRaw.h"
#include <libraw/internal/libraw.h>

@implementation PixRaw

- (instancetype)init
{
    self = [super init];
    if (self) {
        LibRaw *raw = new LibRaw();
        raw->open_file([@"" UTF8String]);
        libraw_init(0);
        NSLog(@"PixRaw init");
    }
    return self;
}

@end
