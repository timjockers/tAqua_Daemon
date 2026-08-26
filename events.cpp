#include "events.hpp"

#include <iostream>

using namespace std;

string irrigationEvent::getInfo()
{
    return "IrrigationEvent";
}

void irrigationEvent::activate() {}

void irrigationEvent::deactivate() {}

bool irrigationEvent::isActive()
{
    return false;
}



relayEvent::relayEvent(int r)
    : relay(r)
{}

string relayEvent::getInfo()
{
    return "RelayEvent: R" + to_string(relay);
}



buttonEvent::buttonEvent(int r, chrono::seconds irrDuration)
    : relayEvent(r), duration(irrDuration)
{}

void buttonEvent::activate()
{
    relayEvent::activate();

    startTime = chrono::system_clock::now();
}

void buttonEvent::deactivate()
{
    relayEvent::deactivate();
}

bool buttonEvent::isActive()
{
    return (chrono::system_clock::now() - startTime) < duration;
}
