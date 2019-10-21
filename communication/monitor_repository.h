#ifndef MONITOR_REPOSITORY_H
#define MONITOR_REPOSITORY_H

#include <mutex>
#include <vector>

#include <furgbol-core/proto/messages_ai_data_manager.pb.h>

class MonitorRepository
{
public:
    MonitorRepository(size_t);
    bool is_package_sent();
    void set_package_sent();
    AIDataManagerPackage packet();
    void packet(AIDataManagerPackage);
    AIRobotPackage package(size_t);
    void package(size_t, AIRobotPackage);
private:
    std::mutex mutex_;
    AIDataManagerPackage packet_;
    bool package_sent_;
};

#endif // MONITOR_REPOSITORY_H
