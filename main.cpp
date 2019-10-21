#include <thread>

#include <furgbol-core/io/udp_receiver.h>
#include <furgbol-core/proto/messages_data_manager.pb.h>

#include "system/system.h"
#include "utils/utils.h"

#include <QCoreApplication>
#include "Sistema/Sistema.h"

int main(int argc, char* argv[])
{
    ConfigComunicacao::load();
    Config::load();
    if (argc >= 2) {
        std::string config_path(argv[1]);
        Configuration::load(config_path);
    } else {
        DEBUG("missing config path parameter");
        exit(1);
    }

    System system;
    system.init();

    Sistema legacy_system_;
    legacy_system_.init(system.repository());
    std::thread legacy_system_thread(&Sistema::run, &legacy_system_);

    std::thread system_thread(&System::run, &system);

    system_thread.join();
    legacy_system_thread.join();
    return 0;
}
