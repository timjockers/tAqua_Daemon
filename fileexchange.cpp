#include "fileexchange.hpp"

#include <iostream>
#include <cstddef>
using namespace libconfig;
using namespace std;


ConfigManager::ConfigManager(const string& filepath)
    : path(filepath)
{
    relayConfig = {RelayConfig::UNUSED};

    updateConfig();
}

void ConfigManager::updateConfig()
{
    read();
    store();
}

void ConfigManager::read()
{
    try
    {
        cfg.readFile(path.c_str());
    }
    catch(const FileIOException &fioex)
    {
        cerr << "I/O error while reading file." << endl;
    }
    catch(const ParseException &pex)
    {
        cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
    }
}

void ConfigManager::store()
{
    try
    {
        const Setting& r = cfg.lookup("relayConfig");

        if (r.getLength() != static_cast<int>(relayConfig.size()))
        {
            cerr << "Length of relayConfig is not "
                      << relayConfig.size()
                      << endl;
            return;
        }

        for (size_t i = 0; i < relayConfig.size(); ++i)
        {
            relayConfig[i] = static_cast<RelayConfig>(
                static_cast<int>(r[i])
            );
        }
    }
    catch (const SettingNotFoundException&)
    {
        cerr << "Error: relayConfig could not be found." << endl;
    }

    try
    {
        int value = 0;
        cfg.lookupValue("buttonIrrigationTime", value);
        buttonIrrTime = chrono::seconds(value);
    }
    catch (const SettingNotFoundException&)
    {
        cerr << "Error: buttonIrrigationTime could not be found." << endl;
    }
    catch (const SettingTypeException&)
    {
        cerr << "Error: buttonIrrigationTime is not an integer." << endl;
    }

    try
    {   
        scheduledEvents.clear();

        const Setting& scheduled = cfg.lookup("scheduled");

        for (size_t i = 0; i < scheduled.getLength(); ++i)
        {
            const Setting& item = scheduled[i];

            int relay = 0;
            int duration = 0;

            if (!item.lookupValue("relay", relay) || !item.lookupValue("duration", duration)
                || relay < 0 || relay >= static_cast<int>(RELAYS.size()))
            {
                cerr << "Invalid scheduled event relay or duration" << endl;
                continue;
            }

            int minutes = 0;
            WeekdayMask weekdays = 0;

            if (item.exists("weekdays"))
            {
                int weekdayMask = 0;
                if (!item.lookupValue("weekdays", weekdayMask)
                    || weekdayMask < 0 || weekdayMask > 0x7f
                    || !item.lookupValue("start", minutes))
                {
                    cerr << "Invalid scheduled event weekday mask or start time" << endl;
                    continue;
                }

                weekdays = static_cast<WeekdayMask>(weekdayMask);
            }
            else
            {
                const Setting& start = item.lookup("start");
                const int weekday = start[0];
                minutes = start[1];

                if (weekday < 0 || weekday > 6)
                {
                    cerr << "Invalid legacy scheduled event weekday" << endl;
                    continue;
                }

                weekdays = weekdayBit(static_cast<Weekday>(weekday));
            }

            if (minutes < 0 || minutes >= 24 * 60)
            {
                cerr << "Invalid scheduled event start time" << endl;
                continue;
            }

            scheduledEvents.emplace_back(
                RELAYS[relay],
                std::chrono::seconds(duration),
                weekdays,
                std::chrono::minutes(minutes)
            );
        }
    }
    catch (const SettingNotFoundException&)
    {
        cerr << "Error reading schedule" << endl;
    }
    catch (const SettingTypeException&)
    {
        cerr << "Invalid setting type in schedule" << endl;
    }
}


RelayConfig ConfigManager::getRelayConfig(Relay relay)
{
    return relayConfig[relayIndex(relay)];
}

chrono::seconds ConfigManager::getButtonIrrTime()
{
    return buttonIrrTime;
}

const vector<scheduledEvent>& ConfigManager::getScheduledEvents() const
{
    return scheduledEvents;
}
