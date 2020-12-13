#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"


namespace olc
{
    namespace net
    {
        template <typename T>
        class ServerInterface
        {
        public:
            ServerInterface(uint16_t port)
            {
                
            }

            virtual ~ServerInterface()
            {
                
            }


        };

    }
}
