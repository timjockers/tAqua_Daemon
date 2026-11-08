#pragma once

#include <vector>
#include "queue.hpp"
#include "fileexchange.hpp"

class Scheduler {
public:
    explicit Scheduler(QueueManager *queueManager, StatusManager *statusManager);

    void setEventVector(const std::vector<scheduledEvent>& events);
    void work();

private:
    QueueManager *queueM;
    StatusManager *statusM;

    std::vector<scheduledEvent> scheduledEvents;
};
