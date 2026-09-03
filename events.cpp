#include "events.hpp"

#include <iostream>

using namespace std;

string irrigationEvent::getInfo()
{
    return "irrigationEvent";
}

void irrigationEvent::activate() {}

void irrigationEvent::deactivate() {}

bool irrigationEvent::isActive()
{
    return false;
}



relayEvent::relayEvent(Relay r)
    : relay(r)
{}

string relayEvent::getInfo()
{
    return irrigationEvent::getInfo() + " >> " + "relayEvent(R" + to_string(relayIndex(relay) + 1) + ")";
}



buttonEvent::buttonEvent(Relay r, chrono::seconds irrDuration)
    : relayEvent(r), duration(irrDuration)
{}

string buttonEvent::getInfo()
{
    return relayEvent::getInfo() + " >> " + "buttonEvent(D" + to_string(duration.count()) + "s)";
}

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
