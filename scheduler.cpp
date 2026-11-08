#include "scheduler.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;


Scheduler::Scheduler(QueueManager& queueManager, StatusManager& statusManager)
    : queueM(queueManager),
      statusM(statusManager)
{}

void Scheduler::updateStatus()
{
    std::vector<json> scheduledInformation;
    scheduledInformation.reserve(scheduledEvents.size());

    for (auto& event : scheduledEvents)
    {
        scheduledInformation.push_back(event.getJSON());
    }

    statusM.setScheduledInformation(scheduledInformation);
    statusM.write();
}

void Scheduler::setEventVector(const std::vector<scheduledEvent>& events)
{
    scheduledEvents = events;

    updateStatus();
}

void Scheduler::work()
{
    for (scheduledEvent& ev: scheduledEvents)
    {
        if (ev.shouldEnqueueNow())
        {
            queueM.addEvent(ev.toDurationEvent());
        }
    }
}
