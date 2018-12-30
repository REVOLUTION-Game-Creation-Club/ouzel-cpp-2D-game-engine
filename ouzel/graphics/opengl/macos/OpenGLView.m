// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OpenGLView.h"

@implementation OpenGLView

-(void)setContext:(NSOpenGLContext*)context
{
    openGLContext = context;
}

-(void)setLayer:(CALayer*)layer
{
    [super setLayer:layer];

    [openGLContext update];
}

@end

#endif
