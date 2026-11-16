#pragma once

#include <vector>
#include "queue.hpp"
#include "fileexchange.hpp"

class Scheduler {
public:
    explicit Scheduler(QueueManager& queueManager, StatusManager& statusManager);

    void setEventVector(const std::vector<scheduledEvent>& events);
    void work();

private:
    QueueManager& queueM;
    StatusManager& statusM;

    std::vector<scheduledEvent> scheduledEvents;

    void updateStatus();

    bool minute_passed; // An event can't be added to queue when it's scheduled in the same minute (avoiding multiple scheduling)
    int last_minute;
};
