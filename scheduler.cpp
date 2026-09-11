#include "scheduler.hpp"

Scheduler::Scheduler(QueueManager *queueManager)
    : queueM(queueManager)
{}

void Scheduler::work()
{
    for (scheduledEvent ev: scheduledEvents)
    {
        if (ev.shouldEnqueueNow())
        {
            queueM->addEvent(ev.toDurationEvent());
        }
    }
}
