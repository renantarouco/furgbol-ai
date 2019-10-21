#ifndef SISTEMA_H
#define SISTEMA_H

#include <memory>

#include <furgbol-core/io/udp_receiver.h>
#include <furgbol-core/proto/messages_ai_2Dsimulator.pb.h>
#include <furgbol-core/proto/messages_data_manager.pb.h>

#include "communication/repository_proxy.h"

// LEGACY CODE
#include "GerenteDeliberativoCentral.h"
#include "Agente.h"
#include "Avaliador.h"
#include "Relogio.h"
#include "ModeloMundo.h"
#include "DefendePonto.h"
#include "Chuta.h"
#include "Carregador.h"
#include "Definitions.h"

/*!
 * Classe que controla a execução do programa através de comandos da interface
 */

class Sistema {

public:
    static ModeloMundo modeloMundo;
    static Avaliador avaliador;
    Sistema();
    void tratarPacoteRecebido();
    void addPacoteMonitoradorBUS();
    void setPapeisAgentes(map<int, vector<Tatica*> > & papeis);
    void init(std::shared_ptr<RepositoryProxy>);
    void run();
private:
    std::shared_ptr<RepositoryProxy> repository_;
    furgbol::io::UDPReceiver reader_;
    GerenteDC gerenteCentral;
    vector<Agente> agentes; /// O número de agentes depende de quantos foram enviados pelo gerente de dados
    unsigned int numAgentesAtivos;   /// < Variável contém o número de agentes em atividade para um dado momento na partida.
    /// variáveis auxiliares usadas na execução do sistema
    Relogio relogio;
    map<int, vector< Tatica*> > papeisRobos;
    int iteracao;
    AI2DSimulatorPackage pacoteSimulador2D;
    DataPackage pacoteGD;
    void processPacotes();
};

#endif // SISTEMA_H
