#include "queue.hpp"

#include <chrono>
#include <iostream>

using namespace std;

QueueManager::QueueManager(ioManager& io)
    : ioM(io)
{}

string QueueManager::getQueueInfo()
{
    lock_guard<mutex> lock(mtx);
    return getQueueInfoUnlocked();
}

string QueueManager::getQueueInfoUnlocked()
{

    string info = "Active event: ";

    if (activeEvent)
    {
        info += activeEvent->getInfo();
    }
    else
    {
        info += "none";
    }

    info += "\nQueued events: [";

    bool firstEvent = true;

    for (const auto& event : events)
    {
        if (event)
        {
            if (!firstEvent)
            {
                info += ", ";
            }

            info += event->getInfo();
            firstEvent = false;
        }
    }

    info += "]";
    return info;
}

bool QueueManager::containsButtonEvent(Relay relay) const
{
    lock_guard<mutex> lock(mtx);
    return containsButtonEventUnlocked(relay);
}

bool QueueManager::containsButtonEventUnlocked(Relay relay) const
{
    for (const auto& event : events)
    {
        const auto* buttonEvent = dynamic_cast<const class buttonEvent*>(event.get());

        if (buttonEvent && buttonEvent->getRelay() == relay)
        {
            return true;
        }
    }

    return false;
}

void QueueManager::refreshYLEDsUnlocked()
{
    for (size_t i = 0; i < YLEDS.size(); ++i)
    {
        ioM.setYLED(YLEDS[i], containsButtonEventUnlocked(RELAYS[i]));
    }
}

void QueueManager::removeQueuedButtonEvent(Relay relay)
{
    lock_guard<mutex> lock(mtx);

    auto it = events.begin();
    while (it != events.end())
    {
        const auto* queuedButton = dynamic_cast<const class buttonEvent*>(it->get());
        if (queuedButton && queuedButton->getRelay() == relay)
        {
            it = events.erase(it);
        }
        else
        {
            ++it;
        }
    }

    refreshYLEDsUnlocked();
    condition.notify_one();
}

void QueueManager::cancelActiveEvent()
{
    lock_guard<mutex> lock(mtx);

    if (activeEvent)
    {
        activeEvent->deactivate(&ioM);
        activeEvent.reset();
        refreshYLEDsUnlocked();
    }

    condition.notify_one();
}

void QueueManager::addEvent(unique_ptr<irrigationEvent> event)
{
    if (!event)
    {
        return;
    }

    lock_guard<mutex> lock(mtx);

    if (stopping)
    {
        return;
    }

    events.push_back(std::move(event));
    refreshYLEDsUnlocked();
    condition.notify_one();
}

unique_ptr<irrigationEvent> QueueManager::takeFirstEventUnlocked()
{
    if (events.empty())
    {
        return nullptr;
    }

    auto event = std::move(events.front());
    events.pop_front();

    refreshYLEDsUnlocked();
    return event;
}

void QueueManager::work()
{   
    unique_lock<mutex> lock(mtx);

    if (!activeEvent)
    {
        condition.wait_for(lock, chrono::milliseconds(100), [this]
        {
            return stopping || !events.empty();
        });

        if (stopping)
        {
            return;
        }

        activeEvent = takeFirstEventUnlocked();

        if (activeEvent)
        {
            activeEvent->activate(&ioM);
            cout << getQueueInfoUnlocked() << endl;
            refreshYLEDsUnlocked();
        }
    }

    else if (activeEvent->isActive())
    {
        condition.wait_for(lock, chrono::milliseconds(10));
    }

    if (activeEvent && !activeEvent->isActive())
    {
        activeEvent->deactivate(&ioM);
        activeEvent.reset();
        refreshYLEDsUnlocked();
    }
}

void QueueManager::stop()
{
    lock_guard<mutex> lock(mtx);
    stopping = true;
    condition.notify_one();
}
