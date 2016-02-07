// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t COLOR_PIXEL_SHADER_OGL[] =
    "#version 330\n"
    "in vec4 ex_Color;\n"
    "out vec4 out_Color;\n"
    "void main(void)\n"
    "{\n"
    "    out_Color = ex_Color;\n"
    "}";
