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
    static Settings& getInstance();
};

#endif 