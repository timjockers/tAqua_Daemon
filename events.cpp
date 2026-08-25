#include "events.hpp"

using namespace std;

std::string irrigationEvent::getInfo()
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

std::string relayEvent::getInfo()
{
    return "RelayEvent: R" + to_string(relay);
}




buttonEvent::buttonEvent(int r, std::chrono::seconds irrDuration)
    : relayEvent(r), duration(irrDuration)
{}
