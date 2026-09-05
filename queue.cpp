#include "queue.hpp"

void QueueManager::addEvent(std::unique_ptr<irrigationEvent> event)
{
    if (event)
    {
        events.push_back(std::move(event));
    }
}

std::unique_ptr<irrigationEvent> QueueManager::takeFirstEvent()
{
    if (events.empty())
    {
        return nullptr;
    }

    auto event = std::move(events.front());
    events.pop_front();

    return event;
}