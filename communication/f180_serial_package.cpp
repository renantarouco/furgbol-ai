#include "f180_serial_package.h"

#include <stdexcept>

void F180SerialPackage::velocity(uint8_t x, uint8_t y, uint8_t theta)
{
  x_vel(x);
  y_vel(y);
  theta_vel(theta);
}

void F180SerialPackage::direction(Direction x, Direction y, Direction theta)
{
  x_direction(x);
  y_direction(y);
  theta_direction(theta);
}

void F180SerialPackage::serialize(std::vector<uint8_t>& serialization_buffer)
{
  if (serialization_buffer.size() >= 9) {
    serialization_buffer[0] = robot_id_;
    serialization_buffer[1] = std::get<0>(velocity_);
    serialization_buffer[2] = std::get<1>(velocity_);
    serialization_buffer[3] = std::get<2>(velocity_);
    serialization_buffer[4] = static_cast<uint8_t>(std::get<0>(direction_));
    serialization_buffer[5] = static_cast<uint8_t>(std::get<1>(direction_));
    serialization_buffer[6] = static_cast<uint8_t>(std::get<2>(direction_));
    serialization_buffer[7] = dribbler_;
    serialization_buffer[8] = kick_;
  } else {
    throw furgbol::io::errors::SerializeSizeError();
  }
}

void F180SerialPackage::serialize(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end)
{
  if (std::distance(begin, end) >= 9) {
    *(begin + 0) = robot_id_;
    *(begin + 1) = std::get<0>(velocity_);
    *(begin + 2) = std::get<1>(velocity_);
    *(begin + 3) = std::get<2>(velocity_);
    *(begin + 4) = static_cast<uint8_t>(std::get<0>(direction_));
    *(begin + 5) = static_cast<uint8_t>(std::get<1>(direction_));
    *(begin + 6) = static_cast<uint8_t>(std::get<2>(direction_));
    *(begin + 7) = dribbler_;
    *(begin + 8) = kick_;
  } else {
    throw furgbol::io::errors::SerializeSizeError();
  }
}
