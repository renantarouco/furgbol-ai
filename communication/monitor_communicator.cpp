#include "monitor_communicator.h"

#include <furgbol-core/proto/messages_ai_data_manager.pb.h>

#include "configuration/configuration.h"
#include "utils/utils.h"

MonitorCommunicator::MonitorCommunicator(std::shared_ptr<MonitorRepository> repository, std::shared_ptr<bool> running) :
    repository_(repository),
    writer_(Configuration::MONITOR_IP, Configuration::MONITOR_PORT),
    running_(running)
{

}

void MonitorCommunicator::init()
{

}

void MonitorCommunicator::run()
{
    while (*running_) {
        if (repository_->is_package_sent()) {
            AIDataManagerPackage packet = repository_->packet();
            std::string datagram;
            packet.SerializeToString(&datagram);
            writer_.send(datagram);
            repository_->set_package_sent();
        }
    }
}
