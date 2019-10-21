#include "configuration.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "utils/utils.h"

size_t Configuration::SYSTEM_MAX_ROBOTS;
RobotsType Configuration::SYSTEM_ROBOTS_TYPE;
std::string Configuration::PERCEPTION_IP;
unsigned short Configuration::PERCEPTION_RECEIVE_PORT;
unsigned short Configuration::PERCEPTION_SEND_PORT;
std::string Configuration::SERIAL_PORT;
size_t Configuration::SERIAL_PACKAGE_SIZE;
std::string Configuration::GRSIM_IP;
unsigned short Configuration::GRSIM_PORT;
std::string Configuration::MONITOR_IP;
unsigned short Configuration::MONITOR_PORT;

void Configuration::load(std::string path)
{
    std::ifstream stream(path);
    nlohmann::json json;
    stream >> json;
    SYSTEM_MAX_ROBOTS = json["system"]["max_robots"];
    DEBUG(SYSTEM_MAX_ROBOTS);
    SYSTEM_ROBOTS_TYPE = static_cast<RobotsType>(json["system"]["robots_type"]);
    PERCEPTION_IP = json["perception"]["ip"];
    PERCEPTION_RECEIVE_PORT = json["perception"]["receive_port"];
    PERCEPTION_SEND_PORT = json["perception"]["send_port"];
    SERIAL_PORT = json["serial"]["port"];
    SERIAL_PACKAGE_SIZE = json["serial"]["package_size"];
    GRSIM_IP = json["grsim"]["ip"];
    GRSIM_PORT = json["grsim"]["port"];
    MONITOR_IP = json["monitor"]["ip"];
    MONITOR_PORT = json["monitor"]["port"];
}
