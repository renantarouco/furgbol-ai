#include "grsim_communicator.h"

#include "configuration/configuration.h"
#include "utils/utils.h"

GrSimCommunicator::GrSimCommunicator(std::shared_ptr<GrSimRepository> repository, std::shared_ptr<bool> running) :
    repository_(repository),
    ip_(Configuration::GRSIM_IP.c_str()),
    port_(Configuration::GRSIM_PORT),
    writer_(),
    running_(running)
{

}

void GrSimCommunicator::init()
{

}

void GrSimCommunicator::run()
{
    while (*running_) {
        grSim_Packet packet = repository_->packet();
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
