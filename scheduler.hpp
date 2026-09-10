#pragma once

#include <vector>
#include "queue.hpp"

class Scheduler {
public:
    explicit Scheduler(QueueManager *queueManager);

private:
    QueueManager *queueM;

    // std::vector<> scheduledEvents;
};
