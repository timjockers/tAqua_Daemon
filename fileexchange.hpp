#pragma once

#include <string>
#include <array>
#include <libconfig.h++>


#define RC_UNUSED         0
#define RC_VALVE          1
#define RC_PERMANENTPOWER 2

class ConfigManager {
public:
    ConfigManager(const std::string& filepath);

    void updateConfig();
    int getRelayConfig(int relay);
    
private:
    void read();
    void store();

    const std::string path;

    libconfig::Config cfg;

    std::array<int, 8> relayConfig;
};
