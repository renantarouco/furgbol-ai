#include "monitor_repository.h"

MonitorRepository::MonitorRepository(size_t n_robots) :
    mutex_(),
    packet_(),
    package_sent_(true)
{
    for (size_t i = 0; i < n_robots; ++i) {
        AIRobotPackage* command = packet_.add_team();
        command->set_id(static_cast<google::protobuf::uint32>(i));
    }
}

bool MonitorRepository::is_package_sent()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return package_sent_;
}

void MonitorRepository::set_package_sent()
{
    std::lock_guard<std::mutex> lock(mutex_);
    package_sent_ = true;
}

AIDataManagerPackage MonitorRepository::packet()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return packet_;
}

void MonitorRepository::packet(AIDataManagerPackage packet)
{
    std::lock_guard<std::mutex> lock(mutex_);
    packet_.CopyFrom(packet);
}

AIRobotPackage MonitorRepository::package(size_t id)
{
  std::lock_guard<std::mutex> lock(mutex_);
  return packet_.team(static_cast<int>(id));
}

void MonitorRepository::package(size_t id, AIRobotPackage command)
{
  std::lock_guard<std::mutex> lock(mutex_);
  AIRobotPackage* command_ = packet_.mutable_team(static_cast<int>(id));
  command_->CopyFrom(command);
  package_sent_ = false;
}
