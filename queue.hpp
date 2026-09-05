#pragma once

#include "events.hpp"
#include <memory>


class QueueManager {
public:
    QueueManager();

    void addEvent(std::unique_ptr<irrigationEvent> event);
    
private:
    std::unique_ptr<irrigationEvent> firstEvent;
};
