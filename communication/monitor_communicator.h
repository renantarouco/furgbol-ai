#ifndef MONITOR_COMMUNICATOR_H
#define MONITOR_COMMUNICATOR_H

#include <memory>

#include <QUdpSocket>

#include "monitor_repository.h"

class MonitorCommunicator
{
public:
    MonitorCommunicator(std::shared_ptr<MonitorRepository>, std::shared_ptr<bool>);
    void init();
    void run();
private:
    std::shared_ptr<MonitorRepository> repository_;
    QHostAddress ip_;
    unsigned short port_;
    QUdpSocket writer_;
    std::shared_ptr<bool> running_;
};

#endif // MONITOR_COMMUNICATOR_H
