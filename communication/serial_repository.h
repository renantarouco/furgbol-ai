#ifndef SERIAL_REPOSITORY_H
#define SERIAL_REPOSITORY_H

#include <mutex>
#include <vector>
#include <iostream>

#include "f180_serial_package.h"

class SerialRepository
{
public:
  SerialRepository(size_t);
  ~SerialRepository();
  bool is_package_sent(size_t);
  void set_package_sent(size_t);
  F180SerialPackage package(size_t);
  void package(size_t, F180SerialPackage);
private:
  std::mutex mutex_;
  std::vector<F180SerialPackage*> packages_;
  std::vector<bool> packages_sent_;
};

#endif // SERIAL_REPOSITORY_H
