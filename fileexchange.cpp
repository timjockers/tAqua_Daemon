#include "fileexchange.hpp"

#include <iostream>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <csignal>
#include <fstream>
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

            if (item.exists("valid"))
            {
                bool valid = false;
                if (!item.lookupValue("valid", valid) || !valid) {
                    // Skips the event if "valid" is found and is false
                    continue;
                }
            }

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


LogManager::LogManager(const string& filepath)
    : path(filepath)
{
    ifstream inFile(path);
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        logLines.push_back(line);
        if (logLines.size() > 500) {
            logLines.pop_front();
        }
    }
}

string LogManager::getTimestamp()
{
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void LogManager::addLine(const string& message)
{
    const string newLog = "[" + getTimestamp() + "] " + message;
    ofstream appendFile(path, ios::app);
    if (!appendFile.is_open()) {
        cerr << "Error writing log file!" << endl;
        return;
    }

    appendFile << newLog << '\n';
    appendFile.close();
    if (!appendFile) {
        cerr << "Error writing log file!" << endl;
        return;
    }

    logLines.push_back(newLog);
    if (logLines.size() <= 500) {
        return;
    }

    logLines.pop_front();

    ofstream outFile(path, ios::trunc);
    if (!outFile.is_open()) {
        cerr << "Error writing log file!" << endl;
        return;
    }

    for (const auto& logLine : logLines) {
        outFile << logLine << '\n';
    }
    outFile.close();
    if (!outFile) {
        cerr << "Error writing log file!" << endl;
    }
}


StatusManager::StatusManager(const string& filepath, const string& taqua_pid_path)
    : path(filepath),
      pid_path(taqua_pid_path)

{
    irrigationStatus = {StatusIndicator::None};

    web_pid_loaded = false;
}

bool StatusManager::readWebPID()
{
    ifstream pidFile(pid_path);

    if (!pidFile)
    {
        cerr << "Could not open taqua.pid" << endl;
        return false;
    }

    pidFile >> web_pid;

    if (!web_pid)
    {
        cerr << "Could not read PID from taqua.pid" << endl;
        return false;
    }

    if (web_pid <= 0)
    {
        cerr << "Invalid daemon PID: " << web_pid << endl;
        return false;
    }

    web_pid_loaded = true;

    return true;
}

bool StatusManager::notifyWebStatusChanged()
{
    if (!web_pid_loaded)
    {   
        cout << "WARNING: Could not notify web: web pid not loaded!" << endl;
        return false;
    }

    if (kill(web_pid, 0) != 0)
    {
        perror("Web is not running");
        return false;
    }

    if (kill(web_pid, SIGUSR1) != 0)
    {
        perror("Could not send SIGUSR1");
        return false;
    }

    return true;
}

bool StatusManager::write()
{
    Config cfg;
    Setting &root = cfg.getRoot();

    Setting &arraySetting = root.add("irrigationStatus", Setting::TypeArray);

    for (StatusIndicator i : irrigationStatus)
    {
        Setting &element = arraySetting.add(Setting::TypeInt);
        element = static_cast<int>(i);
    }

    try
    {
        cfg.writeFile(path.c_str());
        
        if (!notifyWebStatusChanged())
        {
            cerr << "Could not notify web" << endl;
            return false;
        }

        return true;
    }
    catch(const FileIOException &fioex)
    {
        cerr << "I/O error while writing file: " << path << endl;
        return false;
    }
}


void StatusManager::setIrrigationStatus(const Relay r, StatusIndicator irrStatus)
{
    irrigationStatus[relayIndex(r)] = irrStatus;
}

const StatusIndicator StatusManager::getIrrigationStatus(const Relay r)
{
    return irrigationStatus[relayIndex(r)];
}
