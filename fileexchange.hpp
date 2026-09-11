#pragma once

#include <string>
#include <array>
#include <vector>
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
    std::vector<scheduledEvent>& getScheduledEvents() const;
    
private:
    void read();
    void store();

    const std::string path;

    libconfig::Config cfg;

    std::array<RelayConfig, 8> relayConfig;
    std::chrono::seconds buttonIrrTime;
    std::vector<scheduledEvent> scheduledEvents;
};
