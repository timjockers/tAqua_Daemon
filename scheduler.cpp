#include "scheduler.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;


Scheduler::Scheduler(QueueManager& queueManager, StatusManager& statusManager)
    : queueM(queueManager),
      statusM(statusManager)
{
    minute_passed = true;
    last_minute = -1;
}

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
    
    auto now = std::chrono::system_clock::now();
    const auto tt = std::chrono::system_clock::to_time_t(now);
    const auto* local_time = std::localtime(&tt);

    last_minute = local_time->tm_min;
    minute_passed = false;
}

void Scheduler::work()
{
    if (!minute_passed)
    {   
        auto now = std::chrono::system_clock::now();
        const auto tt = std::chrono::system_clock::to_time_t(now);
        const auto* local_time = std::localtime(&tt);

        if (last_minute == local_time->tm_min)
        {
            return;
        }
        else
        {
            minute_passed = true;
        }
    }


    for (scheduledEvent& ev: scheduledEvents)
    {
        if (ev.shouldEnqueueNow())
        {
            queueM.addEvent(ev.toDurationEvent());
        }
    }
}
