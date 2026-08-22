#ifndef FILEEXCHANGE_HPP
#define FILEEXCHANGE_HPP

#include <string>
#include <libconfig.h++>

class ConfigManager {
public:
    ConfigManager(const std::string& filepath);
    void read();
private:
    const std::string path;
};

#endif // FILEEXCHANGE_HPP