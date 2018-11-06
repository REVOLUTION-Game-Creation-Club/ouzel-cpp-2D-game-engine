// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_KEYBOARDDEVICEWIN_HPP
#define OUZEL_KEYBOARDDEVICEWIN_HPP

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceWin final: public KeyboardDevice
        {
        public:
            KeyboardDeviceWin(InputSystem& initInputSystem,
                              uint32_t initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }

            std::future<bool> handleKeyPress(Keyboard::Key key);
            void update();

        private:
            bool leftShiftDown = false;
            bool rightShiftDown = false;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_KEYBOARDDEVICEWIN_HPP
