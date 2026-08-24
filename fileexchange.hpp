#pragma once

#include <string>
#include <array>
#include <libconfig.h++>

class ConfigManager {
public:
    ConfigManager(const std::string& filepath);
    void read();
    void store();
private:
    const std::string path;

    libconfig::Config cfg;

    std::array<int, 8> relayConfig;
};
