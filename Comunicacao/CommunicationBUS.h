#ifndef BARRAMENTOCOMUNICACAO_H
#define BARRAMENTOCOMUNICACAO_H

#include <grSim-proto/grSim_Packet.pb.h>
#include <furgbol-core/proto/messages_ai_data_manager.pb.h>

#include "ProtocoloSerial.h"

class CommunicationBUS
{
private:
    vector<ProtocoloSerial*> pacoteSerial;  ///< vetor de pacotes contendo um para cada robo para enviar aos robos
    vector<grSim_Packet*> pacoteSim; ///< vetor de pacotes contendo um para cada robo para enviar ao simulador
    AIDataManagerPackage* pacoteIaMonitorador; ///< Pacote que sera enviado para ser monitorado no gerente de dados

    //vector<furgbol::io::F180SerialMessage*> serial_messages;
public:
    CommunicationBUS();
    ~CommunicationBUS();    
    ProtocoloSerial* getPacoteSerial(int id);
    //furgbol::io::F180SerialMessage* getPacoteSerial(size_t);
    grSim_Packet* getPacoteSimulador(int id);
    AIDataManagerPackage* getPacoteMonitorador();
    void setPacoteRobo(int id, const ProtocoloSerial&);
    void setPacoteRobo(int id, const grSim_Packet&);
    void limparPacoteRobo(int id);
    void carregaPacoteMonitorador(const AIDataManagerPackage& _pacoteIaMonitorador); /// Carrega apenas se a monitoracao estiver habilitada.

    //void setPacoteRobo(size_t, furgbol::io::F180SerialMessage); /// Sobrecarga dos métodos anteriores.
};

#endif // BARRAMENTOCOMUNICACAO_H
