#include "monitor_communicator.h"

#include "configuration/configuration.h"
#include "utils/utils.h"

MonitorCommunicator::MonitorCommunicator(std::shared_ptr<MonitorRepository> repository, std::shared_ptr<bool> running) :
    repository_(repository),
    ip_(Configuration::MONITOR_IP.c_str()),
    port_(Configuration::MONITOR_PORT),
    writer_(),
    running_(running)
{

}

void MonitorCommunicator::init()
{

}

void MonitorCommunicator::run()
{
    while (*running_) {
        AIDataManagerPackage packet = repository_->packet();
        std::string datagram;
        packet.SerializeToString(&datagram);
        long long res = writer_.writeDatagram(
            datagram.c_str(),
            static_cast<long long>(datagram.size()),
            ip_,
            port_);
        if (res == -1) {
            DEBUG("error sending datagram");
        }
    }
}
