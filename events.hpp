#pragma once

#include <string>
#include <chrono>
#include <memory>
#include "types.hpp"
#include "io.hpp"

class irrigationEvent {
public:
    virtual ~irrigationEvent() = default;

    virtual std::string getInfo();

    virtual void activate(ioManager* io);
    virtual void deactivate(ioManager *io);
    virtual bool isActive();
};


class relayEvent : public irrigationEvent {
public:
    relayEvent(Relay r);

    std::string getInfo() override;
    Relay getRelay() const;

    void activate(ioManager* io) override;
    void deactivate(ioManager* io) override;

protected:
    Relay relay;
};


class durationEvent : public relayEvent {
public:
    durationEvent(Relay r, std::chrono::seconds irrDuration);

    std::string getInfo() override;
    void activate(ioManager* io) override;
    bool isActive() override;

private:
    std::chrono::seconds duration;
    std::chrono::system_clock::time_point startTime;
};


class scheduledEvent : public durationEvent {
public:
    scheduledEvent(Relay r, std::chrono::seconds irrDuration, Weekday weekday, std::chrono::minutes startTime);

    std::string getInfo() override;

    bool matchesSchedule();
private:
    Weekday wday;
    std::chrono::minutes starttime; // Minutes past midnight
    int lastQueuedDayKey = -1; // Verifies that an event can only be added once per day
};
