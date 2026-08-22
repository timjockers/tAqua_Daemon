#include "fileexchange.hpp"

#include <iostream>
using namespace libconfig;
using namespace std;


ConfigManager::ConfigManager(const string& filepath)
    : path(filepath)
{
    read();
}

void ConfigManager::read()
{
    cout << "Read" << endl;
}
