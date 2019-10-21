#ifndef MONITOR_COMMUNICATOR_H
#define MONITOR_COMMUNICATOR_H

#include <memory>

#include <furgbol-core/io/udp_sender.h>

#include "monitor_repository.h"

class MonitorCommunicator
{
public:
    MonitorCommunicator(std::shared_ptr<MonitorRepository>, std::shared_ptr<bool>);
    void init();
    void run();
private:
    std::shared_ptr<MonitorRepository> repository_;
    furgbol::io::UDPSender writer_;
    std::shared_ptr<bool> running_;
};

#endif // MONITOR_COMMUNICATOR_H
