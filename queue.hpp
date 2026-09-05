#pragma once

#include "events.hpp"

#include <deque>
#include <memory>

class QueueManager {
public:
    void addEvent(std::unique_ptr<irrigationEvent> event);
    std::unique_ptr<irrigationEvent> takeFirstEvent();

private:
    std::deque<std::unique_ptr<irrigationEvent>> events;
};