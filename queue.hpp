#pragma once

#include "events.hpp"

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>


class QueueManager {
public:
    void addEvent(std::unique_ptr<irrigationEvent> event);
    void work();
    void stop();

private:
    std::unique_ptr<irrigationEvent> takeFirstEventUnlocked();

    std::deque<std::unique_ptr<irrigationEvent>> events;
    std::unique_ptr<irrigationEvent> activeEvent;

    std::mutex mtx;
    std::condition_variable condition;
    bool stopping = false;
};