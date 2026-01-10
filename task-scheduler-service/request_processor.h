#pragma once
#include "request.h"
#include <memory>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sstream>

class Request;

class RequestProcessor {
public:
    std::queue<std::shared_ptr<Request>> m_requests;
    std::vector<std::thread> m_processors;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    int giMaxThreads;

    RequestProcessor(int riMaxThreads);
    ~RequestProcessor();

    void processRequest(std::shared_ptr<Request> roReq);
    void submitRequest(const std::shared_ptr<Request> &roReq);
    void handleRequest();
};