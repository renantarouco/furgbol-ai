#include "grsim_repository.h"

GrSimRepository::GrSimRepository(size_t n_robots) :
  mutex_(),
  packet_(),
  package_sent_(true)
{
    for (size_t i = 0; i < n_robots; ++i) {
        grSim_Robot_Command* command = packet_.mutable_commands()->add_robot_commands();
        command->set_id(static_cast<google::protobuf::uint32>(i));
    }
}

bool GrSimRepository::is_package_sent()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return package_sent_;
}

void GrSimRepository::set_package_sent()
{
    std::lock_guard<std::mutex> lock(mutex_);
    package_sent_ = true;
}

grSim_Packet GrSimRepository::packet()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return packet_;
}

grSim_Robot_Command GrSimRepository::package(size_t id)
{
  std::lock_guard<std::mutex> lock(mutex_);
  return packet_.commands().robot_commands(static_cast<int>(id));
}

void GrSimRepository::package(size_t id, grSim_Robot_Command command)
{
  std::lock_guard<std::mutex> lock(mutex_);
  grSim_Robot_Command* command_ = packet_.mutable_commands()->mutable_robot_commands(static_cast<int>(id));
  command_->CopyFrom(command);
  package_sent_ = false;
}