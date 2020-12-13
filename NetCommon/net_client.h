#pragma once

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"

namespace olc
{
    namespace net
    {
        template <typename T>
        class ClientInterface
        {
        public:

            ClientInterface()
                : m_socket(m_context)
            {
                
            }

            virtual ~ClientInterface()
            {
                Disconnect();
            }

        public:

            bool Connect(const std::string& host, const uint16_t port)
            {
                try
                {
                    m_connection = std::make_unique<Connection<T>>();

                    // resolve the hostname or ip;
                    asio::ip::tcp::resolver resolver(m_context);
                    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                    m_connection-->ConnectToServer(endpoints);

                    m_thread_context = std::thread([this](){ m_context.run(); });

                }
                catch (std::exception& e)
                {
                    std::cerr << "Client exception: " << e.what() << std::endl;
                    return false;
                }

                return false;
            }

            void Disconnect()
            {
                if (IsConnected())
                {
                    m_connection->Disconnect();
                }

                m_context.stop();
                if (m_thread_context.joinable())
                    m_thread_context.join();

                m_connection.release();

            }

            bool IsConnected()
            {
                if (m_connection)
                    return m_connection->IsConnected();
                return false;
            }

            TSQueue<OwnedMessage<T>>& Incoming()
            {
                return m_q_in;
            }

        protected:
            asio::io_context m_context;

            std::thread m_thread_context;

            // hardware socket that is connected to the server;
            asio::ip::tcp::socket m_socket;

            std::unique_ptr<Connection<T>> m_connection;


        private:
            // queue of incoming messages from the server;
            TSQueue<OwnedMessage<T>> m_q_in;

        };
    }
}
