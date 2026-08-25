#pragma once

#include <string>
#include <chrono>

class irrigationEvent {
public:
    virtual std::string getInfo();

    virtual void activate();
    virtual void deactivate();
    virtual bool isActive();
};

class relayEvent : public irrigationEvent {
public:
    relayEvent(int r);

    virtual std::string getInfo() override;

protected:
    int relay;
};


class buttonEvent : public relayEvent {
public:
    buttonEvent(int r, std::chrono::seconds irrDuration);

protected:
    std::chrono::seconds duration;
};