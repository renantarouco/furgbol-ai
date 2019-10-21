#include "ModeloMundo.h"

#include <Sistema.h>

#include "configuration/configuration.h"
#include "utils/utils.h"

ModeloMundo::ModeloMundo(){

}

ModeloMundo::~ModeloMundo(){
}

void ModeloMundo::init(){
    cout << "INICIANDO MODELO DE MUNDO" << endl;

    /// criando o vetor dos robos da equipe e dos adversários
    robosEq.resize(Configuration::SYSTEM_MAX_ROBOTS);
    robosAdv.resize(Configuration::SYSTEM_MAX_ROBOTS);
    for(size_t id=0; id < Configuration::SYSTEM_MAX_ROBOTS; id++) {
        robosEq[id].init(static_cast<int>(id), true);
        robosAdv[id].init(static_cast<int>(id), true);
    }
    idGoleiroEq = 1;

    /// setando o tempo inicial da iteração com a hora atual
    relogio.setTempoInicial(relogio.getTempoDoDia());
    tempoIteracao = 0.0;

    // LADO ESQUERDO É O DEFAULT
    corEquipe = ControlPackage::Yellow;
    ladoCampo = ControlPackage::Left;
    tempoPartida=0;
    numeroUltimoFrameRecebido = -1;
    estadoAtual="halt";
    estadoAnt="";
    posBolaNormalStart = CGAL::ORIGIN;

    /// construindo o objeto que contém a descrição do campo
    campo.construir();

    /// Preenchendo o map de fatos do modelo de mundo que serao usados para escolher a melhor jogada em cada situacao de jogo
    preencheFatos();
}

void ModeloMundo::setIdGoleiro(int id){
    idGoleiroEq = id;
}

int ModeloMundo::getIdGoleiro(){
    return idGoleiroEq;
}

vector<int> ModeloMundo::getIdRobosComChute(){
    vector<int> idRobosComChute;

    BOOST_FOREACH(Robo& robo, robosEq){
        if(robo.isPresente() && robo.isTemChute() && robo.getId() != idGoleiroEq){
            idRobosComChute.push_back(robo.getId());
        }
    }

    return idRobosComChute;
}
void ModeloMundo::setRobosTemChute(map<int, bool>& robosTemChute)
{
    /// setando os robos que possuem chute
    BOOST_FOREACH(Robo& robo, robosEq){
        robo.setTemChute(robosTemChute[robo.getId()]);
    }
}

bool ModeloMundo::isLadoCampoEsquerdo(){
    return ladoCampo == ControlPackage::Left ? true : false;
}

void ModeloMundo::preencheFatos()
{
    fatos["posse_bola"] = false;
    fatos["bola_area_eq"] = false;
    fatos["bola_area_adv"] = false;
    fatos["bola_campo_eq"] = false;
    fatos["bola_campo_adv"] = false;
    fatos["bola_cercada"] = false;
    fatos["tem_robo_marcar"] = false;
    fatos["jogo_normal"] = true;
    fatos["stop"] =false;
    fatos["penalty_shootout"] = false;
    fatos["halt"] = false;
    fatos["kickoff_eq"] = false;
    fatos["kickoff_adv"] = false;
    fatos["direct_kick_eq"] = false;
    fatos["direct_kick_adv"] = false;
    fatos["indirect_kick_eq"] = false;
    fatos["indirect_kick_adv"] = false;
    fatos["penalty_eq"] = false;
    fatos["penalty_adv"] = false;
    fatos["ready"] = false;
    fatos["bola_perto_area_eq"] = false;
    fatos["bola_perto_linha_fundo_eq"] = false;
    fatos["teste"] = false;
}

bool ModeloMundo::verificarRoboMaisProxBola(){

    /// Variaveis que serao usadas para identificar o robo mais proximo da bola
    float menorDistEq = numeric_limits<float>::infinity();
    float menorDistAdv = numeric_limits<float>::infinity();
    Ponto posBola = bola.getPosicao();

    for(size_t id=0; id < Configuration::SYSTEM_MAX_ROBOTS; id++){

        /// Verificando se o robo esta presente na partida
        Robo* roboEq = &robosEq[id];
        if(roboEq->isPresente()){
            float distBola = squared_distance(posBola, roboEq->getPosicao());

            /// Verificando se a distancia e menor que a menor distancia encontrada
            if(distBola < menorDistEq){
                menorDistEq = distBola;
                roboEqMaisProxBola = roboEq;
            }
        }

        /// Verificando se o robo esta presente na partida
        Robo* roboAdv = &robosAdv[id];
        if(roboAdv->isPresente()){
            float distBola = squared_distance(posBola, roboAdv->getPosicao());

            /// Verificando se a distancia e menor que a menor distancia encontrada
            if(distBola < menorDistAdv){
                menorDistAdv = distBola;
                roboAdvMaisProxBola = roboAdv;
            }
        }
    }

    /// retornando se o nosso robo está mais proximo do que o outro
    return menorDistEq < menorDistAdv ? true : false;
}

void ModeloMundo::atualizarFatos(){

    /// variáveis auxiliares
    Ponto posBola = bola.getPosicao();

    /// verificando se a bola está perto da area da equipe
    fatos["bola_perto_area_eq"] = bola.isPertoAreaEq();

    /// verificando se a bola está perto da linha de fundo
    fatos["bola_perto_linha_fundo_eq"] = bola.isPertoLinhaFundoEq();

    /// Verificando se o adversario possue mais de 2 robos para que possamos marcar um robo, caso contrario iremos posicionar o robo na defesa
    fatos["tem_robo_marcar"] = qtRobosAdvPresentes > 2;

    /// funcao que analisa o id dos dois robos mais proximos da bola e retorna se a posse de bola é nossa ou não
    fatos["posse_bola"] = verificarRoboMaisProxBola();

    /// verificando se a bola está no campo da equipe
    fatos["bola_campo_eq"] = Sistema::avaliador.isPontoLadoEq(posBola);
    if(fatos["bola_campo_eq"]){

        /// verificando se a bola está dentro da area
        fatos["bola_area_eq"] = campo.getAreaEq()->isPontoDentro(posBola);

        /// setando os outros como false
        fatos["bola_area_adv"] = false;
        fatos["bola_campo_adv"] = false;
    }else{

        /// verificando se a bola está no campo adv
        fatos["bola_campo_adv"] = Sistema::avaliador.isPontoLadoAdv(posBola);
        if(fatos["bola_campo_adv"]){
            fatos["bola_area_adv"] = campo.getAreaAdv()->isPontoDentro(posBola);

            fatos["bola_area_eq"] = false;
        }
    }

    /// verificando se tem muitos robos adv perto da bola e perto do nosso gol
    if(fatos["posse_bola"] && fatos["bola_campo_eq"] && Sistema::avaliador.getNumAdvPertoPonto(posBola, 600.0) > 1)
        fatos["bola_cercada"] = true;
}

void ModeloMundo::resetaFatos(){
    map<string,bool>::iterator it;

    for(it = fatos.begin(); it != fatos.end(); it++)
        (*it).second = false;
}

ControlPackage::TeamColor ModeloMundo::getCorEquipe()
{
    return corEquipe;
}

Campo *ModeloMundo::getCampo()
{
    return &campo;
}

vector<Robo*> ModeloMundo::getOutrosRobosComTatica(int _id, string _nomeTatica){

    /// Robôs presentes com a mesma tática
    vector<Robo*> robosComMesmaTatica;

    /// Pegando o id dos robos menos o do goleiro
    for(size_t idRobo = 0; idRobo < Configuration::SYSTEM_MAX_ROBOTS; idRobo++){
        Robo* roboEq = &robosEq[idRobo];

        /// Verificando se o robô não tem o mesmo id que o passado como parâmetro e se possue a mesma tática corrente
        if((roboEq->isPresente()) && (idRobo != idGoleiroEq) && (idRobo != _id) && (roboEq->getNomeTaticaCorrente() == _nomeTatica)){
            robosComMesmaTatica.push_back(roboEq);
        }
    }

    return robosComMesmaTatica;
}

int ModeloMundo::getQtRobosEqPresentes()
{
    return qtRobosEqPresentes;
}

int ModeloMundo::getQtRobosAdvPresentes()
{
    return qtRobosAdvPresentes;
}

bool ModeloMundo::isEqAmarelo()
{
    return corEquipe == ControlPackage::Yellow;
}

vector<int>* ModeloMundo::getIdRobosMenosGoleiro()
{
    return &idRobosEqMenosGoleiro;
}

void ModeloMundo::coletaDados(const DataPackage& pacoteEntrada)
{
    /// lendo os dados das posições dos robos
    if(pacoteEntrada.has_control()){
        coletaDadosControle(pacoteEntrada.control());
    }

    /// lendo os dados do estado do jogo caso tivermos que usar o juiz
    if(pacoteEntrada.has_referee() && ConfigComunicacao::USAR_REFEREE){
        coletaDadosEstados(pacoteEntrada.referee());
    }

    /// lendo os dados das posições dos robos
    if(pacoteEntrada.has_vision()){
        coletaDadosVisao(pacoteEntrada.vision());
    }
    atualizarFatos();
}

void ModeloMundo::coletaDados(const AI2DSimulatorPackage& pacoteEntrada)
{
    tempoIteracao = relogio.getTempoPassado(); /// pegando o tempo entre dois recebimentos de pacotes, esse é o delay da IA + VISÃO
    relogio.setTempoInicial(relogio.getTempoDoDia());

    /// Se o valor antigo do lado do campo for diferente do atual, então iremos mudar o lado do campo e alterar o valor dos pontos nas jogadas.
    if(ladoCampo != pacoteEntrada.field_side()){

        /// função que troca o lado do nosso gol
        campo.trocarLado();

        /// Setar valor da flag como true para poder mudar o sinal.
        mudarSinal = true;

        /// setando o novo lado do campo
        ladoCampo = (ControlPackage::FieldSide) pacoteEntrada.field_side();
    }

    /// setando o id dos goleiros das equipes
    idGoleiroEq = 0;
    idGoleiroAdv = 0;

    /// setando a cor da equipe
    corEquipe = (ControlPackage::TeamColor) pacoteEntrada.team_color();

    /// Deixa todos os robos como ausentes e so poem como presentes os que chegarem neste pacote
    idRobosEqMenosGoleiro.clear();
    for (size_t id = 0; id < Configuration::SYSTEM_MAX_ROBOTS; id++) {
        robosEq[id].dontPresent();
        robosAdv[id].dontPresent();
    }

    /// setando os dados dos robos da equipe
    qtRobosEqPresentes = pacoteEntrada.team_size();
    for(int i = 0; i < qtRobosEqPresentes; i++){

        /// pegando o pacote do robo que foi detectado pela visão e setando a posicao e orientação
        AIRobotPackage pacoteRobo = pacoteEntrada.team(i);
        int idRobo = pacoteRobo.id();
        robosEq[idRobo].setDados(Ponto(pacoteRobo.x(), pacoteRobo.y()), pacoteRobo.orientation());

        /// inserindo no vetor os ids dos robos presentes menos o id do goleiro
        if(idRobo != idGoleiroEq)
            idRobosEqMenosGoleiro.push_back(idRobo);
    }

    /// setando os dados dos robos adversarios
    qtRobosAdvPresentes = pacoteEntrada.enemy_size();
    for(int i = 0; i < qtRobosAdvPresentes; i++){

        /// pegando o pacote do robo adv e setando os dados da posicao e orientação
        RobotPackage pacoteRobo = pacoteEntrada.enemy(i);
        int roboId = pacoteRobo.id();
        robosAdv[roboId].setDados(Ponto(pacoteRobo.x(),pacoteRobo.y()), pacoteRobo.orientation());
    }

    delayGerenteDados = 0.0;
    numeroUltimoFrameRecebido = 1;

    /// coletando dados da bola
    coletaDadosBola(pacoteEntrada.ball());

    /// setando os dados do referee
    coletaDadosEstados(pacoteEntrada.referee());

    /// atualizando os fatos do mundo toda vez que chega um pacote
    atualizarFatos();
}


void ModeloMundo::coletaDadosVisao(const VisionPackage& pacoteVisao)
{
    int numFramePacote = pacoteVisao.frame_number();
    if(numFramePacote > numeroUltimoFrameRecebido){

        if ( numFramePacote > numeroUltimoFrameRecebido+1){
            cout << "Perdeu " << numFramePacote - numeroUltimoFrameRecebido -1 << " frames " << endl;
        }
        numeroUltimoFrameRecebido = pacoteVisao.frame_number();
        /// setando que os robos não estão presentes inicialmente
        idRobosEqMenosGoleiro.clear();
        for(size_t id = 0; id < Configuration::SYSTEM_MAX_ROBOTS; id++){
            robosEq[id].dontPresent();
            robosAdv[id].dontPresent();
        }
        /// analisando os pacotes dos robos que foram encontrados no campo
        qtRobosEqPresentes = pacoteVisao.team_size();
        for(int i = 0; i < qtRobosEqPresentes; i++){
            /// pegando o pacote do robo da equipe
            RobotPackage pacoteRobo = pacoteVisao.team(i);
            int idRobo = pacoteRobo.id();
            robosEq[idRobo].setDados(Ponto(pacoteRobo.x(), pacoteRobo.y()), pacoteRobo.orientation());
            /// inserindo no vetor os ids dos robos presentes menos o id do goleiro
            if(idRobo != idGoleiroEq)
                idRobosEqMenosGoleiro.push_back(idRobo);
        }
        /// analisando os pacotes dos robos adv que foram encontrados no campo
        qtRobosAdvPresentes = pacoteVisao.enemy_size();
        for(int i = 0; i < qtRobosAdvPresentes; i++){

            /// pegando o pacote do robo adv e setando a posicao e orientação
            RobotPackage pacoteRobo = pacoteVisao.enemy(i);
            int idRobo = pacoteRobo.id();
            robosAdv[idRobo].setDados(Ponto(pacoteRobo.x(),pacoteRobo.y()), pacoteRobo.orientation());
        }
        delayGerenteDados = pacoteVisao.delay();
        numeroUltimoFrameRecebido = pacoteVisao.frame_number();
        coletaDadosBola(pacoteVisao.ball());
    }
}


void ModeloMundo::coletaDadosEstados(const SSL_Referee &pacoteEstados){

    //    cout << " INICIO LEITURA DADOS ESTADOS" << endl;

    /// setando o id do goleiro da equipe e do adversário
    if(corEquipe == ControlPackage::Yellow){
        idGoleiroEq = pacoteEstados.yellow().goalie();
        idGoleiroAdv = pacoteEstados.blue().goalie();
    }else{
        idGoleiroEq = pacoteEstados.blue().goalie();
        idGoleiroAdv = pacoteEstados.yellow().goalie();
    }

    /// Verificando qual o estado atual da partida
    switch(pacoteEstados.command()){
    case SSL_Referee::FORCE_START:
        estadoAtual = "jogo_normal";
        break;

    case SSL_Referee::STOP:
        estadoAtual = "stop";
        break;

    case SSL_Referee::PREPARE_KICKOFF_BLUE:

        if(corEquipe == ControlPackage::Blue){ /// verificando qual time nos somos
            estadoAtual = "kickoff_eq";
        }else{

            /// se o estado anterior for jogo_normal é por que o normal start já foi executado, caso contrário o normal start não foi executado
            if(estadoAtual != "jogo_normal"){
                estadoAtual = "kickoff_adv";
            }
        }
        break;

    case SSL_Referee::PREPARE_KICKOFF_YELLOW:

        if(corEquipe == ControlPackage::Yellow){ /// verificando qual time nos somos
            estadoAtual = "kickoff_eq";
        }else{

            /// se o estado anterior for jogo_normal é por que o normal start já foi executado, caso contrário o normal start não foi executado
            if(estadoAtual != "jogo_normal"){
                estadoAtual = "kickoff_adv";
            }
        }
        break;

    case SSL_Referee::PREPARE_PENALTY_BLUE:

        if(corEquipe == ControlPackage::Blue){ /// verificando qual time nos somos
            ConfigMontador::DESACELERACAO = 0.6;
            ConfigMontador::ACELERACAO = 0.3;
            estadoAtual = "penalty_eq";
        }else{
            estadoAtual = "penalty_adv";
        }
        break;

    case SSL_Referee::PREPARE_PENALTY_YELLOW:

        if(corEquipe == ControlPackage::Yellow){ /// verificando qual time nos somos
            ConfigMontador::DESACELERACAO = 0.6;
            ConfigMontador::ACELERACAO = 0.3;
            estadoAtual = "penalty_eq";
        }else{
            estadoAtual = "penalty_adv";
        }
        break;

    case SSL_Referee::DIRECT_FREE_BLUE:
        if(corEquipe == ControlPackage::Blue){ /// verificando qual time nos somos
            estadoAtual = "direct_kick_eq";
        }else{
            /// se o estado anterior for jogo_normal é por que o normal start já foi executado, caso contrário o normal start não foi executado
            if(estadoAtual != "jogo_normal"){
                estadoAtual = "direct_kick_adv";
            }
        }

        break;

    case SSL_Referee::DIRECT_FREE_YELLOW:
        if(corEquipe == ControlPackage::Yellow){ /// verificando qual time nos somos
            estadoAtual = "direct_kick_eq";
        }else{

            /// se o estado anterior for jogo_normal é por que o normal start já foi executado, caso contrário o normal start não foi executado
            if(estadoAtual != "jogo_normal"){
                estadoAtual = "direct_kick_adv";
            }
        }

        break;

    case SSL_Referee::INDIRECT_FREE_BLUE:

        if(corEquipe == ControlPackage::Blue){ /// verificando qual time nos somos
            estadoAtual = "indirect_kick_eq";
        }else{

            /// se o estado anterior for jogo_normal é por que o normal start já foi executado, caso contrário o normal start não foi executado
            if(estadoAtual != "jogo_normal"){
                estadoAtual = "indirect_kick_adv";
            }
        }

        break;

    case SSL_Referee::INDIRECT_FREE_YELLOW:

        if(corEquipe == ControlPackage::Yellow){ /// verificando qual time nos somos
            estadoAtual = "indirect_kick_eq";
        }else{

            /// se o estado anterior for jogo_normal é por que o normal start já foi executado, caso contrário o normal start não foi executado
            if(estadoAtual != "jogo_normal"){
                estadoAtual = "indirect_kick_adv";
            }
        }

        break;

    case SSL_Referee::NORMAL_START:

        /// se o estado anterior for jogo_normal é por que o normal start já foi executado, caso contrário o normal start não foi executado
        if(estadoAtual != "jogo_normal"){
            estadoAtual = "ready";
        }

        break; /// dando return para que não seja setado que o estado anterior não é o normal start

    default:
        estadoAtual="halt";
        ConfigMontador::DESACELERACAO = 1.1;
        ConfigMontador::ACELERACAO = 4;
        break;
    }

    /// verificando se a bola se moveu
    if(estadoAtual=="ready" || estadoAtual =="indirect_kick_adv" || estadoAtual =="direct_kick_adv"){

        /// Verificando se o estado anterior é o normal start se for iremos verificar se a bola andou mais do que o limite
        if(posBolaNormalStart != CGAL::ORIGIN){
            double dist = squared_distance(bola.getPosicao(), posBolaNormalStart);

            if(dist > 50.0*50.0){ /// verificando se a bola andou mais do que 5 cm
                fatos["jogo_normal"] = true;
                estadoAtual = "jogo_normal";
                posBolaNormalStart = CGAL::ORIGIN;
            }

            /// caso não seja o normal start iremos pegar a posicao da bola no momento, para posteriormente verificar se ela andou
            /// o bastante para classificar o estado como jogo_normal
        }else{
            posBolaNormalStart = bola.getPosicao();
        }
    }

    /// Setando todos os fatos como false a não ser o estado atual
    map<string, bool>::iterator it;
    for(it = fatos.begin(); it != fatos.end(); it++){
        it->second = false;
    }
    fatos[estadoAtual] = true;

    /// caso o estado atual seja o ready não podemos descartar o estado anterior, pois servirá para definir a jogada a ser executada
    if(estadoAtual == "ready" && estadoAnt != ""){
        fatos[estadoAnt] = true;
    }else{
        if(estadoAtual == "ready" && estadoAnt=="")
            estadoAtual = "jogo_normal", fatos["jogo_normal"]=true, fatos["ready"]=false;

        estadoAnt = estadoAtual; /// setando o estado anterior como atual
    }

    //    cout << " FIM LEITURA DADOS ESTADOS" << endl;
}

void ModeloMundo::coletaDadosControle(const ControlPackage& pacoteControle)
{
    //    cout << " INICIO LEITURA DADOS CONTROLE" << endl;

    /// Se o valor antigo do lado do campo for diferente do atual, então iremos mudar o lado do campo e alterar o valor dos pontos nas jogadas.
    if(pacoteControle.field_side() != ladoCampo){

        /// trocando o lado da nossa equipe no campo
        campo.trocarLado();

        /// Setar valor da flag como true para poder mudar o sinal.
        mudarSinal = true;

        /// setando o novo lado do campo
        ladoCampo = pacoteControle.field_side();
    }

    /// setando a cor da equipe
    corEquipe = pacoteControle.team_color();

    //    /// setando o id do goleiro
//    idGoleiroEq = pacoteControle.idgoleiro();

    //    cout << " FIM LEITURA DADOS CONTROLE" << endl;
}

void ModeloMundo::coletaDadosBola(const BallPackage &pacoteBola)
{
    /// setando a posicao e o tempo da iteracao para pordemos calcular a velocidade atual da bola entre os dois frames
    //    bola.setPosicao(Ponto(pacoteBola.x(), pacoteBola.y()), tempoIteracao);
    bola.setPosicao(Ponto(pacoteBola.x(), pacoteBola.y()));

    /// se a altura da bola
    bola.setAltura(pacoteBola.z());
}

int ModeloMundo::getIdGoleiroAdv()
{
    return idGoleiroAdv;
}

map<string,bool>* ModeloMundo::getFatos()
{
    return &fatos;
}

Robo* ModeloMundo::getRoboEq(int _id)
{
    if(_id >= Configuration::SYSTEM_MAX_ROBOTS || _id < 0 ){
        cout <<  " ERRO id invalido na get ROBO " << endl;
        return NULL;
    }

    // cout << " Pegou Robo " << _id << endl;
    return &robosEq[_id];
}

Robo* ModeloMundo::getRoboAdv(int _id)
{
    if(_id > Configuration::SYSTEM_MAX_ROBOTS || _id < 0 ){
        cout <<  " ERRO id invalido na get ROBO " << endl;
        return NULL;
    }

    return &robosAdv[_id];
}

Bola* ModeloMundo::getBola()
{
    return &bola;
}
ControlPackage::FieldSide ModeloMundo::getLadoCampo()
{
    return ladoCampo;
}

vector<Robo>* ModeloMundo::getRobosEq()
{
    return &robosEq;
}

vector<Robo>* ModeloMundo::getRobosAdv()
{
    return &robosAdv;
}

void ModeloMundo::setMudarSinalPontosJogadas(bool _mudarSinal){
    this->mudarSinal = _mudarSinal;
}

bool ModeloMundo::isMudarSinalPontosJogadas(){
    return mudarSinal;
}

int ModeloMundo::getIdRoboAtivo(){

    /// analisando qual dos robos da equipe está com a tatica ativa
    BOOST_FOREACH(Robo& robo, robosEq){
        if(robo.isPresente() && robo.getTaticaCorrente()->isAtiva()){
            return robo.getId();
        }
    }

    cout << " Sem robos ativos no momento " << endl;
    return -1;
}

int ModeloMundo::getIdRoboNaoPodeTocarBola()
{
    return -1;
}

void ModeloMundo::PontoDentroAreaAdv(Ponto& alvo)
{
    Area *areaAdv = this->getCampo()->getAreaAdv();
    Ponto pbola = bola.getPosicao();
    if(areaAdv->isPontoDentro(pbola)){
        Segmento seg= areaAdv->getSegmentoArea();
        Ponto pbola = bola.getPosicao();
        //Ponto depoisBola= Ponto(pbola.x());
        Robo goleiroadv = robosAdv[0];
        Ponto pgoleiroadv= goleiroadv.getPosicao();
        Reta reta= Reta(pgoleiroadv,pbola);
        Objeto inter= CGAL::intersection(reta,seg);
        //alvo= Ponto(0,0);
        //cout<<"-------------------------------------------------------"<<inter.<<"------------------------------------------------------------------"<<endl;
        /*for(i=0;seg.point(i)!=seg.end();i++){
            cout<<"BOLA X:"<<(int)pbola.x()<<endl;
            cout<<"PONTO X:"<<(int)seg.point(i).x()<<endl;

        }*/

    }
}

bool ModeloMundo::isBolaDentroAreaAdv()
{
    Area areaAdv = *campo.getAreaAdv();
    return areaAdv.isPontoDentro(bola.getPosicao());
}
