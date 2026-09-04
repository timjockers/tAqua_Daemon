#pragma once

#include "events.hpp"


class QueueManager {
public:
    QueueManager();
    
private:
    irrigationEvent* firstEvent = nullptr;
};
