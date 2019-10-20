#ifndef AGENTE_H
#define AGENTE_H

#include <memory>

#include <AvaliadorRobo.h>
#include "CommunicationBUS.h"
#include <Tatica.h>
#include <Montador.h>
#include <Comunicador.h>
#include <ProtocoloSerial.h>
#include <Navegador.h>

#include "communication/serial_repository.h"

/**
 * Classe que guarda as informações de um processo referente a um robô.
 * Esta classe possue as táticas que o robô deve executar, e gerencia a execução também da máquina de estado gerada pela tática.
 * Também, se der tempo , vai guardar o desempenho deste robo para cada tatica e assim poder avaliar se aquele papel é um papel adequado.
 * O robo deve determinar se a tatica acabou, ou seja se ela cumpriu seu objetivo pra isso ele executa uma função da propria tatica.
 * Ele tambem pode antes de pegar uma tatica da fila ver se aquela tatica realmente vai ser bem aplicada, isso pode acontecer sempre ou de tantas em tantas iterações.
 */
class Agente
{
protected:
    unsigned int id;
    QMutex* mBUS;
    CommunicationBUS* bus;
    int iteracoes;
    vector<Tatica*> filaTaticasPen;
    unsigned int posFila; /// Especifica a posicao atual sendo utilizada na fila de taticas pendentes
    /// Variáveis usadas para o path planning.
    Navegador navegador; /// Navegador usado para calcular o path planning dos robos para que não se colidam.
    bool navegadorPronto;
    Montador montador; /// Objeto usado para calcular a velocidade que o robô tem que andar.
public:
    Agente();
    ~Agente();
    void init(int _id, QMutex* _mBUS, CommunicationBUS* _bus);
    unsigned int getId();
    void executarTatica();
    bool atualizarTatica();
    void limparFilaTaticas();
    /** Funções que são usadas para ativar e desativar o navegador */
    void ativarObstaculosEq();
    void desativarObstaculosEq();
    void ativarObstaculosAdv();
    void desativarObstaculosAdv();
    void addAreaEqObstaculo();
    void addAreaAdvObstaculo();
    void addBolaComoObstaculo(float dist=150.0);
    void setPapel(vector<Tatica*>& _taticasPen); /// Metodo da parte sequencial que seta a fila do agente.
    bool isFilaTaticasVazia();
    void setPosicao(int pos); /// Metodo para setar a posicao atual da fila que o agente está usando.
    int getPosicao(); /// Metodo para obter a posicao atual na fila que o agente está utilizando.
    Tatica* getTaticaCorrente(); /// Obtem a tatica que o agente esta usando no momento.
    /**
        Usa o navegador para gerar um alvo intermediario
        @param o ponto onde se deseja chegar
        @return o ponto onde se deseja ir.
    */
    Ponto calcAlvoFromPathPlanning(const Ponto& destino);
    vector<Ponto> calcAlvosFromPathPlanning(const Ponto &destino);
    bool isPresente();
    /**
        @brief funcao utilizada para adicionar os pacotes que a thread de comunicacao vai se preocupar em enviar
         Aqui vai ser necessário colocar um mutex read/write.
    */
    void addPacoteBUS();
    void limparPacote();
    void setNavegador(Navegador _navegador);
    Navegador* getNavegador();
    // Funcoes de atribuição e impressao.
    void operator=(const Agente& a);
    friend ostream& operator<<(ostream& os, const Agente& a);
};
#endif
