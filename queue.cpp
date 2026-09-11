#include "queue.hpp"

#include <chrono>
#include <iostream>

using namespace std;

QueueManager::QueueManager(ioManager& io)
    : ioM(io)
{}

string QueueManager::getQueueInfo()
{
    cout << "LOG: QueueManager::getQueueInfo() requested" << endl; // LOG
    lock_guard<mutex> lock(mtx);
    return getQueueInfoUnlocked();
}

string QueueManager::getQueueInfoUnlocked()
{
    cout << "LOG: QueueManager::getQueueInfoUnlocked() building info" << endl; // LOG

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

bool QueueManager::containsDurationEvent(Relay relay) const
{
    cout << "LOG: QueueManager::containsDurationEvent() relay lookup" << endl; // LOG
    lock_guard<mutex> lock(mtx);
    return containsDurationEventUnlocked(relay);
}

bool QueueManager::containsDurationEventUnlocked(Relay relay) const
{
    cout << "LOG: QueueManager::containsDurationEventUnlocked() scanning queue" << endl; // LOG
    for (const auto& event : events)
    {
        const auto* durationEvent = dynamic_cast<const class durationEvent*>(event.get());

        if (durationEvent && durationEvent->getRelay() == relay)
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
        ioM.setYLED(YLEDS[i], containsDurationEventUnlocked(RELAYS[i]));
    }
}

void QueueManager::removeQueuedDurationEvent(Relay relay)
{
    cout << "LOG: QueueManager::removeQueuedDurationEvent() removing relay event" << endl; // LOG
    lock_guard<mutex> lock(mtx);

    auto it = events.begin();
    while (it != events.end())
    {
        const auto* queuedButton = dynamic_cast<const class durationEvent*>(it->get());
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
    cout << "LOG: QueueManager::cancelActiveEvent() canceling active event" << endl; // LOG
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
    cout << "LOG: QueueManager::addEvent() entering" << endl; // LOG
    if (!event)
    {
        cout << "LOG: QueueManager::addEvent() dropped null event" << endl; // LOG
        return;
    }

    cout << "LOG: QueueManager::addEvent() enqueueing " << event->getInfo() << endl; // LOG

    lock_guard<mutex> lock(mtx);

    if (stopping)
    {
        cout << "LOG: QueueManager::addEvent() stopping, reject event " << event->getInfo() << endl; // LOG
        return;
    }

    events.push_back(std::move(event));
    cout << "LOG: QueueManager::addEvent() queued " << getQueueInfoUnlocked() << endl; // LOG
    refreshYLEDsUnlocked();
    condition.notify_one();
}

unique_ptr<irrigationEvent> QueueManager::takeFirstEventUnlocked()
{
    cout << "LOG: QueueManager::takeFirstEventUnlocked() taking first queued event" << endl; // LOG
    if (events.empty())
    {
        cout << "LOG: QueueManager::takeFirstEventUnlocked() queue empty" << endl; // LOG
        return nullptr;
    }

    auto event = std::move(events.front());
    cout << "LOG: QueueManager::takeFirstEventUnlocked() pulled " << event->getInfo() << endl; // LOG
    events.pop_front();

    refreshYLEDsUnlocked();
    return event;
}

void QueueManager::work()
{   
    cout << "LOG: QueueManager::work() tick" << endl; // LOG
    unique_lock<mutex> lock(mtx);

    if (!activeEvent)
    {
        cout << "LOG: QueueManager::work() no active event, waiting for queue" << endl; // LOG
        condition.wait_for(lock, chrono::milliseconds(100), [this]
        {
            cout << "LOG: QueueManager::work() queue wait condition evaluated" << endl; // LOG
            return stopping || !events.empty();
        });

        if (stopping)
        {
            cout << "LOG: QueueManager::work() stopping received" << endl; // LOG
            return;
        }

        cout << "LOG: QueueManager::work() dequeueing next event" << endl; // LOG
        activeEvent = takeFirstEventUnlocked();

        if (activeEvent)
        {
            cout << "LOG: QueueManager::work() activating " << activeEvent->getInfo() << endl; // LOG
            activeEvent->activate(&ioM);
            cout << "LOG: QueueManager::work() queue state after activation: " << getQueueInfoUnlocked() << endl; // LOG
            refreshYLEDsUnlocked();
        }
    }

    else if (activeEvent->isActive())
    {
        cout << "LOG: QueueManager::work() active event still running " << activeEvent->getInfo() << endl; // LOG
        condition.wait_for(lock, chrono::milliseconds(10));
    }

    if (activeEvent && !activeEvent->isActive())
    {
        cout << "LOG: QueueManager::work() deactivating finished event " << activeEvent->getInfo() << endl; // LOG
        activeEvent->deactivate(&ioM);
        activeEvent.reset();
        refreshYLEDsUnlocked();
    }
}

void QueueManager::stop()
{
    cout << "LOG: QueueManager::stop() requested" << endl; // LOG
    lock_guard<mutex> lock(mtx);
    stopping = true;
    condition.notify_one();
}
