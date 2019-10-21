#ifndef REPOSITORYPROXY_H
#define REPOSITORYPROXY_H

#include <memory>

#include <grSim-proto/grSim_Packet.pb.h>
#include <furgbol-core/proto/messages_ai_data_manager.pb.h>

#include "serial_repository.h"
#include "grsim_repository.h"
#include "monitor_repository.h"
#include "f180_serial_package.h"

class RepositoryProxy
{
public:
    RepositoryProxy(
        std::shared_ptr<SerialRepository>,
        std::shared_ptr<GrSimRepository>,
        std::shared_ptr<MonitorRepository>);
    void set_package(size_t id, F180SerialPackage package) { serial_repo_->package(id, package); }
    void set_package(size_t id, grSim_Robot_Command package) { grsim_repo_->package(id, package); }
    void set_package(size_t id, AIRobotPackage package) { monitor_repo_->package(id, package); }
    void set_packet(AIDataManagerPackage packet) { monitor_repo_->packet(packet); }
    void set_packet(grSim_Packet packet) { grsim_repo_->packet(packet); }
private:
    std::shared_ptr<SerialRepository> serial_repo_;
    std::shared_ptr<GrSimRepository> grsim_repo_;
    std::shared_ptr<MonitorRepository> monitor_repo_;
};

#endif // REPOSITORYPROXY_H
