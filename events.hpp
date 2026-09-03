#pragma once

#include <string>
#include <chrono>
#include "types.hpp"

class irrigationEvent {
public:
    virtual std::string getInfo();

    virtual void activate();
    virtual void deactivate();
    virtual bool isActive();
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