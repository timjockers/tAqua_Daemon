#include "scheduler.hpp"

Scheduler::Scheduler(QueueManager *queueManager)
    : queueM(queueManager)
{}

void Scheduler::setEventVector(const std::vector<scheduledEvent>& events)
{
    scheduledEvents.clear();
    scheduledEvents.reserve(events.size());

    for (const auto& event : events)
    {
        scheduledEvents.push_back(event);
    }
}

void Scheduler::work()
{
    for (scheduledEvent& ev: scheduledEvents)
    {
        if (ev.shouldEnqueueNow())
        {
            queueM->addEvent(ev.toDurationEvent());
        }
    }
}
