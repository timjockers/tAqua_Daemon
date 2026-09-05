#include "queue.hpp"


QueueManager::QueueManager()
{

}

void QueueManager::addEvent(std::unique_ptr<irrigationEvent> event)
{
    if (!event)
    {
        return;
    }

    if (firstEvent == nullptr)
    {
        firstEvent = std::move(event);
    }
    else
    {
        irrigationEvent* lastEvent = firstEvent.get();

        while (lastEvent->nextEvent != nullptr)
        {
            lastEvent = lastEvent->nextEvent.get();
        }

        lastEvent->nextEvent = std::move(event);
    }
}
