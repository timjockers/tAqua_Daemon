#include "queue.hpp"

#include <chrono>
#include <iostream>

using namespace std;

QueueManager::QueueManager(ioManager& io, LogManager& l, StatusManager& s)
    : ioM(io),
      logM(l),
      statusM(s)
{
    lock_guard<mutex> lock(mtx);
    synchronizeStatusUnlocked();
}

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

void QueueManager::synchronizeStatusUnlocked()
{
    vector<json> queuedEvents;
    array<bool, RELAYS.size()> queuedRelays{};
    array<bool, RELAYS.size()> queuedButtons{};

    for (const auto& event : events)
    {
        if (!event)
        {
            continue;
        }

        queuedEvents.emplace_back(event->getJSON());

        const auto* relayEvent = dynamic_cast<const class relayEvent*>(event.get());
        if (!relayEvent)
        {
            continue;
        }

        const size_t index = relayIndex(relayEvent->getRelay());
        queuedRelays[index] = true;
        if (dynamic_cast<const class buttonEvent*>(event.get()))
        {
            queuedButtons[index] = true;
        }
    }

    const auto* activeRelayEvent = dynamic_cast<const relayEvent*>(activeEvent.get());
    statusM.setQueueInformation(queuedEvents);

    for (size_t i = 0; i < YLEDS.size(); ++i)
    {
        const Relay relay = RELAYS[i];
        ioM.setYLED(YLEDS[i], queuedButtons[i]);

        const bool isActive = activeRelayEvent && activeRelayEvent->getRelay() == relay;
        const StatusIndicator newStatus = isActive
            ? StatusIndicator::Active
            : queuedRelays[i] ? StatusIndicator::Scheduled : StatusIndicator::None;
        statusM.setIrrigationStatus(relay, newStatus);
    }

    statusM.write();
}

void QueueManager::removeQueuedButtonEvent(Relay relay)
{
    lock_guard<mutex> lock(mtx);

    bool queueChanged = false;
    auto it = events.begin();
    while (it != events.end())
    {
        const auto* queuedButton = dynamic_cast<const class buttonEvent*>(it->get());
        if (queuedButton && queuedButton->getRelay() == relay)
        {
            it = events.erase(it);
            queueChanged = true;
        }
        else
        {
            ++it;
        }
    }

    if (queueChanged)
        synchronizeStatusUnlocked();
    condition.notify_one();
}

void QueueManager::cancelActiveEvent()
{
    lock_guard<mutex> lock(mtx);

    if (activeEvent)
    {
        activeEvent->deactivate(&ioM);
        activeEvent.reset();
        synchronizeStatusUnlocked();
    }

    condition.notify_one();
}

void QueueManager::refreshStatus()
{
    lock_guard<mutex> lock(mtx);
    synchronizeStatusUnlocked();
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
    synchronizeStatusUnlocked();

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
            synchronizeStatusUnlocked();

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
        synchronizeStatusUnlocked();
    }
}

void QueueManager::stop()
{
    lock_guard<mutex> lock(mtx);
    stopping = true;
    condition.notify_one();
}
