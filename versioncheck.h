#ifndef VERSIONCHECK_H
#define VERSIONCHECK_H

#include <iostream>
#include <regex>
#include "program.h"

using namespace std;

struct ver {
    ver() { }
    ver(string v) : value(v) { }
    
    string value;
    int sub[4];
    
    bool update() {
        
        return true;
    }
};

void checkConfigVersion(string cfg, int &x) {
    ver thisVer(program_version);
    if (!thisVer.update()) {
        x = -2;
        return;
    }
    
    ver cfgVer(cfg);
    if (!cfgVer.update()) {
        x = -3;
        return;
    }
    
    for (int i = 0; i < 3; ++i) {
        if (thisVer.sub[i] < cfgVer.sub[i]) {
            x = 0;
            return;
        }
        if (thisVer.sub[i] > cfgVer.sub[i]) {
            x = 1;
            return;
        }
    }
    x = 2;
}

#endif /* VERSIONCHECK_H */
