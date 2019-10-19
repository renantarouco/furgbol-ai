#include "system.h"

#include "configuration/configuration.h"
#include "utils/utils.h"

System::System()
{

}

void System::init(int argc, char* argv[])
{
    if (argc >= 2) {
        std::string config_path(argv[1]);
        Configuration::load(config_path);
        running_ = std::make_shared<bool>(false);
        if (Configuration::SYSTEM_ROBOTS_TYPE != REAL && Configuration::SYSTEM_ROBOTS_TYPE != SIMULATED) {
            DEBUG("robots must be real or simulated");
            exit(1);
        }
        switch (Configuration::SYSTEM_ROBOTS_TYPE) {
        case REAL: {
            serial_repo_ = std::make_shared<SerialRepository>(Configuration::SYSTEM_MAX_ROBOTS);
            serial_comm_ = std::make_shared<SerialCommunicator>(serial_repo_, running_);
            break;
        }
        case SIMULATED: {
            grsim_repo_ = std::make_shared<GrSimRepository>(Configuration::SYSTEM_MAX_ROBOTS);
            grsim_comm_ = std::make_shared<GrSimCommunicator>(grsim_comm_, running_);
            break;
        }
        }
        monitor_repo_ = std::make_shared<MonitorRepository>(Configuration::SYSTEM_MAX_ROBOTS);
        monitor_comm_ = std::make_shared<MonitorCommunicator>(monitor_repo_, running_);
    } else {
        DEBUG("missing config path parameter");
        exit(1);
    }
}

void run()
{

}
