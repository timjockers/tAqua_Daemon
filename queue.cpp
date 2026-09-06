#include "queue.hpp"

using namespace std;

void QueueManager::addEvent(unique_ptr<irrigationEvent> event)
{
    if (!event)
    {
        return;
    }

    lock_guard<mutex> lock(mtx);
    events.push_back(std::move(event));
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
    lock_guard<mutex> lock(mtx);

    if (!activeEvent)
    {
        activeEvent = takeFirstEventUnlocked();

        if (activeEvent)
        {
            activeEvent->activate();
        }
    }

    if (activeEvent && !activeEvent->isActive())
    {
        activeEvent->deactivate();
        activeEvent.reset();
    }
}
