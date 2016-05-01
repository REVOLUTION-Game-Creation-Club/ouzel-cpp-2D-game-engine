// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "Noncopyable.h"
#include "Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Texture: public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~Texture();

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true);
            virtual bool initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps = true);
            virtual bool initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps = true);

            const std::string& getFilename() const { return filename; }

            virtual bool upload(const void* data, const Size2& newSize);
            virtual bool uploadMipmap(uint32_t level, const void* data);

            const Size2& getSize() const { return size; }

            bool isDynamic() const { return dynamic; }

        protected:
            Texture();

            virtual bool uploadData(const void* data, const Size2& newSize);

            std::string filename;

            Size2 size;
            bool dynamic = false;
            bool mipmaps = false;
            std::vector<Size2> mipmapSizes;
        };
    } // namespace graphics
} // namespace ouzel
