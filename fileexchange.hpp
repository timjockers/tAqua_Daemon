#pragma once

#include <string>
#include <array>
#include <libconfig.h++>
#include "types.hpp"


class ConfigManager {
public:
    ConfigManager(const std::string& filepath);

    void updateConfig();
    RelayConfig getRelayConfig(Relay relay);
    
private:
    void read();
    void store();

    const std::string path;

    libconfig::Config cfg;

    std::array<RelayConfig, 8> relayConfig;
};
