#include "http_tcpServer.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "request.h"

namespace http {
    TcpServer::TcpServer(std::string ip_addrress, int port) 
    : m_ip_addrress(ip_addrress), m_port(port), m_server_socket(),
      m_client_socket(), m_incoming_messages(), m_socket_addrress(),
      m_socket_addrress_len(sizeof(m_socket_addrress)),
      m_serverMessage(), processor(4)
    {
        m_socket_addrress.sin_family = AF_INET;
        m_socket_addrress.sin_port = htons(m_port);
        m_socket_addrress.sin_addr.s_addr = inet_addr(m_ip_addrress.c_str());

        if (startServer() != 0) {
            std::cout << "failed to start port at: " << ntohs(m_socket_addrress.sin_port) << std::endl;
        }
    }

    TcpServer::~TcpServer() {
        closeServer();
    }

    int TcpServer::startServer() {
        m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_server_socket < 0) {
            std::cout << "Cannot create socket" << std::endl;
            exit(1);
            return 1;
        }

        if (bind(m_server_socket, (sockaddr* ) &m_socket_addrress, m_socket_addrress_len) < 0) {
            std::cout << "Cannot create socket" << std::endl;
            exit(1);
            return 1;
        }


        return 0;
    }

    void TcpServer::closeServer() {
        close(m_server_socket);
        close(m_client_socket);
        exit(0);
    }

    void TcpServer::startListen() {
        if (listen(m_server_socket, 20) < 0) {
            std::cout << "socket listen failed" << std::endl;
            exit(1);
        }

        std::cout << "Listening on addrress " << inet_ntoa(m_socket_addrress.sin_addr) << " port " << ntohs(m_socket_addrress.sin_port) << std::endl;

        int bytesReceieved;

        while (true) {
            std::cout << "Waiting for new connection" << std::endl;

            acceptConnection(m_client_socket);
            char buffer[30720] = {0};
            bytesReceieved = read(m_client_socket, buffer, 30720);
            if (bytesReceieved < 0) {
                std::cout << "Failed to read bytes from client socket connection" << std::endl;
                exit(1);
            }

            std::cout << "Received request from client" << std::endl;

            auto loRequest = std::make_shared<Request>(m_client_socket);
            processor.submitRequest(loRequest);
        }
    }

    void TcpServer::acceptConnection(int & new_socket) {
        new_socket = accept(m_server_socket, (sockaddr*) & m_socket_addrress, &m_socket_addrress_len);
        if (new_socket < 0) {
            std::cout << "Servr failed to accept incoming request from address " << inet_ntoa(m_socket_addrress.sin_addr) << " port " << ntohs(m_socket_addrress.sin_port) << std::endl;
            exit(1);
        }
    }

    std::string TcpServer::buildResponse() {
        std::string htmlFile = "<!DOCTYPE html><html lang=\"eng\"><body><h1>Home</h1><p>Hello from your server! :)</p></body></html>";
        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n" << htmlFile;
        return ss.str();
    }

    void TcpServer::sendResponse() {
        long bytesSent;

        bytesSent = write(m_client_socket, m_serverMessage.c_str(), m_serverMessage.size());

        if (bytesSent == m_serverMessage.size()) {
            std::cout << "Server response sent to client" << std::endl;
        } else {
            std::cout << "Error sending response to client" << std::endl;
        }

        close(m_client_socket);
    }
}