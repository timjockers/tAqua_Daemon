#pragma once

#include "events.hpp"
#include "io.hpp"
#include "fileexchange.hpp"

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <string>


class QueueManager {
public:
    explicit QueueManager(ioManager& io, LogManager& l, StatusManager& s);

    void addEvent(std::unique_ptr<irrigationEvent> event);
    bool containsButtonEvent(Relay relay) const;
    void removeQueuedButtonEvent(Relay relay);
    void cancelActiveEvent();
    void work();
    void stop();

    std::string getQueueInfo();

private:
    ioManager& ioM;
    LogManager& logM;
    StatusManager& statusM;

    std::string getQueueInfoUnlocked();
    bool containsButtonEventUnlocked(Relay relay) const;
    void refreshYLEDsUnlocked();
    std::unique_ptr<irrigationEvent> takeFirstEventUnlocked();

    std::deque<std::unique_ptr<irrigationEvent>> events;
    std::unique_ptr<irrigationEvent> activeEvent;

    mutable std::mutex mtx;
    std::condition_variable condition;
    bool stopping = false;
};