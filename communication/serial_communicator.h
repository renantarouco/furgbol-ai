#ifndef SERIAL_COMMUNICATOR_H
#define SERIAL_COMMUNICATOR_H

#include <memory>

#include <furgbol-core/io/serial_writer.h>

#include "serial_repository.h"

class SerialCommunicator
{
public:
    SerialCommunicator(std::shared_ptr<SerialRepository>, std::shared_ptr<bool>);
    void init();
    void run();
private:
    std::shared_ptr<SerialRepository> repository_;
    furgbol::io::SerialWriter writer_;
    std::shared_ptr<bool> running_;
};

#endif // SERIAL_COMMUNICATOR_H
