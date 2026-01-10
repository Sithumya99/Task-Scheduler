#ifndef INCLUDED_HTTP_TCPSERVER
#define INCLUDED_HTTP_TCPSERVER

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include "request_processor.h"

namespace http {
    class TcpServer {
        public:
            TcpServer(std::string ip_addrress, int port);
            ~TcpServer();

            int startServer();
            void closeServer();
            void startListen();
            void acceptConnection(int & new_socket);
            void sendResponse();
            std::string buildResponse();
        private:
            std::string m_ip_addrress;
            int m_port;
            int m_server_socket;
            int m_client_socket;
            long m_incoming_messages;
            struct sockaddr_in m_socket_addrress;
            unsigned int m_socket_addrress_len;
            std::string m_serverMessage;
            RequestProcessor processor;
    };
}
#endif