#include "daemon.hpp"

#include <iostream>

using namespace std;

int main() {
    cout << "LOG: main() constructing tAquaDaemon" << endl; // LOG
    tAquaDaemon d;
    cout << "LOG: main() entering daemon run loop" << endl; // LOG
    d.run();

    cout << "LOG: main() daemon finished" << endl; // LOG
    return 0;
}