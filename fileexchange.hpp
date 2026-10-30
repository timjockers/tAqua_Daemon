#pragma once

#include <string>
#include <array>
#include <vector>
#include <deque>
#include <chrono>
#include <libconfig.h++>
#include "types.hpp"
#include "events.hpp"
#include <sys/types.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;


class ConfigManager {
public:
    ConfigManager(const std::string& filepath);

    void updateConfig();
    RelayConfig getRelayConfig(Relay relay);
    std::chrono::seconds getButtonIrrTime();
    const std::vector<scheduledEvent>& getScheduledEvents() const;
    
private:
    void read();
    void store();

    const std::string path;

    libconfig::Config cfg;

    std::array<RelayConfig, 8> relayConfig;
    std::chrono::seconds buttonIrrTime;
    std::vector<scheduledEvent> scheduledEvents;
};


class LogManager {
public:
    LogManager(const std::string& filepath);

    void addLine(const std::string& message);
private:
    std::string getTimestamp();
    const std::string path;
    std::deque<std::string> logLines;
};


class StatusManager {
public:
    StatusManager(const std::string& filepath, const std::string& taqua_pid_path);

    void setIrrigationStatus(const Relay r, StatusIndicator irrStatus);
    const StatusIndicator getIrrigationStatus(const Relay r);

    void setQueueInformation(const std::vector<json>* information);
    const std::vector<json> getQueueInformation();

    bool write();

    bool readWebPID();

private:
    const std::string path;
    const std::string pid_path;

    bool notifyWebStatusChanged();
    pid_t web_pid;
    bool web_pid_loaded;

    std::array<StatusIndicator, 8> irrigationStatus;
    std::vector<json> queueInformation;
};
