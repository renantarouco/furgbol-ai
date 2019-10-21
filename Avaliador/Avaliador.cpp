#include "Avaliador.h"
#include <Sistema.h>
#include <ConfigAvaliador.h>

Avaliador::Avaliador(){
    cout << "INICIANDO AVALIADOR" << endl;
    posseBola = true;
}

Reta Avaliador::getRetaCirculo(){
    return retaCirculo;
}

void Avaliador::setRetaCirculo(const Reta &_retaCirculo){
    retaCirculo = _retaCirculo;
}

bool Avaliador::verificarPosseBola(){
    double tempoComBola;

    /// Verificando se a bola alterou de posse
    if(isBolaNossa() != bolaNossaAnt){
        tempoDaPosse = relogio.getTempoDoDia();

    }else{
        tempoComBola = relogio.getTempoDoDia() - tempoDaPosse;

        if(tempoComBola > ConfigAvaliador::TEMPO_ANALISAR_POSSE_BOLA)
            posseBola = bolaNossaAnt;
    }

    /// Guarda se a posse da bola era ou nao era nossa na iteracao anterior
    bolaNossaAnt = isBolaNossa();

    return posseBola;
}

float Avaliador::calcularOrientacaoAlvoBola(const Ponto& alvo){
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    /// calculando o angulo entre o alvo e a bola
    return atan2((posBola.y()-alvo.y()),(posBola.x()-alvo.x()));
}

float Avaliador::calcularOrientacaoPosicaoParaAlvo(const Ponto &posicao, const Ponto &alvo)
{
    /// calculando o angulo entre o alvo e a bola
    return atan2((alvo.y()-posicao.y()),(alvo.x()-posicao.x()));
}

int Avaliador::getIdRoboNaoPodeTocarBola(){

    /// variáveis auxiliares
    vector<Robo>* robosEq = Sistema::modeloMundo.getRobosEq();

    /// Pegando o id do robo que nao pode tocar na bola
    for(int id=0; id < Configuration::SYSTEM_MAX_ROBOTS;id++){
        Robo* roboEq = &robosEq->at(id);

        /// verificando se o robo está presente e se ele não pode tocar na bola
        if(roboEq->isPresente() && !roboEq->isTocarBola()){
            return roboEq->getId();
        }
    }

    return -1;
}

IdDistancia Avaliador::getIdDistRoboEqMaisPertoBola(){

    /// Variaveis que serao usadas para identificar o robo mais proximo da bola
    int idRoboMaisPerto = -1;
    float menor = numeric_limits<float>::infinity();
    vector<Robo>* robosEq = Sistema::modeloMundo.getRobosEq();
    int qtRobosEq = robosEq->size();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    for(int id=0;id<qtRobosEq;id++){
        Robo* roboEq = &robosEq->at(id);

        /// Verificando se o robo esta presente na partida
        if(roboEq->isPresente()){
            float distBola = squared_distance(posBola, roboEq->getPosicao());

            /// Verificando se a distancia e menor que a menor distancia encontrada
            if(distBola < menor){
                menor = distBola;
                idRoboMaisPerto = id;
            }
        }
    }

    /// Verificando se foi possivel encontrar o robo mais proximo da bola
    if(idRoboMaisPerto == -1){
        cout << " WARNING: Nao tem um robo eq mais proximo da bola" << endl;
    }

    return IdDistancia(idRoboMaisPerto, menor);
}

Ponto Avaliador::getPosAdvMaisPertoBola(){

    /// Variaveis que serao usadas para identificar o robo mais proximo da bola
    float menor = numeric_limits<float>::infinity();
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    int qtRobosAdv = robosAdv->size();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    /// posicao do robo mais proximo da bola
    Ponto posMaisProx;

    for(int id=0;id<qtRobosAdv;id++){
        Robo* robo = &robosAdv->at(id);

        /// Verificando se o robo esta presente na partida
        if(robo->isPresente()){
            float distBola = squared_distance(posBola, robo->getPosicao());

            /// Verificando se a distancia e menor que a menor distancia encontrada
            if(distBola < menor){
                menor = distBola;
                posMaisProx = robo->getPosicao();
            }
        }
    }

    return posMaisProx;
}

IdDistancia Avaliador::getIdDistAdvMaisPertoBola(){

    /// Variaveis que serao usadas para identificar o robo mais proximo da bola
    int idRoboMaisPerto = -1;
    float menor = numeric_limits<float>::infinity();
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    BOOST_FOREACH(Robo& robo, *robosAdv){

        /// Verificando se o robo esta presente na partida
        if(robo.isPresente()){
            float distBola = squared_distance(posBola, robo.getPosicao());

            /// Verificando se a distancia e menor que a menor distancia encontrada
            if(distBola < menor){
                menor = distBola;
                idRoboMaisPerto = robo.getId();
            }
        }
    }

    /// Verificando se foi possivel encontrar o robo mais proximo da bola
    if(idRoboMaisPerto == -1){
        cout << " WARNING: Nao tem um robo adv mais proximo da bola" << endl;
    }

    return IdDistancia(idRoboMaisPerto, menor);
}

int Avaliador::getNumAdvPertoPonto(const Ponto &ponto, float distancia){

    /// Variaveis que serao usadas para identificar o robo mais proximo da bola
    int cont = 0;
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    distancia = distancia*distancia; /// deixando ao quadrado pra diminuir o numero de calculos

    BOOST_FOREACH(Robo& roboAdv, *robosAdv){

        /// Verificando se o robo esta presente na partida
        if(roboAdv.isPresente()){

            /// Verificando se o robo esta proximo do ponto de acordo com a distancia que foi passada como parametro
            if(squared_distance(roboAdv.getPosicao(),ponto) < distancia){
                cont++;
            }
        }
    }

    return cont;
}

int Avaliador::getIdAdvMaisProxGolSemBola(bool roboForaAlcanceBola){

    /// variáveis auxilires
    Campo* campo = Sistema::modeloMundo.getCampo();
    Area* areaEq = campo->getAreaEq();
    Ponto centroGolEq = areaEq->getCentro();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    int idRoboGoleiroAdv = Sistema::modeloMundo.getIdGoleiroAdv();

    /// Pegando o id do robo adv mais perto da bola
    int idRoboAdvMaisPertoBola = getIdDistAdvMaisPertoBola().first;
    float distRobosJogoParado = ConfigAvaliador::DIST_ROBOS_BOLA_JOGO_PARADO*ConfigAvaliador::DIST_ROBOS_BOLA_JOGO_PARADO;

    /// Analisando todos os robos adversarios
    int idAdvMaisProxGolSemBola = -1; /// Id do robô mais próximo do nosso gol sem bola
    float menorDist = numeric_limits<float>::infinity(); /// Menor distância encontrada entre o robô e a nossa área.
    BOOST_FOREACH(Robo& robo, *robosAdv){

        /// Verificando se o robo esta presente no campo
        if(robo.isPresente()){

            /// variáveis auxiliares
            int idRobo = robo.getId();
            Ponto posRobo = robo.getPosicao();

            /// Verificando se o robo nao e o mais perto da bola e se esta dentro do campo e fora da area
            if(idRobo != idRoboAdvMaisPertoBola && idRobo != idRoboGoleiroAdv && campo->isPontoDentro(posRobo) && !areaEq->isPontoDentro(posRobo)){

                /// Calculando a distancia do robo a bola
                float distRoboBola = squared_distance(posRobo, posBola);

                /// Verificando se o robo esta proximo demais da bola
                if(!roboForaAlcanceBola || distRoboBola > distRobosJogoParado){

                    /// Calculando a distancia ao gol
                    float distRoboGol = squared_distance(posRobo, centroGolEq);

                    //                    cout << sqrt(distRoboGol) << endl;

                    /// Verificando se e a menor distancia ate entao
                    if(distRoboGol < menorDist){
                        idAdvMaisProxGolSemBola = idRobo;
                        menorDist = distRoboGol;

                        //                        cout << "Id do robo " << robo->getId() << " - dist: " << menorDist << endl;
                    }
                }
            }
        }
    }

    //    cout << idAdvMaisProxGolSemBola << endl;

    return idAdvMaisProxGolSemBola;
}

int Avaliador::getIdAdvMaisProxGolSemBola(vector<int> idsRobosAdvMarcados, bool robosForaAlcanceBola){

    /// variáveis auxiliares
    Campo* campo = Sistema::modeloMundo.getCampo();
    Area* areaEq = campo->getAreaEq();
    Ponto centroGolEq = areaEq->getCentro();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    int idAdvMaisProxGolSemBola = -1; /// Id do robô mais próximo do nosso gol sem bola
    float menorDist = numeric_limits<float>::infinity(); /// Menor distância encontrada entre o robô e a nossa área.

    /// Pegando o id do robo adv mais perto da bola
    int idRoboAdvMaisPertoBola = getIdDistAdvMaisPertoBola().first;

    for(int id = 0;id<Configuration::SYSTEM_MAX_ROBOTS;id++){
        Robo* roboAdv = &robosAdv->at(id);

        if(roboAdv->isPresente()){

            /// verificando se o robo atual não é o mais próximo da bola
            int idRoboAdv = roboAdv->getId();
            if(idRoboAdv != idRoboAdvMaisPertoBola){

                /// variáveis auxiliares
                Ponto posRoboAdv = roboAdv->getPosicao();
                float distRoboAdvBola = squared_distance(posRoboAdv, posBola);
                bool analisarRobo = true; /// Variável que irá indicar se iremos considerar esse robô ou não.

                /// verificando se o robo está dentro do campo, se está dentro da area ou se está perto da bola, esses robos não iremos analisar na marcacao
                if(!campo->isPontoDentro(posRoboAdv) || areaEq->isPontoDentro(posRoboAdv) || (robosForaAlcanceBola && distRoboAdvBola <= 650.0*650.0)){
                    analisarRobo = false;
                }else{

                    /// Analisando se o robo já está sendo marcado
                    for(int i= 0; i<idsRobosAdvMarcados.size();i++){

                        /// Se uma dessas condições for verdadeira não iremos marcar esse robô iremos atrás de outro robô para marcar
                        if(idsRobosAdvMarcados[i] == idRoboAdv){
                            analisarRobo = false;
                            break;
                        }
                    }
                }

                /// Verificando se iremos analisar o robô
                if(analisarRobo){
                    float distRoboAdvGolEq = squared_distance(posRoboAdv, centroGolEq);

                    /// se não tiver nenhum robo escolhido ele será o melhor, caso contrário iremos analisar a distancia entre os robos
                    if(idAdvMaisProxGolSemBola == -1){
                        idAdvMaisProxGolSemBola = idRoboAdv;  /// Pegando o id do robô adv mais próximo do nosso gol
                        menorDist = distRoboAdvGolEq; /// Setando a menor distância.

                        /// Verificando se a nova distância calculada é menor que a distância anterior
                    }else if((distRoboAdvGolEq < menorDist && fabs(distRoboAdvGolEq - menorDist) >= ConfigAvaliador::DIST_MARCAR_ROBO)){
                        idAdvMaisProxGolSemBola = idRoboAdv;  /// Pegando o id do robô adv mais próximo do nosso gol
                        menorDist = distRoboAdvGolEq; /// Setando a menor distância.
                    }
                }
            }
        }
    }

    return idAdvMaisProxGolSemBola;
}

int Avaliador::getQtRobosAreaCampoFrenteRoboComBola(int idDesconsiderar, int posCampo){

    /// Pegando a posição do robô que está com a bola, para podermos analisar somente os robôs que estão a sua frente.
    vector<Robo>* robosEq = Sistema::modeloMundo.getRobosEq();
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    IdDistancia roboEqMaisPertoBola = Sistema::avaliador.getIdDistRoboEqMaisPertoBola();
    Ponto posRoboMaisPertoBola = robosEq->at(roboEqMaisPertoBola.first).getPosicao();
    bool ladoCampoEsquerdo = Sistema::modeloMundo.isLadoCampoEsquerdo();

    /// Variáveis que irão armazenar a quantidade de robôs em cada área do campo respectivamente
    int direitoCima = 0, esquerdoCima = 0, direitoBaixo = 0, esquerdoBaixo = 0;
    Ponto posRobo;

    for(int id=0;id<Configuration::SYSTEM_MAX_ROBOTS;id++){
        Robo* roboEq = &robosEq->at(id);

        /// verificando se o robo está presente e se é diferente do id que queremos desconsiderar
        if(roboEq->isPresente() && id != idDesconsiderar){

            /// pegando a posicao do robo para saber se ele esta dentro da area
            posRobo = roboEq->getPosicao();

            /// verificando o lado do campo
            if((!ladoCampoEsquerdo && posRobo.x() < posRoboMaisPertoBola.x()) || (ladoCampoEsquerdo && posRobo.x() > posRoboMaisPertoBola.x())){

                /// Verificando em qual área do campo o robô se encontra o robô se encontra
                if(posRobo.x() > 0 && posRobo.y() > 0) direitoCima++;
                else if(posRobo.x() < 0 && posRobo.y() > 0) esquerdoCima++;
                else if(posRobo.x() < 0 && posRobo.y() < 0) esquerdoBaixo++;
                else if(posRobo.x() > 0 && posRobo.y() < 0) direitoBaixo++;
            }
        }

        /// analisando o robo adversario
        Robo* roboAdv = &robosAdv->at(id);
        if(roboAdv->isPresente()){

            /// pegando a posicao do robo para saber se ele esta dentro da area
            posRobo = roboAdv->getPosicao();

            /// Verificando se o robô que iremos analisar a posição está na frente do robô que está com a bola
            if((!ladoCampoEsquerdo && posRobo.x() < posRoboMaisPertoBola.x()) || (ladoCampoEsquerdo && posRobo.x() > posRoboMaisPertoBola.x())){

                /// Verificando em qual área do campo o robô se encontra o robô se encontra
                if(posRobo.x() > 0 && posRobo.y() > 0) direitoCima++;
                else if(posRobo.x() < 0 && posRobo.y() > 0) esquerdoCima++;
                else if(posRobo.x() < 0 && posRobo.y() < 0) esquerdoBaixo++;
                else if(posRobo.x() > 0 && posRobo.y() < 0) direitoBaixo++;
            }
        }

    }

    /// Analisando qual o lado do campo que iremos atacar e qual posição (emcima ou embaixo) iremos ficar do robô que está com a bola.
    if(!ladoCampoEsquerdo && posCampo == 1) return direitoCima;
    else if(ladoCampoEsquerdo && posCampo == 1) return esquerdoCima;
    else if(!ladoCampoEsquerdo && posCampo == 0)  return direitoBaixo;
    else if(ladoCampoEsquerdo && posCampo == 0)return esquerdoBaixo;

    return -1;
}

vector<float> Avaliador::getDistEntreRobosSegmento(const vector<Ponto>& posicoes, const Segmento& segmento){

    vector<float> distancias(posicoes.size()); /// Vetor com as distâncias dos robôs ao segmento

    /// Pegando cada posição dos robôs.
    for(unsigned int i=0;i<posicoes.size();i++){
        distancias[i] = sqrt(squared_distance(segmento, posicoes[i]));
    }

    return distancias;
}

vector<float> Avaliador::getDistEntreAlvoeOutrosRobos(const vector<Ponto> &posicoes, const Ponto &alvo){
    vector<float> distancias(posicoes.size()); /// Vetor com as distâncias dos robôs ao alvo

    /// Pegando cada posição dos robôs.
    for(unsigned int i=0;i<posicoes.size();i++){
        distancias[i] = sqrt(squared_distance(alvo, posicoes[i]));
    }

    return distancias;
}

float Avaliador::reduzirDistGolAdv(){

    /// Iremos dividir o campo em três partes cada uma com um valor correspondente

    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao(); /// Pegando a posição da bola
    float distanciaGol; /// Distância do robô que irá chutar ao gol adv

    if(Sistema::modeloMundo.isLadoCampoEsquerdo()){
        if (posBola.x() < 0){
            distanciaGol = -1*posBola.x() + Config::MEIO_COMPRIMENTO_CAMPO;
        }else{
            distanciaGol = Config::MEIO_COMPRIMENTO_CAMPO - posBola.x();
        }
    }
    else{
        if (posBola.x() < 0){
            distanciaGol = -1*posBola.x();
        }else{
            distanciaGol = Config::MEIO_COMPRIMENTO_CAMPO + posBola.x();
        }
    }


    /// de 75% até 100% de distancia do gol
    if(distanciaGol > (Config::COMPRIMENTO_CAMPO*3)/4){
        return 1.0;
    }
    else{/// de 25% a 75% do campo

        if(distanciaGol > Config::COMPRIMENTO_CAMPO/4 && distanciaGol < (Config::COMPRIMENTO_CAMPO*3)/4 ){
            return distanciaGol/((Config::COMPRIMENTO_CAMPO*3)/4);
        }
        else{ /// de 0 a 25% do campo
            return (distanciaGol + (Config::COMPRIMENTO_CAMPO/4 - distanciaGol)*0.2)/(Config::COMPRIMENTO_CAMPO/2);
        }
    }

}

Reta Avaliador::calcularRetaRoboAdvBola(){
    /// Variáveis auxiliares
    Area* areaEq = Sistema::modeloMundo.getCampo()->getAreaEq();
    Ponto centroGolEq = areaEq->getCentro();
    Bola* bola = Sistema::modeloMundo.getBola();
    int idRoboAdvMaisPertoBola=-1;
    Reta retaAuxiliar;

    /// Se a bola não for nosso iremos criar um reta do robô adv que está com ela e a posição da bola
    if(!isBolaNossa(idRoboAdvMaisPertoBola)){

        /// Reta é entre o robo adv mais perto da bola e a bola
        if(idRoboAdvMaisPertoBola != -1){

            /// Criando a reta do robô adv que está com a bola e a bola.
            retaAuxiliar = Reta(bola->getPosicao(), Sistema::modeloMundo.getRoboAdv(idRoboAdvMaisPertoBola)->getPosicao());
        }else{
            cout << " ERRO ! " << endl;
        }

        /// Verificando se o vetor de velocidade da bola não é nulo
    }else if(bola->getVetorVelocidade() != CGAL::NULL_VECTOR){
        retaAuxiliar = Reta(bola->getPosicao(), bola->getVetorVelocidade());
        cout << "vel bola: " << sqrt(bola->getVetorVelocidade().squared_length())<< endl;
    }

    /// Se a reta não intercepta o nosso gol, então iremos criar uma reta com a posição da bola e o centro do nosso gol.
    if(retaAuxiliar.is_degenerate() || !areaEq->isInterseccaoReta(retaAuxiliar)){
        retaAuxiliar = Reta(bola->getPosicao(), centroGolEq);
    }

    return retaAuxiliar;
}

vector<Ponto> Avaliador::getRobosEntrePontos(const vector<int>& idRobosEqIgnorar, const vector<int>& idRobosAdvIgnorar,
                                             const Segmento& segPontoInicialAlvo, float distSegmento){

    /// variáveis auxiliares
    Campo* campo = Sistema::modeloMundo.getCampo();
    vector<Robo>* robosEq = Sistema::modeloMundo.getRobosEq();
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    double squaredDistSegmento = distSegmento*distSegmento; /// Usando a distancia ao quadrado pra diminiur a complexidade do calculo
    vector<Ponto> posRobos; /// Posição dos robôs que estão

    /// Pegando os robos da equipe proximos do segmento
    for(int id = 0; id < Configuration::SYSTEM_MAX_ROBOTS; id++){
        Robo* roboEq = &robosEq->at(id);

        /// Pegando os robos da equipe que estão proximos do segmento
        if(roboEq->isPresente()){
            Ponto posRoboEq = roboEq->getPosicao();
            bool ignorarRobo = false;

            /// Verificando se o robo atual esta no vetor dos robos que serao ignorados ou se ele nao esta dentro do campo
            BOOST_FOREACH(int idRoboIgnorar, idRobosEqIgnorar){
                if(idRoboIgnorar == id || !campo->isPontoDentro(posRoboEq)){
                    ignorarRobo = true;
                    break;
                }
            }

            /// Senao formos ignorar o robo iremos pegar a posicao dele e inserir no vetor
            if(!ignorarRobo && (squared_distance(segPontoInicialAlvo, posRoboEq) < squaredDistSegmento)){
                posRobos.push_back(posRoboEq);
            }
        }

        /// Pegando os robos adv que estão proximos do segmento
        Robo* roboAdv = &robosAdv->at(id);
        if(roboAdv->isPresente()){
            Ponto posRoboAdv = roboAdv->getPosicao();
            bool ignorarRobo = false;

            /// Verificando se o robo atual esta no vetor dos robos que serao ignorados ou se ele nao esta dentro do campo
            BOOST_FOREACH(int idRoboIgnorar, idRobosAdvIgnorar){
                if(idRoboIgnorar == id || !campo->isPontoDentro(posRoboAdv)){
                    ignorarRobo = true;
                    break;
                }
            }

            /// Senao formos ignorar o robo iremos pegar a posicao dele e inserir no vetor
            if(!ignorarRobo && squared_distance(segPontoInicialAlvo, posRoboAdv) < squaredDistSegmento){
                posRobos.push_back(posRoboAdv);
            }
        }
    }

    return posRobos;
}

Reta Avaliador::calcularRetaDirecaoBola()
{
    /// criando uma reta com base na direção da bola
    Bola* bola = Sistema::modeloMundo.getBola();
    Reta retaDirecao(bola->getPosicao(), bola->getPosicaoAnterior());

    //        Reta retaAuxiliar(bola->getPosicao(), bola->getPosicaoAnterior()-bola->getPosicao());

    //    cout << retaAuxiliar.is_degenerate() << endl;
    //    cout << interseccaoRetaArea(campo.getAreaEq(), retaAuxiliar) == CGAL::ORIGIN <<endl;

    /// Se a reta não intercepta o nosso gol, então iremos criar uma reta com a posição da bola e o centro do nosso gol.
    //    if(retaAuxiliar.is_degenerate() || interseccaoRetaArea(campo.getAreaEq(), retaAuxiliar) == CGAL::ORIGIN)
    //    {
    //        retaAuxiliar = Reta(Sistema::modeloMundo.getBola()->getPosicao(), Sistema::modeloMundo.centroGolEq);
    //    }

    return retaDirecao;
}

float Avaliador::analisarAlvo(const float& distSegmento, const float& distRobo){

    /// Constanstes definadas para alterarmos os valores que queremos receber. Se serão muito filtrados ou não.
    float K = float(ConfigAvaliador::K_ANALISE);
    float Q = float(ConfigAvaliador::Q_ANALISE);

    /** A primeira parcela é sobre a distancia do robô �  reta enquanto a segunda é sobre a distancia robo.

        Quanto maior for 'K' maior será o score retornado. O que realmente interessa é o grau de decrescimento
        deterado por Q, o valor de Q é nove vezes menor para a parcela das distancias ao robo.

        A segunda parcela dentro da raiz, 'Q' representa um valor inicial para função, quanto mais alto for esse
        valor mais baixo será a valor da função se esse valor for 0 a função tera valor de K quando a distancia tender a 0.
    */

    /// Equação criada para determinar, dependendo da distância do robô a reta e dependendo da distância ao outro robô,
    /// se é bom para chute ou não.
    return K/exp(sqrt(distSegmento/(K/Q) +0.5)) + K/exp(sqrt(distRobo/(K/(Q/9)) + K/60));
}

float Avaliador::analisarScoreAlvo(const Ponto &pontoInicial, const Ponto &alvo, vector<Ponto> posRobos, bool reducaoScore){
    float score = 0.0; /// Valor que iremos guardar para verificarmos se iremos ou não chutar a bola para o alvo especificado

    /// criando a reta da posição que iremos chutar com o alvo para o qual iremos chutar
    Segmento segPontoInicialAlvo(pontoInicial, alvo);

    /// Calculando a distância dos robôs ao segmento e as distancia dos robôs ao alvo.
    vector<float> distRobosSegmento = getDistEntreRobosSegmento(posRobos, segPontoInicialAlvo);
    vector<float> distRobosAlvo = getDistEntreAlvoeOutrosRobos(posRobos, alvo);

    /// Pegando cada distância e calculando o score de cada uma e somando ao score total.
    for(unsigned int i = 0; i < distRobosSegmento.size();i++){

        /// Calculando o score de cada robô separado e somando ao score total.
        score += analisarAlvo(distRobosSegmento[i], distRobosAlvo[i]);
    }

    /// Verificando se iremos usar o fator de redução para alterar o score dependendo da distância do robo ao gol adv.
    if(reducaoScore){
        //cout << " REDUCAO FOI " << Sistema::avaliador.reducaoDistanciaGolAdv();
        /// Essa reduçao que estamos fazendo serve para aumentarmos o score caso o robô esteje muito longe do gol adversário.
        score = score * reduzirDistGolAdv();
    }

    return score;
}

float Avaliador::calcularAnguloAbertura(Ponto alvo, float largura){

    /// Pega a o posicao da bola que vai ser a origem dos dois vetores criados
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    /// Calcula uma reta entre a posicao da bola e o  meio da area de chute (ALVO)
    Reta retaBolaAlvo(posBola, alvo);

    /// a partir da reta bola alvo se cria uma reta para deslocamento dos pontos para criar a area de chute.
    Reta retaDeslocamento = retaBolaAlvo.perpendicular(alvo);

    /// cria dois pontos finais dos vetores
    Ponto p1 = Geometria::calcularPontoEmRetaComDesloc(alvo, retaDeslocamento, largura);
    Ponto p2 = Geometria::calcularPontoEmRetaComDesloc(alvo, retaDeslocamento, -largura);

    /// calculando angulo e dividindo para saber o angulo de um dos lados
    return Geometria::calcularAnguloVetores(Vetor(posBola, p1), Vetor(posBola, p2))/2.0;
}

void Avaliador::calculaAnguloChuteGol(){ //ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt
    /// variaveis auxiliares
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();
    Ponto centroGolAdv = Sistema::modeloMundo.getCampo()->getAreaAdv()->getCentro();

    float distanciax = posBola.x() - centroGolAdv.x();
    float distanciay = posBola.y() - centroGolAdv.y();

    float distancia = sqrt(distanciax*distanciax + distanciay*distanciay);

}

float Avaliador::calcularAnguloAberturaGol(){

    /// variáveis auxiliares
    Ponto centroGolAdv = Sistema::modeloMundo.getCampo()->getAreaAdv()->getCentro();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    /// Calcula a posicao das traves do gol adversario
    Ponto trave1 = Ponto(centroGolAdv.x(), centroGolAdv.y()+Config::COMPRIMENTO_GOL/2.0);
    Ponto trave2 = Ponto(centroGolAdv.x(), centroGolAdv.y()-Config::COMPRIMENTO_GOL/2.0);

    /// Calcula os vetores entre a bola e as traves
    Vetor bolaTrave1 = Vetor(posBola,trave1);
    Vetor bolaTrave2 = Vetor(posBola,trave2);

    return Geometria::calcularAnguloVetores(bolaTrave1,bolaTrave2);
}

float Avaliador::calcularAnguloAberturaGol(Ponto alvo, float largura){

    /// Pega a o posicao da bola que vai ser a origem dos dois vetores criados
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    /// cria dois pontos finais dos vetores
    Ponto p1 = Ponto(alvo.x(), alvo.y() + largura);
    Ponto p2 = Ponto(alvo.x(), alvo.y() -largura);

    return Geometria::calcularAnguloVetores(Vetor(posBola, p1), Vetor(posBola, p2));
}

bool Avaliador::roboAdvTocouBola(){

    /// Variáveis auxiliares
    vector<Robo>* robosAdv = Sistema::modeloMundo.getRobosAdv();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    BOOST_FOREACH(Robo& roboAdv, *robosAdv){

        /// Verificando se o robo esta presente na partida
        if(roboAdv.isPresente()){

            /// Pegando a distância do robô adv a bola
            float distRoboAdvBola = squared_distance(roboAdv.getPosicao(), posBola);

            /// Verificando se a distância do robô adv a bola é menor que a tolerância
            if(distRoboAdvBola <= ConfigAvaliador::TOL_TOQUE*ConfigAvaliador::TOL_TOQUE)
                return true;
        }
    }

    return false;
}

bool Avaliador::roboEqTocouBola(){

    /// variáveis auxiliares
    vector<Robo>* robosEq = Sistema::modeloMundo.getRobosEq();
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao();

    BOOST_FOREACH(Robo& robo, *robosEq){

        /// Verificando se o robo esta presente na partida
        if(robo.isPresente()){

            /// Pegando a distância do robô adv a bola
            float distRoboAdvBola = squared_distance(robo.getPosicao(), posBola);

            /// Verificando se a distância do robô adv a bola é menor que a tolerância
            if(distRoboAdvBola <= ConfigAvaliador::TOL_TOQUE*ConfigAvaliador::TOL_TOQUE)
                return true;
        }
    }

    return false;
}

Ponto Avaliador::calcularAlvoComAfastamentoRetaIncidente(const Ponto& pontoInterseccao, float distAfastamento){
    Reta retaDeslocAlvo; /// Reta aonde o alvo será calculado de acordo como o deslocamento

    /// Verificando se a reta de posicionamento da defesa, está criada e se não se degenere em um ponto, caso aconteça iremos
    /// posicionar os robôs em frente da área, caso não iremos pegar a própria reta para poder calcular um novo alvo com o deslocamento
    if(!retaCirculo.is_degenerate()){
        retaDeslocAlvo = retaCirculo; /// Pegando a reta perpendicular ao ponto para fazer o posicionamento
    }else{
        retaDeslocAlvo = Reta(pontoInterseccao, Ponto(pontoInterseccao.x(), pontoInterseccao.y()+1));
    }

    /// Calculando o novo ponto na reta de acordo com o deslocamento
    return Geometria::calcularPontoEmRetaComDesloc(pontoInterseccao, retaDeslocAlvo, distAfastamento);
}

Ponto Avaliador::calcularAlvoPosicionarPasseCurto(string tipoPosicao){

    /// Variáveis auxiliares
    Ponto posBola = Sistema::modeloMundo.getBola()->getPosicao(); /// Pegando a posição da bola
    Ponto centroGolAdv = Sistema::modeloMundo.getCampo()->getAreaAdv()->getCentro();

    /// Criando a região com centro na bola e raio definido
    Circulo circuloRegiao(posBola, ConfigAvaliador::RAIO_REGIAO_RECEBER_PASSE*ConfigAvaliador::RAIO_REGIAO_RECEBER_PASSE);
    Reta retaCortaRegiaoPasse(posBola, Ponto(posBola.x(), posBola.y()+1));

    /// Pegando os pontos que essa reta intercepta o circulo da regiao
    vector<Ponto> pontosInterseccao;
    bool intercepta = circuloRegiao.isInterseccaoReta(retaCortaRegiaoPasse, pontosInterseccao);

    /// Alvo que iremos calcular para o robô ir para receber o passe curto de outro robô
    Ponto alvoParaPosicionar;

    /// Verificando qual ponto queremos pegar, se é um ponto para dar o passe ou receber o passe
    if(tipoPosicao == "dara_passe"){

        /// Analisando o ponto com maior y para dar o passe
        if(squared_distance(pontosInterseccao[0], centroGolAdv) >= squared_distance(pontosInterseccao[1], centroGolAdv)){
            alvoParaPosicionar = pontosInterseccao[0];
        }else{
            alvoParaPosicionar = pontosInterseccao[1];
        }

    }else if(tipoPosicao == "recebera_passe"){

        /// Analisando o ponto com menor y para receber passe
        /// Analisando o ponto com maior y para dar o passe
        if(squared_distance(pontosInterseccao[0], centroGolAdv) <= squared_distance(pontosInterseccao[1], centroGolAdv)){
            alvoParaPosicionar = pontosInterseccao[0];
        }else{
            alvoParaPosicionar = pontosInterseccao[1];
        }
    }

    return alvoParaPosicionar;
}



Ponto Avaliador::calcularAlvoAtrasRobo(){

    /// Pegando a posição do robô que está com a bola
    Ponto posRoboBola = Sistema::modeloMundo.getRoboEq(getIdDistRoboEqMaisPertoBola().first)->getPosicao();

    int x; /// Distância x que o robô que irá passar por trás ficará do robô que está com a bola.

    /// Verificando o lado do nosso campo, dependendo do lado iremos diminuir ou somar a coordenada o diâmetro do robô.
    if(Sistema::modeloMundo.isLadoCampoEsquerdo()){
        x = posRoboBola.x() - RAIO_ROBO*RAIO_ROBO; /// Subtraindo as coordenadas para poder ficar atrás.
    }else{
        x = posRoboBola.x() + RAIO_ROBO*RAIO_ROBO; /// Somando as coordenadas para poder ficar atrás.
    }

    return Ponto(x, posRoboBola.y());           /// Criando o ponto atrás do robô que está com a bola.
}

bool Avaliador::isBolaNossa(int& idAdvMaisPertoBola){

    // Pegando os robos mais pertos da bola das duas equipes
    IdDistancia posDisEq = getIdDistRoboEqMaisPertoBola();
    IdDistancia posDisAdv = getIdDistAdvMaisPertoBola();

    /// Setando o id do robo adv mais perto da bola
    idAdvMaisPertoBola = posDisAdv.first;

    return posDisEq.second < posDisAdv.second;
}

bool Avaliador::isBolaNossa(){

    /// Pegando o robo da nossa equipe e o adv mais perto da bola e verificando se a bola e nossa
    IdDistancia posDisEq = getIdDistRoboEqMaisPertoBola();
    IdDistancia posDisAdv = getIdDistAdvMaisPertoBola();

    return posDisEq.second < posDisAdv.second;
}

bool Avaliador::isPontoLadoEq(const Ponto &ponto){
    ControlPackage::FieldSide lado = Sistema::modeloMundo.getLadoCampo();

    /// Funcao varia dependendo do lado do campo utilizado
    if(lado == ControlPackage::Left){

        /// Para o lado esquerdo um x positivo é no campo adversario e um x negativo é no campo da equipe
        if(ponto.x() < 0){
            return true;
        }else{
            return false;
        }

    }else{

        /// Para o lado direito um x negativo é no campo adversario e um x positivo é no campo da equipe
        if(ponto.x() > 0){
            return true;
        }else{
            return false;
        }
    }
}

bool Avaliador::isPontoLadoAdv(const Ponto &ponto){
    return !isPontoLadoEq(ponto); /// o campo adversario é sempre o inverso do da equipe
}

Ponto Avaliador::calcularAlvoGirar(const Ponto& pontoParaGirar, bool horario, int idRobo){

    /// Pegando a posição do robo para servir como centro de um dos circulos
    Ponto posRobo = Sistema::modeloMundo.getRoboEq(idRobo)->getPosicao();

    /// Pegando a distância ao quadrado do robo ao ponto para girar para servir como raio de um dos circulos
    //    float distPontoRoboQuadrada = squared_distance(posRobo, Sistema::modeloMundo.getBola()->getPosicao());
    float distPontoRoboQuadrada = squared_distance(posRobo, pontoParaGirar);

    /// Criando os circulos ao redor da bola que servirá como deslocamento
    Circulo circuloBola(pontoParaGirar, distPontoRoboQuadrada);

    /// Criando o circulo ao redor do robo para indentificar qual é o sentido que temos que girar para alcançar o alvo
    double  raioCirculoRobo = ConfigAvaliador::RAIO_CIRCULO_GIRAR*ConfigAvaliador::RAIO_CIRCULO_GIRAR;
    if(distPontoRoboQuadrada < ConfigAvaliador::RAIO_CIRCULO_GIRAR*ConfigAvaliador::RAIO_CIRCULO_GIRAR)
        raioCirculoRobo = distPontoRoboQuadrada;

    Circulo circuloRobo(posRobo, raioCirculoRobo);

    /// Calculando os pontos que interseccionam os circulos
    vector<Ponto> pontosInterseccao = Geometria::interseccaoCirculoCirculo(circuloBola, circuloRobo);

    /// Pegando os pontos que iremos usar para direcionar o robo
    Ponto p1 = pontosInterseccao[0];
    Ponto p2 = pontosInterseccao[1];

    /// Criando os vetores para calcular o produto vetorial para indicar pra que lado girar
    Vetor pontoRobo(pontoParaGirar, posRobo);
    Vetor pontoP1(pontoParaGirar, p1);

    /// Normalizando os vetores para poder calcular o produto vetorial
    pontoP1 = pontoP1/sqrt(pontoP1.squared_length());
    pontoRobo = pontoRobo/sqrt(pontoRobo.squared_length());

    if(Geometria::produtoVetorial2D(pontoRobo, pontoP1) < 0){
        return horario ? p1 : p2;
    }else{
        return horario ? p2 : p1;
    }
}
