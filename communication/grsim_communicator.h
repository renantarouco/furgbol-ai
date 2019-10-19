#ifndef GRSIM_COMMUNICATOR_H
#define GRSIM_COMMUNICATOR_H

#include <memory>

#include <QUdpSocket>
#include <grSim-proto/grSim_Packet.pb.h>

#include "grsim_repository.h"

class GrSimCommunicator
{
public:
    GrSimCommunicator(std::shared_ptr<GrSimRepository>, std::shared_ptr<bool>);
    void init();
    void run();
private:
    std::shared_ptr<GrSimRepository> repository_;
    QHostAddress ip_;
    unsigned short port_;
    QUdpSocket writer_;
    std::shared_ptr<bool> running_;
};

#endif // GRSIM_COMMUNICATOR_H
