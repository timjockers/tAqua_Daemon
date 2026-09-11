#include "events.hpp"

#include "io.hpp"
#include <ctime>
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


scheduledEvent::scheduledEvent(Relay r, chrono::seconds irrDuration, Weekday weekday, chrono::minutes startTime)
    : durationEvent(r, irrDuration), wday(weekday), starttime(startTime)
{}

string scheduledEvent::getInfo()
{
    const int total = static_cast<int>(starttime.count());
    const int hour = total / 60;
    const int minute = total % 60;

    return relayEvent::getInfo() + " >> " + "scheduledEvent(W" + to_string(static_cast<int>(wday)) + ", ST" + to_string(hour) + ":" + to_string(minute) + ")";
}

bool scheduledEvent::matchesSchedule(chrono::system_clock::time_point now)
{
    const auto tt = chrono::system_clock::to_time_t(now);
    const auto* local_time = std::localtime(&tt);

    if (!local_time)
    {
        return false;
    }

    if (static_cast<Weekday>(local_time->tm_wday) != wday)
    {
        return false;
    }

    const int currentMinuteOfDay = local_time->tm_hour * 60 + local_time->tm_min;
    const int startMinuteOfDay = static_cast<int>(starttime.count());

    return currentMinuteOfDay == startMinuteOfDay;
}

bool scheduledEvent::shouldEnqueueNow()
{   
    auto now = chrono::system_clock::now();

    const auto tt = chrono::system_clock::to_time_t(now);
    const auto* local_time = std::localtime(&tt);

    if (!local_time)
    {
        return false;
    }

    const int todayKey = (local_time->tm_year + 1900) * 10000 + (local_time->tm_mon + 1) * 100 + local_time->tm_mday;

    if (lastQueuedDayKey == todayKey)
    {
        return false;
    }

    if (matchesSchedule(now))
    {
        lastQueuedDayKey = todayKey;
        return true;
    }
    else
    {
        return false;
    }
}

unique_ptr<durationEvent> scheduledEvent::toDurationEvent() {
    return make_unique<durationEvent>(relay, duration);
}
