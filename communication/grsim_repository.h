#ifndef GRSIM_REPOSITORY_H
#define GRSIM_REPOSITORY_H

#include <mutex>
#include <vector>

#include <grSim-proto/grSim_Packet.pb.h>

class GrSimRepository
{
public:
    GrSimRepository(size_t);
    bool is_package_sent();
    void set_package_sent();
    grSim_Packet packet();
    void packet(grSim_Packet);
    grSim_Robot_Command package(size_t);
    void package(size_t, grSim_Robot_Command);
private:
    std::mutex mutex_;
    grSim_Packet packet_;
    bool package_sent_;
};

#endif // GRSIM_REPOSITORY_H
