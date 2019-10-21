#ifndef SYSTEM_H
#define SYSTEM_H

#include <memory>
#include <thread>

#include "communication/serial_repository.h"
#include "communication/grsim_repository.h"
#include "communication/monitor_repository.h"
#include "communication/repository_proxy.h"
#include "communication/serial_communicator.h"
#include "communication/grsim_communicator.h"
#include "communication/monitor_communicator.h"

class System
{
public:
    System();
    void init();
    void run();
    std::shared_ptr<RepositoryProxy> repository() { return repository_proxy_; }
private:
    std::shared_ptr<bool> running_;
    std::shared_ptr<SerialRepository> serial_repo_;
    std::shared_ptr<GrSimRepository> grsim_repo_;
    std::shared_ptr<MonitorRepository> monitor_repo_;
    std::shared_ptr<RepositoryProxy> repository_proxy_;
    std::unique_ptr<SerialCommunicator> serial_comm_;
    std::unique_ptr<GrSimCommunicator> grsim_comm_;
    std::unique_ptr<MonitorCommunicator> monitor_comm_;
    std::unique_ptr<std::thread> serial_comm_thread_;
    std::unique_ptr<std::thread> grsim_comm_thread_;
    std::unique_ptr<std::thread> monitor_comm_thread_;
};

#endif // SYSTEM_H
