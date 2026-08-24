#include "fileexchange.hpp"

#include <iostream>
using namespace libconfig;
using namespace std;


ConfigManager::ConfigManager(const string& filepath)
    : path(filepath)
{
    relayConfig = {0};

    read();
    store();
}

void ConfigManager::read()
{
    try
    {
        cfg.readFile(path.c_str());
    }
    catch(const FileIOException &fioex)
    {
        cerr << "I/O error while reading file." << endl;
    }
    catch(const ParseException &pex)
    {
        cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
    }
}

void ConfigManager::store()
{
    try
    {
        const Setting& r = cfg.lookup("relayConfig");
        const size_t count = static_cast<size_t>(r.getLength());

        if (count != relayConfig.size())
        {
            cerr << "Length of array relayConfig not " << relayConfig.size() << endl;
            return;
        }

        for (size_t i = 0; i < relayConfig.size(); ++i)
        {
            relayConfig[i] = r[i];
        }
    }
    catch (const SettingNotFoundException&)
    {
        cerr << "Error: relayConfig could not be found." << endl;
    }
}
