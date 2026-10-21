#pragma once

#include <string>
#include <array>
#include <vector>
#include <deque>
#include <chrono>
#include <libconfig.h++>
#include "types.hpp"
#include "events.hpp"


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
    StatusManager(const std::string& filepath);

    void setIrrigationStatus(const uint r, StatusIndicator irrStatus);
    bool write();

private:
    const std::string path;

    std::array<StatusIndicator, 8> irrigationStatus;
};
