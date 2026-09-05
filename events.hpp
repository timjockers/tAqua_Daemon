#pragma once

#include <string>
#include <chrono>
#include <memory>
#include "types.hpp"

class irrigationEvent {
public:
    virtual ~irrigationEvent() = default;

    irrigationEvent* getNextEvent() const;
    std::unique_ptr<irrigationEvent> takeNextEvent();

    virtual std::string getInfo();

    virtual void activate();
    virtual void deactivate();
    virtual bool isActive();

private:
    std::unique_ptr<irrigationEvent> nextEvent;

    friend class QueueManager;
};


class relayEvent : public irrigationEvent {
public:
    relayEvent(Relay r);

    std::string getInfo() override;

protected:
    Relay relay;
};


class buttonEvent : public relayEvent {
public:
    buttonEvent(Relay r, std::chrono::seconds irrDuration);

    std::string getInfo() override;
    void activate() override;
    void deactivate() override;
    bool isActive() override;

private:
    std::chrono::seconds duration;
    std::chrono::system_clock::time_point startTime;
};