#include "scheduler.hpp"

#include <iostream>

using namespace std;

Scheduler::Scheduler(QueueManager *queueManager)
    : queueM(queueManager)
{}

void Scheduler::setEventVector(const std::vector<scheduledEvent>& events)
{
    cout << "LOG: Scheduler::setEventVector loaded " << events.size() << " scheduled events" << endl; // LOG
    scheduledEvents = events;
}

void Scheduler::work()
{
    cout << "LOG: Scheduler::work() scan " << scheduledEvents.size() << " scheduled events" << endl; // LOG
    for (scheduledEvent ev: scheduledEvents)
    {
        cout << "LOG: Scheduler::work() checking " << ev.getInfo() << endl; // LOG
        if (ev.shouldEnqueueNow())
        {
            cout << "LOG: scheduled event hinzugefügt " << ev.getInfo() << endl; // LOG
            queueM->addEvent(ev.toDurationEvent());
        }
    }
}
