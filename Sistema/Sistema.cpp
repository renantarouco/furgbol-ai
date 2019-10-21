#include "Sistema.h"
#include <Halt.h>

#include "configuration/configuration.h"
#include "utils/utils.h"

ModeloMundo Sistema::modeloMundo;
Avaliador Sistema::avaliador;

Sistema::Sistema() : reader_(Configuration::PERCEPTION_IP, Configuration::PERCEPTION_RECEIVE_PORT)
{
}

void Sistema::init(std::shared_ptr<RepositoryProxy> repository)
{
    DEBUG("initializing legacy system");
    repository_ = repository;
    numAgentesAtivos = 0;
    modeloMundo.init();
    modeloMundo.setRobosTemChute(Config::ROBOS_TEM_CHUTE);
    gerenteCentral.init();
    agentes.resize(Configuration::SYSTEM_MAX_ROBOTS);
    for (size_t id=0; id < Configuration::SYSTEM_MAX_ROBOTS; id++) {
        agentes[id].init(static_cast<int>(id), repository);
    }
}


void Sistema::tratarPacoteRecebido(){
    pacoteGD.Clear();
    std::string datagram = reader_.receive();
    pacoteGD.ParseFromString(datagram);
    modeloMundo.coletaDados(pacoteGD);
}

void Sistema::processPacotes()
{
    DEBUG("processing package");
    tratarPacoteRecebido();
    if(modeloMundo.isMudarSinalPontosJogadas()){
        gerenteCentral.mudarSinalPontosJogadas();
    }
    gerenteCentral.analisarJogada();
    if(gerenteCentral.trocarPapeisOn())
    {
        papeisRobos = gerenteCentral.atribuirPapeis();
        setPapeisAgentes(papeisRobos); /// Setando a nova ordem de execução dos agentes
        /// Verificando se o estado atual do GDC ? o de SELE??O para escolhermos uma nova jogada
    }else if(gerenteCentral.selecaoOn())
    {
        papeisRobos = gerenteCentral.selecionarJogada();
        setPapeisAgentes(papeisRobos);
    }
    for(size_t id=0;id<Configuration::SYSTEM_MAX_ROBOTS;id++){
        agentes[id].executarTatica();
    }
    /** Criando o pacote para enviar para o Monitorardor */
    addPacoteMonitoradorBUS();
    iteracao++;
}

void Sistema::addPacoteMonitoradorBUS(){
    if(ConfigComunicacao::ENVIA_MONITORADOR){
        DEBUG("adding monitor package");
        AIDataManagerPackage pacoteMonitorador;
        /// setando o nome da jogada que está sendo executada
        pacoteMonitorador.set_play(gerenteCentral.getNomeJogadaAtual().c_str());
        /// adicionando os robos da equipe e os adversários no pacote
        vector<Robo>* robosEq = modeloMundo.getRobosEq();
        for(size_t id = 0; id < Configuration::SYSTEM_MAX_ROBOTS; id++){
            Robo* robo = &robosEq->at(id);
            if(robo->isPresente()){
                /// setando os dados do robo
                AIRobotPackage* pacoteRobo = pacoteMonitorador.add_team();
                pacoteRobo->set_id(static_cast<google::protobuf::uint32>(robo->getId()));
                pacoteRobo->set_x(robo->getPosicao().x());
                pacoteRobo->set_y(robo->getPosicao().y());
                pacoteRobo->set_orientation(robo->getOrientacao());
                pacoteRobo->set_skill(robo->getSkill());
                pacoteRobo->set_tactic(robo->getTaticaCorrente()->getNome());
                pacoteRobo->set_target_x(robo->getComando().getAlvoFinal().x());
                pacoteRobo->set_target_y(robo->getComando().getAlvoFinal().y());
                pacoteRobo->set_velocity_x(robo->getVelocidade().x()); /// conferir esse vetor enviado
                pacoteRobo->set_velocity_y(robo->getVelocidade().y());
                pacoteRobo->set_velocity_theta(robo->getVelAngular());
                /// setando o chute do robo
                Comando::TipoChute tipoChute = robo->getComando().getTipoChute();
                if(tipoChute == Comando::SEM_CHUTE){
                    pacoteRobo->set_low_kick(0.0);
                    pacoteRobo->set_high_kick(0.0);
                }else if(tipoChute == Comando::CHUTE_BAIXO){
                    pacoteRobo->set_low_kick(1.0);
                    pacoteRobo->set_high_kick(0.0);
                }else if(tipoChute == Comando::CHUTE_ALTO){
                    pacoteRobo->set_low_kick(0.0);
                    pacoteRobo->set_high_kick(1.0);
                }
                pacoteRobo->set_dribbler(robo->isTemDrible());
                pacoteRobo->mutable_path_planning()->CopyFrom(robo->getPacotePathPlanning());
            }
        }
        repository_->set_packet(pacoteMonitorador);
        DEBUG("successfully added package");
    }
}

void Sistema::run()
{
    DEBUG("running legacy system");
    iteracao = 1;
    while (1) {
        processPacotes();
    }
}

void Sistema::setPapeisAgentes(map<int, vector<Tatica*> > & papeis)
{
    for(size_t id = 0; id < Configuration::SYSTEM_MAX_ROBOTS; id++){
        if(agentes[id].isPresente()){
            agentes[id].setPapel(papeis[id]);
        }else{
            agentes[id].limparFilaTaticas();
        }
    }
}
