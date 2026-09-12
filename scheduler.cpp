#include "scheduler.hpp"

Scheduler::Scheduler(QueueManager *queueManager)
    : queueM(queueManager)
{}

void Scheduler::setEventVector(const std::vector<scheduledEvent>& events)
{
    scheduledEvents = events;
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
