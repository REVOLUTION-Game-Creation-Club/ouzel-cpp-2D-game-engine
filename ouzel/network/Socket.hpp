// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SOCKET_HPP
#define OUZEL_SOCKET_HPP

#ifdef _WIN32
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock.h>
#endif

namespace ouzel
{
    namespace network
    {
        class Socket final
        {
        public:
            Socket();
            ~Socket();

            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;

            Socket(Socket&& other);
            Socket& operator=(Socket&& other);

        private:
#ifdef _WIN32
            SOCKET endpoint = INVALID_SOCKET;
#else
            int endpoint = -1;
#endif
        };
    } // namespace network
} // namespace ouzel

#endif // OUZEL_SOCKET_HPP
