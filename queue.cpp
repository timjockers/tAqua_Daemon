#include "queue.hpp"

#include <chrono>
#include <iostream>

using namespace std;

QueueManager::QueueManager(ioManager& io, LogManager& l, StatusManager& s)
    : ioM(io),
      logM(l),
      statusM(s)
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
        const auto* buttonE = dynamic_cast<const class buttonEvent*>(event.get());

        if (buttonE && buttonE->getRelay() == relay)
        {
            return true;
        }
    }

    return false;
}

void QueueManager::refreshYLEDsUnlocked()
{
    bool statusChanged = false;
    const auto* activeRelayEvent = dynamic_cast<const relayEvent*>(activeEvent.get());

    for (size_t i = 0; i < YLEDS.size(); ++i)
    {
        const Relay relay = RELAYS[i];
        const bool isQueued = containsButtonEventUnlocked(relay);
        ioM.setYLED(YLEDS[i], isQueued);

        const bool isActive = activeRelayEvent && activeRelayEvent->getRelay() == relay;
        const StatusIndicator newStatus = isActive
            ? StatusIndicator::Active
            : isQueued ? StatusIndicator::Scheduled : StatusIndicator::None;
        const StatusIndicator currentStatus = statusM.getIrrigationStatus(relay);
        if (currentStatus != newStatus)
        {
            statusM.setIrrigationStatus(
                relay,
                newStatus
            );

            vector<json> evData;

            for (const auto& ev : events)
            {
                if (ev)
                {
                    evData.emplace_back(ev->getJSON());
                }
            }

            statusM.setQueueInformation(&evData);
            statusChanged = true;
        }
    }

    if (statusChanged)
        statusM.write();
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
            refreshYLEDsUnlocked();

            logM.addLine("Start irrigation: " + activeEvent->getInfo());
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

        logM.addLine("Stop irrigation.");
        refreshYLEDsUnlocked();
    }
}

void QueueManager::stop()
{
    lock_guard<mutex> lock(mtx);
    stopping = true;
    condition.notify_one();
}
