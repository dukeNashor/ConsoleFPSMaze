#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"

namespace olc
{
    namespace net
    {
        template <typename T>
        class Connection : public std::enable_shared_from_this<Connection<T>>
        {
        public:
            Connection()
            {
                
            }

            virtual ~Connection()
            {
                
            }

        public:
            bool ConnectToServer();
            bool Disconnect();
            bool IsConnected() const;

        public:
            bool Send(const Message<T>& msg);

        protected:
            // each connection has a unique socket to remote;
            asio::ip::tcp::socket m_socket;

            // This context is shared with the whole asio instance;
            asio::io_context& m_io_context;

            // queue that holds messages to be sent to the remote side of this connection;
            TSQueue<Message<T>> m_q_out;

            // queue that holds messages received from remote side.
            // it is a reference, since the owner of the connection is expected to provide a queue.
            TSQueue<Message<T>>& m_q_in;

        };


    }
}
