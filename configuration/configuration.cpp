#include "configuration.h"

#include <fstream>

#include <nlohmann/json.hpp>

void Configuration::load(std::string path)
{
    std::ifstream stream(path);
    nlohmann::json json;
    stream >> json;
    SYSTEM_MAX_ROBOTS = json["system"]["max_robots"];
    SYSTEM_ROBOTS_TYPE = static_cast<RobotsType>(json["system"]["robots_type"]);
    SERIAL_PORT = json["serial"]["port"];
    SERIAL_PACKAGE_SIZE = json["serial"]["package_size"];
    GRSIM_IP = json["grSim"]["ip"];
    GRSIM_PORT = json["grsim"]["port"];
    MONITOR_IP = json["monitor"]["ip"];
    MONITOR_PORT = json["monitor"]["port"];
}
