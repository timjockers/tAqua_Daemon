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
