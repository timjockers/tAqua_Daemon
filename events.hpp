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

    std::string getInfo() override;

protected:
    int relay;
};


class buttonEvent : public relayEvent {
public:
    buttonEvent(int r, std::chrono::seconds irrDuration);

    std::string getInfo() override;
    void activate() override;
    void deactivate() override;
    bool isActive() override;

private:
    std::chrono::seconds duration;
    std::chrono::system_clock::time_point startTime;
};