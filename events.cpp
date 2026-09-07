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



buttonEvent::buttonEvent(Relay r, chrono::seconds irrDuration)
    : relayEvent(r), duration(irrDuration)
{}

string buttonEvent::getInfo()
{
    return relayEvent::getInfo() + " >> " + "buttonEvent(D" + to_string(duration.count()) + "s)";
}

void buttonEvent::activate(ioManager* io)
{
    relayEvent::activate(io);

    startTime = chrono::system_clock::now();
}

bool buttonEvent::isActive()
{
    return (chrono::system_clock::now() - startTime) < duration;
}
