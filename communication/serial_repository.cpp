#include "serial_repository.h"

SerialRepository::SerialRepository(size_t n_robots) :
  mutex_(),
  packages_(n_robots, new F180SerialPackage),
  packages_sent_(n_robots, true)
{
    for (size_t i = 0; i < n_robots; ++i) {
        packages_[i]->robot_id(static_cast<uint8_t>(i));
    }
}

SerialRepository::~SerialRepository()
{
  for (size_t i = 0; i < packages_.size(); ++i) {
    delete packages_[i];
  }
}

bool SerialRepository::is_package_sent(size_t id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return packages_sent_[id];
}

void SerialRepository::set_package_sent(size_t id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    packages_sent_[id] = true;
}

F180SerialPackage SerialRepository::package(size_t id)
{
  std::lock_guard<std::mutex> lock(mutex_);
  return *packages_[id];
}

void SerialRepository::package(size_t id, F180SerialPackage package)
{
  std::lock_guard<std::mutex> lock(mutex_);
  packages_[id]->robot_id(package.robot_id());
  packages_[id]->velocity(
    package.x_vel(),
    package.y_vel(),
    package.theta_vel());
  packages_[id]->direction(
    package.x_direction(),
    package.y_direction(),
    package.theta_direction());
  packages_[id]->dribbler(package.dribbler());
  packages_[id]->kick(package.kick());
  packages_sent_[id] = false;
}
