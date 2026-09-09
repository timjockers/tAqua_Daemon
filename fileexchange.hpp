#pragma once

#include <string>
#include <array>
#include <chrono>
#include <libconfig.h++>
#include "types.hpp"


class ConfigManager {
public:
    ConfigManager(const std::string& filepath);

    void updateConfig();
    RelayConfig getRelayConfig(Relay relay);
    std::chrono::seconds getButtonIrrTime();
    
private:
    void read();
    void store();

    const std::string path;

    libconfig::Config cfg;

    std::array<RelayConfig, 8> relayConfig;
    std::chrono::seconds buttonIrrTime;
};
