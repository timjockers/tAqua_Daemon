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
        }

        cout << getQueueInfo() << endl;
    }

    else if (activeEvent->isActive())
    {
        condition.wait_for(lock, chrono::milliseconds(10));
    }

    if (activeEvent && !activeEvent->isActive())
    {
        activeEvent->deactivate(&ioM);
        activeEvent.reset();
    }
}

void QueueManager::stop()
{
    lock_guard<mutex> lock(mtx);
    stopping = true;
    condition.notify_one();
}
