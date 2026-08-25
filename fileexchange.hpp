#pragma once

#include <string>
#include <array>
#include <libconfig.h++>

class ConfigManager {
public:
    ConfigManager(const std::string& filepath);

    void updateConfig();
    
private:
    void read();
    void store();

    const std::string path;

    libconfig::Config cfg;

    std::array<int, 8> relayConfig;
};
