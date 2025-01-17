#include "Comunicador.h"

Comunicador::Comunicador() //: serial_sender(ConfigComunicacao::DISPOSITIVO_SERIAL)
{
}

void Comunicador::init(QMutex* _mBUS, CommunicationBUS * _bus)
{
    cout << "INICIANDO COMUNICADOR" << endl;

    mBUS = _mBUS;
    bus = _bus;

    /// verificando se iremos abrir um comunicação pela serial
    switch(ConfigComunicacao::TIPO_ROBOS){
    case REAL:
        transSerial.setDispositivo(ConfigComunicacao::DISPOSITIVO_SERIAL);
        if(!transSerial.open()){
            cout << "Nao foi possivel abrir a porta pra comunicar com a serial" << endl;
        }
        break;

    }
}

void Comunicador::stop()
{
    cout << "FINALIZANDO COMUNICADOR" << endl << endl;

    jogando = false;
    quit();
    wait();
}

void Comunicador::run()
{
    Relogio temporizador;
    jogando = true;

    int ultimoRoboEnviado = 0; /// Guarda ultimo robô que recebeu um pacote para manter a justica entre o uso
    bool passou = false; /// variavel auxiliar para indicar que ja testou todos os robos

    /// calcula o tempo necessario para se esperar apos o envio em cada interface.
    double tempoEnvioSerial = 1.0/(double) ConfigComunicacao::NUM_PACOTES_SERIAL;
    double tempoEnvioMonitorador = 1.0/(double) ConfigComunicacao::NUM_PACOTES_MONITORADOR;
    double tempoEnvioSimulador = 1.0/(double) ConfigComunicacao::NUM_PACOTES_SIMULADOR;

    /// Aqui são as variaveis que guardam o momento do ultimo envio realizado para cada interface
    double ultimoEnvioSerial = temporizador.getTempoPassado() -  tempoEnvioSerial;
    double ultimoEnvioMonitorador = temporizador.getTempoPassado() - tempoEnvioMonitorador;
    double ultimoEnvioSimulador = temporizador.getTempoPassado()- tempoEnvioSimulador;

    /// verificando quais processos iremos nos comunicar
    bool enviarRobos = ConfigComunicacao::TIPO_ROBOS == REAL;

    frequency_ = std::chrono::duration<float>(static_cast<float>(1.0 / ConfigComunicacao::NUM_PACOTES_SERIAL));
    std::chrono::system_clock::time_point compare_time = std::chrono::high_resolution_clock::now();

    cout << "STARTING COMUNICADOR" << endl;

    while(jogando){

        /// comeca a tentar enviar a partir do ultimo robo enviado.
        /// e testa todos os outros dos 5 se nao enviar para nenhum continua tentando na proxima sem
        /// esperar, mas se enviar para algum, espera
        if(enviarRobos){

//            passou=false;
//            for(int id=((ultimoRoboEnviado+1) % NUM_MAX_ROBOS); id!=(ultimoRoboEnviado+1) || !passou; id = ((id+1) % NUM_MAX_ROBOS)){
//            //for (size_t id = 0; id < NUM_MAX_ROBOS; id++) {
//                /// verificando se passou do tempo limite para esperar para enviar outro pacote
//                if ((std::chrono::high_resolution_clock::now() - compare_time) >= frequency_) {
//                //if(temporizador.getTempoPassado()-ultimoEnvioSerial > tempoEnvioSerial){

//                    mBUS->lock();
//                    //ProtocoloSerial* pacoteSerial = bus->getPacoteSerial(id); /// o devolvido é uma copia em formato de ponteiro, não é o endereço.
//                    //furgbol::io::F180SerialMessage* serial_message = bus->getPacoteSerial(static_cast<size_t>(id));
//                    mBUS->unlock();
//                    passou=true;
////                    if(pacoteSerial != NULL){
////                        ultimoRoboEnviado = id;
////                        ultimoEnvioSerial = temporizador.getTempoPassado();
////                        send(*pacoteSerial);
////                        delete pacoteSerial;
////                        compare_time = std::chrono::high_resolution_clock::now();
////                        break;
////                    }
////                    if (serial_message != nullptr) {
////                        std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< VAIENVIAR" << std::endl;
////                        ultimoRoboEnviado = id;
////                        ultimoEnvioSerial = temporizador.getTempoPassado();
////                        send(*serial_message);
////                        delete serial_message;
////                        compare_time = std::chrono::high_resolution_clock::now();
////                        break;
////                    }
//                }
//            }
        }else{

//            /// Enviando pro simulador
//            if(temporizador.getTempoPassado() - ultimoEnvioSimulador > tempoEnvioSimulador){
//                for(int id=((ultimoRoboEnviado +1) % NUM_MAX_ROBOS); id != ultimoRoboEnviado; id=((id+1) % NUM_MAX_ROBOS)){
//                    mBUS->lock();
//                    grSim_Packet* pacoteSimulador = bus->getPacoteSimulador(id);
//                    mBUS->unlock();
//                    if(pacoteSimulador != NULL ){
//                        ultimoRoboEnviado = id;
//                        ultimoEnvioSimulador = temporizador.getTempoPassado();
//                        send(*pacoteSimulador);
//                        delete pacoteSimulador;
//                        break;
//                    }
//                }
//            }
        }

        /// Enviando pro monitorador
        if(ConfigComunicacao::ENVIA_MONITORADOR && temporizador.getTempoPassado() - ultimoEnvioMonitorador > tempoEnvioMonitorador){

            mBUS->lock();
            AIDataManagerPackage* pacoteMonitorador = bus->getPacoteMonitorador();
            mBUS->unlock();

            if(pacoteMonitorador != NULL){

                //                cout << "enviando pacote simulador" << endl;

                ultimoEnvioMonitorador = temporizador.getTempoPassado();
                send(*pacoteMonitorador);
                delete pacoteMonitorador;
            }
        }
    }
}

void Comunicador::send(const AIDataManagerPackage& _pacoteMonitorador)
{
    /// serializando o pacote do google protobuffer
    string buffer;
    _pacoteMonitorador.SerializeToString(&buffer);

    /// enviando o pacote para o monitorador
    if (s_udpSocket.writeDatagram(buffer.c_str(), buffer.length(), ConfigComunicacao::ADDRESS_MONITORADOR.host, ConfigComunicacao::ADDRESS_MONITORADOR.porta) == -1)
        cout << " Envio com problemas para monitorador" << endl;
}

void Comunicador::send(const grSim_Packet& _pacoteSimulador)
{
    /// serializando o pacote do google protobuffer
    string buffer;
    _pacoteSimulador.SerializeToString(&buffer);

    /// enviando o pacote para o monitorador
    if (s_udpSocket.writeDatagram(buffer.c_str(), buffer.length(), ConfigComunicacao::ADDRESS_SIMULADOR.host, ConfigComunicacao::ADDRESS_SIMULADOR.porta) == -1)
        cout << " Envio com problemas para monitorador" << endl;
}

void Comunicador::send(const ProtocoloSerial& _pacoteRobo)
{
    cout << "<<<<<<<<<<<<<<<<<<<< ENVIANDOO" << endl;
    if (!transSerial.write(_pacoteRobo)) {
        cout << "Comunicador não conseguiu escrever na serial" << endl;
        return;
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ENVIOU" << endl;
}

//void Comunicador::send(furgbol::io::F180SerialMessage& serial_message) {
//    std::vector<uint8_t> buffer(9, 0);
//    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< VAI SERIALIZAR" << std::endl;
//    serial_message.serialize(buffer);
//    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SERIALIZOU " << static_cast<int>(buffer[0]) << std::endl;
//    serial_sender.send(buffer);
//    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ENVIOU REAL " << static_cast<int>(serial_message.getRobotId()) << std::endl;
//}
