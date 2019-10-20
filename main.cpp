#include <iostream>
#include <thread>
#include <chrono>

#include <QCoreApplication>
#include "Sistema.h"
#include "Comunicador.h"
#include "Carregador.h"

#include <furgbol-core/io/serial_writer.h>

//#include "f180_serial_package.h"
//#include "serial_repository.h"

#include "system/system.h"

int main(int argc, char* argv[])
{
//    /// carregando as constantes
//    Config config;
//    config.load();

    ConfigComunicacao::load();
    Config::load();

    //Alterei
    /// variáveis utilizadas na IA
    QMutex mBUS; ///< Mutex para controlar o acesso ao objeto communcation bus
    CommunicationBUS bus; /// barramento utilizado na comunicação entre a thread principal e a de comunicacao;


    /// iniciando a thread do Sistema
//    Sistema sistema;
//    sistema.init(&mBUS, &bus, serial_repo);
//    sistema.start();

//    /// iniciando a thread do comunicador que é aonde iremos enviar os pacotes para os robos ou simuladores
//    Comunicador comunicador;
//    comunicador.init(&mBUS, &bus);
//    comunicador.start();

    /// loop da thread do main
    //a.exec();

    /// parando as threads
//    sistema.stop();
    //comunicador.stop();
    System system;
    system.init(argc, argv);
    system.run();
    return 0;
}
