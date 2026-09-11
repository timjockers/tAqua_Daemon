#pragma once

#include <vector>
#include "queue.hpp"

class Scheduler {
public:
    explicit Scheduler(QueueManager *queueManager);

    void work();

private:
    QueueManager *queueM;

    std::vector<scheduledEvent> scheduledEvents;
};
