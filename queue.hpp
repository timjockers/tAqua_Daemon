#pragma once

#include "events.hpp"
#include "io.hpp"

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <string>


class QueueManager {
public:
    explicit QueueManager(ioManager& io);

    void addEvent(std::unique_ptr<irrigationEvent> event);
    void work();
    void stop();

    std::string getQueueInfo();

private:
    ioManager& ioM;

    std::unique_ptr<irrigationEvent> takeFirstEventUnlocked();

    std::deque<std::unique_ptr<irrigationEvent>> events;
    std::unique_ptr<irrigationEvent> activeEvent;

    std::mutex mtx;
    std::condition_variable condition;
    bool stopping = false;
};