#include "request_processor.h"
#include "request.h"
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <sstream>

RequestProcessor::RequestProcessor(int riMaxThreads): m_requests() {
    giMaxThreads = riMaxThreads;
    for (int i = 0; i < riMaxThreads; i++) {
        m_processors.emplace_back(&RequestProcessor::handleRequest, this);
    }
}

RequestProcessor::~RequestProcessor() {
    m_cv.notify_all();
    for (auto & t: m_processors) {
        t.join();
    }
}

void RequestProcessor::submitRequest(const std::shared_ptr<Request> &roReq) {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    if (m_requests.size() == giMaxThreads) {
        std::cout << "Max thread limit reached" << std::endl;
        return;
    }

    m_requests.push(std::move(roReq));
    m_cv.notify_one();
}

void RequestProcessor::handleRequest() {
    std::shared_ptr<Request> request;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_requests.empty();});

        request = std::move(m_requests.front());
        m_requests.pop();
    }
    processRequest(request);
}

void RequestProcessor::processRequest(std::shared_ptr<Request> roReq) {
    //processing request
    std::string htmlFile = "<!DOCTYPE html><html lang=\"eng\"><body><h1>Home</h1><p>Hello from your server! :)</p></body></html>";
    std::ostringstream ss;
    ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n" << htmlFile;

    std::string response = ss.str();

    long bytesSent;

    bytesSent = write(roReq->m_request_socket, response.c_str(), response.size());

    if (bytesSent == response.size()) {
        std::cout << "Server response sent to client" << std::endl;
    } else {
        std::cout << "Error sending response to client" << std::endl;
    }

    close(roReq->m_request_socket);
}