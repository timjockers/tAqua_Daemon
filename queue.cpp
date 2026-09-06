#include "queue.hpp"

using namespace std;

void QueueManager::addEvent(unique_ptr<irrigationEvent> event)
{
    if (event)
    {
        events.push_back(std::move(event));
    }
}

unique_ptr<irrigationEvent> QueueManager::takeFirstEvent()
{
    if (events.empty())
    {
        return nullptr;
    }

    auto event = std::move(events.front());
    events.pop_front();

    return event;
}