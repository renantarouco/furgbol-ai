#include "serial_communicator.h"

#include <thread>

#include "configuration/configuration.h"

SerialCommunicator::SerialCommunicator(std::shared_ptr<SerialRepository> repository, std::shared_ptr<bool> running) :
    repository_(repository),
    writer_(Configuration::SERIAL_PORT, Configuration::SERIAL_PACKAGE_SIZE),
    running_(running)
{

}

void SerialCommunicator::init()
{
    writer_.init(
      furgbol::io::BaudRate::BAUD_500000,
      furgbol::io::CharSize::CHAR_SIZE_8,
      furgbol::io::Parity::PARITY_NONE,
      furgbol::io::StopBits::STOP_BITS_1,
      furgbol::io::FlowControl::FLOW_CONTROL_NONE);
}

void SerialCommunicator::run()
{
    while (*running_) {
        for (size_t i = 0; i < 6; ++i) {
            F180SerialPackage package = repository_->package(i);
            if (!repository_->is_package_sent(i)) {
                writer_.write(package);
                repository_->set_package_sent(i);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
}
