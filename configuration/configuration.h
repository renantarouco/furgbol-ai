#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

enum RobotsType { REAL = 0, SIMULATED = 1 };

class Configuration
{
public:
    static size_t SYSTEM_MAX_ROBOTS;
    static RobotsType SYSTEM_ROBOTS_TYPE;
    static std::string PERCEPTION_IP;
    static unsigned short PERCEPTION_RECEIVE_PORT;
    static unsigned short PERCEPTION_SEND_PORT;
    static std::string SERIAL_PORT;
    static size_t SERIAL_PACKAGE_SIZE;
    static std::string GRSIM_IP;
    static unsigned short GRSIM_PORT;
    static std::string MONITOR_IP;
    static unsigned short MONITOR_PORT;
    static void load(std::string);
};

#endif // CONFIGURATION_H
