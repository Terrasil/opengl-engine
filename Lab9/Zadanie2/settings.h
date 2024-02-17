#pragma once

#ifndef SETTINGS_H
#define SETTINGS_H

class Settings {
private:
    Settings();
    ~Settings();
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
public:
    bool lightQuality = false;
    bool lightMode = false;
    bool lightVisibility = true;
    bool animateLight = false;
    bool fogVisibility = false; 
    bool showRefraction = true;
    bool showReflection = true;
    bool showMinimap = true;
    unsigned int skyBox = 0;
    unsigned int postProcessMode = 0;
    static Settings& getInstance();
};

#endif 