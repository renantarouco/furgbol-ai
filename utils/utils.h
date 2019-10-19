#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <thread>

#define DEBUG(S) std::cout << "[" << std::this_thread::get_id() << "]: " << S << std::endl

#endif // UTILS_H
