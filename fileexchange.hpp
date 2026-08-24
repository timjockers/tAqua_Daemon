#pragma once

#include <string>
#include <array>
#include <libconfig.h++>

class ConfigManager {
public:
    ConfigManager(const std::string& filepath);
    void read();
private:
    const std::string path;

    std::array<int, 8> relayConfig;
};
