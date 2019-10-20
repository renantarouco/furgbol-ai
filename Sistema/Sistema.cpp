#include "Sistema.h"
#include <Halt.h>

#include "utils/utils.h"

ModeloMundo Sistema::modeloMundo;
Avaliador Sistema::avaliador;

Sistema::Sistema()
{
}

Sistema::~Sistema()
{
    stop();
}

void Sistema::init(QMutex* _mBUS, CommunicationBUS* _barramentoComunicacao)
{
    DEBUG("starting system");
    r_udpSocket.bind(ConfigComunicacao::PORTA_RECEBER, QUdpSocket::ShareAddress);
    numAgentesAtivos = 0;
    mBUS = _mBUS;
    bus = _barramentoComunicacao;
    modeloMundo.init();
    modeloMundo.setRobosTemChute(Config::ROBOS_TEM_CHUTE);
    gerenteCentral.init();
    connect(&r_udpSocket, SIGNAL(readyRead()), this, SLOT(processPacotes()));
    agentes.resize(NUM_MAX_ROBOS);
    cout << "INICIANDO AGENTES" << endl;
    for(int id=0; id<NUM_MAX_ROBOS; id++) {
        agentes[id].init(id, mBUS, bus, serial_repo_);
        cout << "AGENTE INICIADO " << id << endl;
    }
}

void Sistema::stop()
{
    quit();
    wait();
}

void Sistema::tratarPacoteRecebido(){
    if (r_udpSocket.hasPendingDatagrams()) {
        datagram.clear();
        datagram.resize(r_udpSocket.pendingDatagramSize());
        r_udpSocket.readDatagram(datagram.data(), datagram.size());
        pacoteGD.ParseFromArray(datagram.data(), datagram.size());
        modeloMundo.coletaDados(pacoteGD);
    }
}

void Sistema::processPacotes()
{
    tratarPacoteRecebido();
    if(modeloMundo.isMudarSinalPontosJogadas()){
        gerenteCentral.mudarSinalPontosJogadas();
    }
    gerenteCentral.analisarJogada();
    if(gerenteCentral.trocarPapeisOn())
    {
        DEBUG("changing agents assignments");
        papeisRobos = gerenteCentral.atribuirPapeis();
        setPapeisAgentes(papeisRobos); /// Setando a nova ordem de execução dos agentes

        /// Verificando se o estado atual do GDC é o de SELEÇÃO para escolhermos uma nova jogada
    }else if(gerenteCentral.selecaoOn())
    {
        papeisRobos = gerenteCentral.selecionarJogada(); /// Fazendo a seleção da melhor jogada e pegando um vetor dos agentes com seus papeis
        setPapeisAgentes(papeisRobos); /// Setando nos agentes os papeis
    }

    /// executando as táticas de cada agente que estiver presente no campo
    for(int id=0;id<NUM_MAX_ROBOS;id++){

        /// Executando a tática do agente
        agentes[id].executarTatica();
    }


    /** Criando o pacote para enviar para o Monitorardor */
    addPacoteMonitoradorBUS();

    iteracao++;
}

void Sistema::addPacoteMonitoradorBUS(){
    if(ConfigComunicacao::ENVIA_MONITORADOR){
        cout << "enviando pacote monitorador" << endl;

        AIDataManagerPackage pacoteMonitorador;

        /// setando o nome da jogada que está sendo executada
        pacoteMonitorador.set_play(gerenteCentral.getNomeJogadaAtual().c_str());

        //        /// setando os fatos que estão habilitados no modelo de mundo
        //        map<string, bool>* fatos = modeloMundo.getFatos();
        //        for (map<string,bool>::iterator it= fatos->begin() ; it != fatos->end(); it++ )
        //        {
        //            if((*it).second==true) pacoteMonitorador.add_fatosdomundo((*it).first);
        //        }

        /// adicionando os robos da equipe e os adversários no pacote
        vector<Robo>* robosEq = modeloMundo.getRobosEq();
        vector<Robo>* robosAdv = modeloMundo.getRobosAdv();
        for(int id = 0; id < NUM_MAX_ROBOS; id++){
            Robo* robo = &robosEq->at(id);
            if(robo->isPresente()){

                /// setando os dados do robo
                AIRobotPackage* pacoteRobo = pacoteMonitorador.add_team();
                pacoteRobo->set_id(robo->getId());
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

                /// setando se o drible está ligado ou não
                pacoteRobo->set_dribbler(robo->isTemDrible());

                /// copiando os alvos do path planning
                pacoteRobo->mutable_path_planning()->CopyFrom(robo->getPacotePathPlanning());
            }
        }

        //        printf("%s\n", pacoteMonitorador.DebugString().c_str());

        /// setando pacote no barramento
        mBUS->lock();
        bus->carregaPacoteMonitorador(pacoteMonitorador);
        mBUS->unlock();

        cout << "pacote enviado com sucesso" << endl;
    }
}

void Sistema::run()
{
    cout << "STARTING SISTEMA" << endl;
    iteracao = 1;
    exec();
}

void Sistema::setPapeisAgentes(map<int, vector<Tatica*> > & papeis)
{
    for(int id = 0; id < NUM_MAX_ROBOS; id++){
        if(agentes[id].isPresente()){
            agentes[id].setPapel(papeis[id]);
        }else{
            agentes[id].limparFilaTaticas();
        }
    }
}
