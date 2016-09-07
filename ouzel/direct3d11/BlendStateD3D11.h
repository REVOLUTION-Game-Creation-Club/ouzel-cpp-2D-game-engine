// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include <atomic>
#define NOMINMAX
#include <d3d11.h>
#include "graphics/BlendState.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;

        class BlendStateD3D11: public BlendState
        {
            friend RendererD3D11;
        public:
            virtual ~BlendStateD3D11();
            virtual void free() override;

            virtual bool init(bool enableBlending,
                              BlendFactor colorBlendSource, BlendFactor colorBlendDest,
                              BlendOperation colorOperation,
                              BlendFactor alphaBlendSource, BlendFactor alphaBlendDest,
                              BlendOperation alphaOperation) override;

            ID3D11BlendState* getBlendState() const { return blendState; }

        protected:
            BlendStateD3D11();
            virtual bool upload() override;

            ID3D11BlendState* blendState = nullptr;

            std::atomic<bool> dirty;
        };
    } // namespace graphics
} // namespace ouzel
