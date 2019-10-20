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
        switch (Configuration::SYSTEM_ROBOTS_TYPE) {
        case REAL: {
            serial_repo_ = std::make_shared<SerialRepository>(Configuration::SYSTEM_MAX_ROBOTS);
            serial_comm_ = std::make_unique<SerialCommunicator>(serial_repo_, running_);
            serial_comm_->init();
            break;
        }
        case SIMULATED: {
            grsim_repo_ = std::make_shared<GrSimRepository>(Configuration::SYSTEM_MAX_ROBOTS);
            grsim_comm_ = std::make_unique<GrSimCommunicator>(grsim_repo_, running_);
            grsim_comm_->init();
            break;
        }
        }
        monitor_repo_ = std::make_shared<MonitorRepository>(Configuration::SYSTEM_MAX_ROBOTS);
        monitor_comm_ = std::make_unique<MonitorCommunicator>(monitor_repo_, running_);
        monitor_comm_->init();
    } else {
        DEBUG("missing config path parameter");
        exit(1);
    }
}

void System::run()
{
    *running_ = true;
    switch (Configuration::SYSTEM_ROBOTS_TYPE) {
    case REAL: {
        serial_comm_thread_ = std::make_unique<std::thread>(&SerialCommunicator::run, serial_comm_.get());
        break;
    }
    case SIMULATED: {
        grsim_comm_thread_ = std::make_unique<std::thread>(&GrSimCommunicator::run, grsim_comm_.get());
        break;
    }
    }
    monitor_comm_thread_ = std::make_unique<std::thread>(&MonitorCommunicator::run, monitor_comm_.get());
    monitor_comm_thread_->join();
}
