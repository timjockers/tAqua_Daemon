#include "fileexchange.hpp"

#include <iostream>
using namespace libconfig;
using namespace std;


ConfigManager::ConfigManager(const string& filepath)
    : path(filepath)
{   
    relayConfig = {0};
    read();
}

void ConfigManager::read()
{
    cout << "Read" << endl;
}
