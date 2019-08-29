#ifndef BARRAMENTOCOMUNICACAO_H
#define BARRAMENTOCOMUNICACAO_H

#include "ProtocoloSerial.h"
#include "grSim_Packet.pb.h"
#include "messages_ai_data_manager.pb.h"

class CommunicationBUS
{
private:
    vector<ProtocoloSerial*> pacoteSerial;  ///< vetor de pacotes contendo um para cada robo para enviar aos robos
    vector<grSim_Packet*> pacoteSim; ///< vetor de pacotes contendo um para cada robo para enviar ao simulador
    AIDataManagerPackage* pacoteIaMonitorador; ///< Pacote que sera enviado para ser monitorado no gerente de dados

public:
    CommunicationBUS();
    ~CommunicationBUS();

    void setPacoteRobo(int id, const ProtocoloSerial&);
    void limparPacoteRobo(int id);
    void setPacoteRobo(int id, const grSim_Packet&);
    void carregaPacoteMonitorador(const AIDataManagerPackage& _pacoteIaMonitorador); ///< Carrega apenas se a monitoracao estiver habilitada

    ProtocoloSerial* getPacoteSerial(int id);
    grSim_Packet* getPacoteSimulador(int id);
    AIDataManagerPackage* getPacoteMonitorador();
};

#endif // BARRAMENTOCOMUNICACAO_H
