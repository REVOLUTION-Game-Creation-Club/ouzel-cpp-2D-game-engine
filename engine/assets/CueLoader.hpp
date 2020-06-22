// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_CUELOADER_HPP
#define OUZEL_ASSETS_CUELOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class CueLoader final: public Loader
    {
    public:
        explicit CueLoader(Cache& initCache);
        bool loadAsset(Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       bool mipmaps = true) final;
    };
}

#endif // OUZEL_ASSETS_CUELOADER_HPP
