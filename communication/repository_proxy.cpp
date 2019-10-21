#include "repository_proxy.h"

#include "configuration/configuration.h"

RepositoryProxy::RepositoryProxy(
    std::shared_ptr<SerialRepository> serial_repo,
    std::shared_ptr<GrSimRepository> grsim_repo,
    std::shared_ptr<MonitorRepository> monitor_repo)
{
    serial_repo_ = serial_repo;
    grsim_repo_ = grsim_repo;
    monitor_repo_ = monitor_repo;
}
