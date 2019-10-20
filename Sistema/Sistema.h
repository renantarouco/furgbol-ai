#ifndef SISTEMA_H
#define SISTEMA_H

#include <memory>

#include <furgbol-core/proto/messages_ai_2Dsimulator.pb.h>

#include "GerenteDeliberativoCentral.h"
#include "Agente.h"
#include "Avaliador.h"
#include "CommunicationBUS.h"
#include "Relogio.h"
#include "ModeloMundo.h"
#include "DefendePonto.h"
#include "Chuta.h"
#include "Carregador.h"
#include "Definitions.h"

#include <QUdpSocket>

#include "communication/serial_repository.h"

/*!
 * Classe que controla a execução do programa através de comandos da interface
 */

class Sistema : public QThread {
    Q_OBJECT

public:
    static ModeloMundo modeloMundo;
    static Avaliador avaliador;

private:
    GerenteDC gerenteCentral;
    vector<Agente> agentes; /// O número de agentes depende de quantos foram enviados pelo gerente de dados
    unsigned int numAgentesAtivos;   /// < Variável contém o número de agentes em atividade para um dado momento na partida.

    QUdpSocket r_udpSocket; ///< receptor usado para receber pacotes de outros processos
    QMutex* mBUS; ///< mutex usado para controlar o acesso do objeto bus
    CommunicationBUS*  bus; ///< Barramento utilizado para comunicar a thread principal com a thread de comunicação.

    /// variáveis auxiliares usadas na execução do sistema
    Relogio relogio;
    map<int, vector< Tatica*> > papeisRobos;
    int iteracao;
    AI2DSimulatorPackage pacoteSimulador2D;
    DataPackage pacoteGD;
    QByteArray datagram;
public slots:
    void processPacotes();
public:
    Sistema();
    ~Sistema();
    void tratarPacoteRecebido();
    void addPacoteMonitoradorBUS();
    void setPapeisAgentes(map<int, vector<Tatica*> > & papeis);
    void init(QMutex* _mBUS, CommunicationBUS* _barramentoComunicacao);
    void run();
    void stop();
};

#endif // SISTEMA_H
