#include "grsim_communicator.h"

#include "configuration/configuration.h"
#include "utils/utils.h"

GrSimCommunicator::GrSimCommunicator(std::shared_ptr<GrSimRepository> repository, std::shared_ptr<bool> running) :
    repository_(repository),
    writer_(Configuration::GRSIM_IP, Configuration::GRSIM_PORT),
    running_(running)
{

}

void GrSimCommunicator::init()
{

}

void GrSimCommunicator::run()
{
    while (*running_) {
        if (!repository_->is_package_sent()) {
            grSim_Packet packet = repository_->packet();
            std::string datagram;
            packet.SerializeToString(&datagram);
            writer_.send(datagram);
            repository_->set_package_sent();
            DEBUG("sent grsim packet");
        }
    }
}
