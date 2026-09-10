#include "events.hpp"

#include <iostream>

using namespace std;


string irrigationEvent::getInfo()
{
    return "irrigationEvent";
}

void irrigationEvent::activate(ioManager* io) {}

void irrigationEvent::deactivate(ioManager* io) {}

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

Relay relayEvent::getRelay() const
{
    return relay;
}

void relayEvent::activate(ioManager* io)
{
    io->setRelay(getRelay(), true);
}

void relayEvent::deactivate(ioManager* io)
{
    io->setRelay(getRelay(), false);
}



durationEvent::durationEvent(Relay r, chrono::seconds irrDuration)
    : relayEvent(r), duration(irrDuration)
{}

string durationEvent::getInfo()
{
    return relayEvent::getInfo() + " >> " + "durationEvent(D" + to_string(duration.count()) + "s)";
}

void durationEvent::activate(ioManager* io)
{
    relayEvent::activate(io);

    startTime = chrono::system_clock::now();
}

bool durationEvent::isActive()
{
    return (chrono::system_clock::now() - startTime) < duration;
}


scheduledEvent::scheduledEvent(Relay r, chrono::seconds irrDuration, Weekday weekday)
    : durationEvent(r, irrDuration), wday(weekday)
{}

string scheduledEvent::getInfo()
{
    return relayEvent::getInfo() + " >> " + "scheduledEvent(W" + to_string(static_cast<int>(wday)) + ", starttime)";
}
