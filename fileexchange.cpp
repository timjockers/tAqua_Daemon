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
    try {
        cfg.readFile(path.c_str());
    } catch (...) {
        cerr << "Error loading the file: " << path << endl;
    }
}

void ConfigManager::store()
{   
    const Setting& root = cfg.getRoot();

    try
    {
        const Setting& r = root["relayConfig"];
        int count = r.getLength();

    }
    catch(const SettingNotFoundException &nfex)
    {
        cerr << "Error: relayConfig could not be found." << endl;
    }
}
