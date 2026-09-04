#include "queue.hpp"


QueueManager::QueueManager()
{

}

void QueueManager::addEvent(irrigationEvent* event)
{
    if (firstEvent == nullptr)
    {
        firstEvent = event;
    }
    else
    {
        firstEvent->setNextEvent(event);
    }
}
