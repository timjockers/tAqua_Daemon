#pragma once

#include "events.hpp"


class QueueManager {
public:
    QueueManager();

    void addEvent(irrigationEvent* event);
    
private:
    irrigationEvent* firstEvent = nullptr;
};
