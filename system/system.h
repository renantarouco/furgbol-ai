#ifndef SYSTEM_H
#define SYSTEM_H

#include <memory>

#include "communication/serial_repository.h"
#include "communication/grsim_repository.h"
#include "communication/monitor_repository.h"
#include "communication/serial_communicator.h"
#include "communication/grsim_communicator.h"
#include "communication/monitor_communicator.h"

class System
{
public:
    System();
    void init(int, char**);
    void run();
private:
    std::shared_ptr<bool> running_;
    std::shared_ptr<SerialRepository> serial_repo_;
    std::shared_ptr<GrSimRepository> grsim_repo_;
    std::shared_ptr<MonitorRepository> monitor_repo_;
    std::shared_ptr<SerialCommunicator> serial_comm_;
    std::shared_ptr<GrSimCommunicator> grsim_comm_;
    std::shared_ptr<MonitorCommunicator> monitor_comm_;
};

#endif // SYSTEM_H
