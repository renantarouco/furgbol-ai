#ifndef F180_SERIAL_PACKAGE_H
#define F180_SERIAL_PACKAGE_H

#include <vector>
#include <tuple>

#include <furgbol-core/io/serial_package.h>

class F180SerialPackage final : public furgbol::io::SerialPackage
{
public:
  enum Direction { NEGATIVE = 1, POSITIVE = 3 };

  // Constructor
  F180SerialPackage() : furgbol::io::SerialPackage() {}

  // Getters
  uint8_t robot_id() const { return robot_id_; }
  uint8_t x_vel() const { return std::get<0>(velocity_); }
  uint8_t y_vel() const { return std::get<1>(velocity_); }
  uint8_t theta_vel() const { return std::get<2>(velocity_); }
  Direction x_direction() const { return std::get<0>(direction_); }
  Direction y_direction() const { return std::get<1>(direction_); }
  Direction theta_direction() const { return std::get<2>(direction_); }
  uint8_t dribbler() const { return dribbler_; }
  uint8_t kick() const { return kick_; }

  //Setters
  void robot_id(uint8_t robot_id) { robot_id_ = robot_id; }
  void x_vel(uint8_t x_vel) { std::get<0>(velocity_) = x_vel; }
  void y_vel(uint8_t y_vel) { std::get<1>(velocity_) = y_vel; }
  void theta_vel(uint8_t theta_vel) { std::get<2>(velocity_) = theta_vel; }
  void x_direction(Direction x_direction) { std::get<0>(direction_) = x_direction; }
  void y_direction(Direction y_direction) { std::get<1>(direction_) = y_direction; }
  void theta_direction(Direction theta_direction) { std::get<2>(direction_) = theta_direction; }
  void dribbler(uint8_t dribbler) { dribbler_ = dribbler; }
  void kick(uint8_t kick) { kick_ = kick; }

  void velocity(uint8_t, uint8_t, uint8_t);
  void direction(Direction, Direction, Direction);

  void serialize(std::vector<uint8_t>&) override;
  void serialize(std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator) override;
private:
  uint8_t robot_id_;
  std::tuple<uint8_t, uint8_t, uint8_t> velocity_;
  std::tuple<Direction, Direction, Direction> direction_;
  uint8_t dribbler_;
  uint8_t kick_;
};

#endif // F180_SERIAL_PACKAGE_H
