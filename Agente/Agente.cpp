#include "Agente.h"

#include <string>

#include<DefendeGol.h>
#include<Sistema.h>

#include "communication/f180_serial_package.h"
#include "configuration/configuration.h"
#include "utils/utils.h"

Agente::Agente()
{
    id = 0;
    posFila = 0;
    iteracoes = 0;
}

Agente::~Agente(){
}

void Agente::init(int _id, std::shared_ptr<RepositoryProxy> repository)
{
    DEBUG("initializing agent " + std::to_string(_id));
    id = static_cast<unsigned int>(_id);
    montador.setId(static_cast<int>(id));
    navegador.setId(static_cast<int>(id));
    navegador.ativar();
    repository_ = repository;
}

void Agente::limparFilaTaticas(){
    filaTaticasPen.clear();
}

void Agente::ativarObstaculosEq(){
    navegador.ativaObstaculosRobosEq();
}

void Agente::ativarObstaculosAdv(){
    navegador.ativaObstaculosRobosEq();
}

void Agente::desativarObstaculosEq(){
    navegador.desativaObstaculosRobosEq();
}

void Agente::desativarObstaculosAdv(){
    navegador.desativaObstaculosRobosAdv();
}

void Agente::addAreaEqObstaculo(){
    navegador.adicionaObstaculo(new Area(*Sistema::modeloMundo.getCampo()->getAreaEq()));
}

void Agente::addAreaAdvObstaculo(){
    navegador.adicionaObstaculo(new Area(*Sistema::modeloMundo.getCampo()->getAreaAdv()));
}

void Agente::addBolaComoObstaculo(float dist)
{
    float squaredDist = dist*dist;
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();
    navegador.adicionaObstaculo(new Circulo(posBola, squaredDist));
}

unsigned int Agente::getId()
{
    return id;
}

void Agente::executarTatica(){

    if(Sistema::modeloMundo.getRoboEq(static_cast<int>(id))->isPresente()){
        /// verificando se ainda tem Tática na fila para executar
        if(!filaTaticasPen.empty()){
            /// verificando se iremos usar o navegador nessa iteração
            navegadorPronto = iteracoes++ % 10 ? true : false;
            /// pegando a tática que será executada
            Tatica* tatica = filaTaticasPen[posFila];
            Sistema::modeloMundo.getRoboEq(static_cast<int>(id))->setTaticaCorrente(tatica);
            tatica->executar(this);
            addPacoteBUS();
        }else{
            DEBUG("tactics queque empty of agent " + std::to_string(id));
        }
    }
}

bool Agente::atualizarTatica(){

    /// Verificando se a jogada terminou
    if(filaTaticasPen[posFila]->verificarTermino(static_cast<int>(id)))
    {
        /// Senao existir mais taticas e a tatica for a tatica ativa iremos selecionar uma nova jogada, caso contrario iremos continuar executando a tatica
        if(posFila == filaTaticasPen.size()-1){
            if(filaTaticasPen[posFila]->isAtiva())
            {
                return true;
            }
        }else{
            posFila++;
            Sistema::modeloMundo.getRoboEq(static_cast<int>(id))->setTaticaCorrente(filaTaticasPen[posFila]);
        }
    }

    return false;
}

void Agente::setPapel(vector<Tatica *>& _taticasPen){
    filaTaticasPen = _taticasPen;
    posFila=0;
    if(filaTaticasPen.size() == 0)
    {
        DEBUG("no tactics apply");
        return;
    }
    Sistema::modeloMundo.getRoboEq(static_cast<int>(id))->setTaticaCorrente(filaTaticasPen[posFila]);
}

bool Agente::isFilaTaticasVazia()
{
    return filaTaticasPen.empty();
}

void Agente::setPosicao(int pos)
{
    posFila = static_cast<unsigned int>(pos);

}

int Agente::getPosicao()
{

    return static_cast<int>(posFila);
}

Tatica* Agente::getTaticaCorrente()
{
    return filaTaticasPen[posFila];
}

Ponto Agente::calcAlvoFromPathPlanning(const Ponto &destino)
{
    return navegador.RRT(destino);
}

vector<Ponto> Agente::calcAlvosFromPathPlanning(const Ponto &destino)
{
    return navegador.pontosRRT(destino);
}

bool Agente::isPresente()
{
    return Sistema::modeloMundo.getRoboEq(static_cast<int>(id))->isPresente();
}

void Agente::addPacoteBUS()
{

    /// Analisando para aonde iremos enviar os pacotes do robo
    switch(Configuration::SYSTEM_ROBOTS_TYPE){
    case RobotsType::REAL:
        if (isPresente()) {
            repository_->set_package(static_cast<size_t>(id), montador.criaPacoteSerial());
        }
        break;

    case RobotsType::SIMULATED:
        repository_->set_packet(montador.criaPacoteGrSim());
        break;
    }
}

//void Agente::limparPacote(){

//    /// deletando o pacote do agente atual para que não seja enviado
//    mBUS->lock();
//    bus->limparPacoteRobo(id);
//    mBUS->unlock();
//}

void Agente::setNavegador(Navegador _navegador){
    navegador = _navegador;
}

Navegador* Agente::getNavegador(){
    return &navegador;
}

void Agente::operator=(const Agente& a){
    id = a.id;
}

ostream& operator<<(ostream& os, const Agente& a){
    //    os <<"agente "<< a.getId() <<endl;
    return os;
}

