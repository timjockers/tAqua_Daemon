#pragma once

#include "events.hpp"

#include <deque>
#include <memory>
#include <mutex>


class QueueManager {
public:
    void addEvent(std::unique_ptr<irrigationEvent> event);
    void work();

private:
    std::unique_ptr<irrigationEvent> takeFirstEvent();

    std::deque<std::unique_ptr<irrigationEvent>> events;
    std::unique_ptr<irrigationEvent> activeEvent;

    std::mutex mtx;
};