#include "Sistema.h"
#include "Chute.h"
#include "Halt.h"
#include "Limpa.h"
#include "Marcar.h"
#include "MarcarSaidaBola.h"
#include "PasseCurto.h"
#include "Posiciona.h"
#include "PosicionaChute.h"
#include "DefesaCirculo.h"
#include "PosicionaPerdaBola.h"
#include "PosicionaSegmento.h"
#include "RecebePasseCurto.h"
#include "RoubaBola.h"
#include "Gira.h"
#include "DefendeGol.h"
#include "DefendePenalty.h"
#include "AndarReto.h"
#include "ReceberPasse.h"
#include "PasseLongo.h"
#include "DefesaArea.h"
#include "MarcarAreaAdv.h"

#include "Carregador.h"

Carregador::Carregador()
{
    cout << "INICIANDO CARREGADOR" << endl;
    nomeUltArqConstantesUsado = "";
    jogadas = new vector<Jogada>;

    /// Adicionando as táticas no sistema para podermos carregar as jogadas.
    taticas.push_back(new Chute);
    taticas.push_back(new Halt);
    taticas.push_back(new Limpa);
    taticas.push_back(new Marcar);
    taticas.push_back(new MarcarSaidaBola);
    taticas.push_back(new PasseCurto);
    taticas.push_back(new PasseLongo);
    taticas.push_back(new Posiciona);
    taticas.push_back(new PosicionaChute);
    taticas.push_back(new PosicionaPerdaBola);
//    taticas.push_back(new PosicionaDefesa<Poligono>);
    taticas.push_back(new DefesaArea);
    taticas.push_back(new DefesaCirculo);
    taticas.push_back(new PosicionaSegmento);
    taticas.push_back(new RecebePasseCurto);
    taticas.push_back(new RoubaBola);
    taticas.push_back(new Gira);
    taticas.push_back(new DefendeGol);
    taticas.push_back(new DefendePenalty);
    taticas.push_back(new AndarReto);
    taticas.push_back(new ReceberPasse);
    taticas.push_back(new MarcarAreaAdv);
}

Carregador::~Carregador(){
    delete jogadas;

    for(unsigned int i = 0; i < taticas.size(); i++){
        delete taticas[i];
    }
}

bool Carregador::isTemJogadas()
{
    return !jogadas->empty();
}

bool Carregador::isCarregamentoSemErro()
{
    /// Verificando se carregamos alguma jogada e se não temos nenhuma jogada que deu erro,
    /// se isso acontecer poderemos iniciar o sistema
    if(jogadas->size() > 0 && logJogadasComErros == ""){
        return true;
    }

    return false;
}

string Carregador::getLogJogadasComErros()
{
    return logJogadasComErros;
}

void Carregador::addLogJogadasComErros(string erro){
    /// Adicionando o erro no log do erro no carregmaneot das jogadas.
    logJogadasComErros.append(nomeJogada + "\n" + erro + "\n\n");
}

vector<Jogada>* Carregador::getJogadas(){
    return jogadas;
}

vector<string> Carregador::getNomeArquivosJogadas()
{
    return nomeArquivosJogadas;
}

void Carregador::setPathPastaJogadas(string _pathPastaJogadas){
    pathPastaJogadas = _pathPastaJogadas;
}

void Carregador::setNomeJogadasEscolhidas(const vector<string> &_nomeJogadasEscolhidas)
{
    nomeJogadasEscolhidas = _nomeJogadasEscolhidas;
}

bool Carregador::pegarNomeArquivosJogadas(string _pathPastaJogadas){

    pathPastaJogadas = _pathPastaJogadas;

    /// Criando um arquivo com o nome de todas as jogadas da pasta passada como parametro.
    if(pathPastaJogadas != ""){

        /// indo até a pasta das jogadas e criando um arquivo com o nome de todas as jogadas da pasta
        int erro = system(("cd " + pathPastaJogadas + " && ls > ./nomeArquivos.txt").c_str());

        /// Verificado se houve algum erro na execução
        if(!erro){

            ifstream arq((pathPastaJogadas + "nomeArquivos.txt").c_str());

            /// Verificando se deu certo abrir o arquivo.
            if(arq.is_open()){

                string valorArq; /// Valor que iremos ler do arquivo
                nomeArquivosJogadas.clear(); /// Limpando o vetor com o nome das jogadas para podermos pegar as novas jogadas.

                /// Iremos ler os nomes até acabar o arquivo.
                while(!arq.eof()){

                    arq >> valorArq; /// Pegando o nome da jogada.

                    /// Iremos inserir todos os nomes menos esse que é o nome do arquivo que estamos lendo.
                    if(isupper(*valorArq.begin()) &&  *(valorArq.end()-1) == 'j'){
//                        cout << valorArq << endl;
                        valorArq.erase(valorArq.end() - 4, valorArq.end()); /// Removendo a extensão do arquivo '.adj'.
                        nomeArquivosJogadas.push_back(valorArq); /// Adicionando o nome ao vetor dos nomes.
                    }
                }

                arq.close();
                remove((pathPastaJogadas + "nomeArquivos.txt").c_str()); /// Removendo o arquivo de onde lemos o nome das jogadas.

                return true;
            }

            remove((pathPastaJogadas + "nomeArquivos.txt").c_str()); /// Removendo o arquivo de onde lemos o nome das jogadas.
            cout << "Caminho da pasta está incorreto." << endl;
            return false;
        }

        remove((pathPastaJogadas + "nomeArquivos.txt").c_str()); /// Removendo o arquivo de onde lemos o nome das jogadas.
        cout << "Caminho da pasta está incorreto." << endl;
        return false;
    }

    remove((pathPastaJogadas + "nomeArquivos.txt").c_str()); /// Removendo o arquivo de onde lemos o nome das jogadas.
    cout << "Insira o caminho da pasta no campo de texto." << endl;
    return false;
}

void Carregador::carregarJogadasEscolhidas(){

    logJogadasComErros =  ""; /// Limpando o log de erros do carregamento das jogadas.
    jogadas->clear(); /// Limpando todas as jogadas para recarregar novamente

    /// Carregando todas as jogadas que foram selecionadas.
    for(unsigned int i = 0; i < nomeJogadasEscolhidas.size(); i++){

        Jogada jogada; /// Variável que irá armazenar as informações da jogada lida do arquivo.

        /// Verificando se a jogada já foi carregada, senão tiver sido carregada, iremos carregar e verificar se deu certo.
        if(carregarJogada(jogada, pathPastaJogadas + nomeJogadasEscolhidas[i] + ".adj", i)){
            jogadas->push_back(jogada);         /// Adicionando a jogada lida do arquivo no vetor das jogadas.
            cout << jogada.getNome() << endl;
        }
    }
}

bool Carregador::carregarJogada(Jogada &jogada, string pathArquivo, unsigned int idJogada){
    fstream arq; /// Atributo que iremos usar para fazer as operações com o arquivo da jogada.

    jogada.setId(idJogada); /// Adicionando o id da jogada atual.

    arq.open(pathArquivo.c_str(), fstream::in); /// Abrindo arquivo como modo de leitura.

    /// Verificando se conseguimos abrir, se não enviamos uma mensagem de erro.
    if(arq.is_open()){

        /** Na leitura do arquivo sempre iremos verificar se digitaram todas as palavras chaves corretamente*/
        string valorArq;


        /// ***** Lendo o Nome da JOGADA ***** .
        arq >> valorArq;                            /// Pegando a palavra-chave JOGADA.

        if(valorArq == "JOGADA"){
            arq >> valorArq;                        /// Pegando o nome da jogada.

            /// Verificando se o cara digitou o nome da jogada.
            if(valorArq == ""){
                addLogJogadasComErros("A palavra-chave 'JOGADA' nao existe.\n\n");
                return false;
            }

            jogada.setNome(valorArq);               /// Setando o valor na jogada.

        }else{
            addLogJogadasComErros("Jogada nao tem nome.\n\n");
            return false;
        }


        /// ***** Lendo as Condições para a jogada ser APLICÁVEL *****
        arq >> valorArq;                            /// Pegando a palavra-chave APLICAVEL.

        if(valorArq == "APLICAVEL"){

            /// Caso não dê certo ler as condições iremos parar a leitura.
            if(!lerCondAplicavel(arq, jogada)) /// Iremos pegar as condiçoes para a jogada ser 'APLICAVEL' e seus valores.
                return false;

        }else{
            addLogJogadasComErros("A palavra-chave 'APLICAVEL'' nao existe.\n\n");
            return false;
        }

        /// ***** Lendo as condições para a jogada ser FINALIZADA. *****

        /// Caso não dê certo ler as condições iremos parar a leitura.
        if(!lerCondFinaliza(arq, jogada))     /// Lendo as condições e o modo da jogada ser finalizada.
            return false;

        /// ***** Lendo as Pos Condições da Jogada *******
        arq >> valorArq;

        /// Se o próximo valor for um caractere e for minusculo, então indica que temos que ler as pos condições da jogada
        if(islower(*valorArq.begin()+1) && isalpha(*valorArq.begin())+1){

            /// Lendo as pos condições da jogada
            do{
                bool valor = true;
                if(ispunct(*valorArq.begin())){
                    valorArq.erase(valorArq.begin());
                    valor = false;
                }

                jogada.addPosCond(valorArq, valor); /// adicionando a condição na jogada

                arq >> valorArq; /// lendo o proximo valor do arquivo

            }while(islower(*valorArq.begin()+1) && isalpha(*valorArq.begin())+1);

            /// *** Lendo o TIMEOUT ***/

            /// Verificando se o próximo valor lido é uma palavra-chave se for não poderemos ler o arquivo
            if(!isupper(*valorArq.begin())){
                addLogJogadasComErros("O 'TIMEOUT' não existe.\n\n");
                return false;
            }

            arq >> valorArq; /// Lendo o valor do TIMEOUT
        }

        /// ***** Continuando Lendo o TIMEOUT. ******

        /// Verificando se digitamos um valor inteiro no timeout, senão não poderemos usar essa jogada.
        if(isalpha(*valorArq.begin())){
            addLogJogadasComErros("O valor do 'TIMEOUT' esta incorreto.\n\n");
            return false;
        }

        jogada.setTimeOut(atoi(valorArq.c_str())); /// Setando o timeOut na jogada.


        /// ***** Lendo se tiver as informações dos ROBÔS ADVERSÁRIOS. *****
        if(!lerRobosAdv(arq, jogada))      /// Caso não dê certo ler as informações dos robôs adversários iremos parar a leitura.
            return false;


        /// ***** Lendo a Tática que o Goleiro irá executar na Jogada ******
        if(!lerTaticaGoleiro(arq, jogada)) /// Caso não consigamos ler a tática do goleiro iremos parar a leitura.
            return false;


        /// ***** Lendo os PAPÉIS da jogada. *****
        arq >> valorArq; /// Pulando o nome de PAPEL1
        /// Laço criado para ler os quatros Papeis da Jogada.
        for(int i = 0; i < NUM_MAX_ROBOS_JOGANDO-1; i++){

            /// Verificando se temos alguma tática para ler, senão tivermos então não poderemos usar essa jogada.
            if(!lerTaticas(arq, jogada, i))        /// Lendo as táticas do arquivo pode ser uma ou mais, e adicionar na jogada.
                return false;

            /// Verificando se conseguimos ler as táticas desse papel, senão tivermos lido nenhuma tática, então não poderemos usar essa jogada.
            if(jogada.getPapel(i).size() == 0){
                stringstream out;
                out << "Insira a(s) tatica(s) do Papel " << i << ".\n\n";
                addLogJogadasComErros(out.str());
                return false;
            }

            /// Pulando as palavras Papeis
            arq >> valorArq;
        }

        arq.close();
        return "";
    }

    arq.close();
    addLogJogadasComErros("Erro ao abrir o arquivo.\n");
    return false;
}

bool Carregador::lerCondAplicavel(fstream &arq, Jogada &jogada){
    string valorArq;                               /// Guarda a condicao que iremos ler.
    bool valor;                                    /// Indica o valor da condição que iremos ler.

    arq >> valorArq;                               /// Ler a primeira condição.

    /// Iremos ler as condições até que o valor lido seja uma palavra-chave
    while(!isupper(*valorArq.begin())){

        /// Verificando se a condicao tem o caractere '!', para podermos definir o valor da condição.
        if(ispunct(*valorArq.begin())){
            valorArq.erase(valorArq.begin());      /// Removendo o sinal da condicao.
            valor = false;                         /// Setando o valor da condição como false.
        }else{
            valor = true;                          /// Setando o valor da condição com true.
        }

        jogada.addCondAplicavel(valorArq, valor);  /// Inserindo a condição na jogada.

        arq >> valorArq;                           /// Pegar o próximo valor do arquivo.
    }

    /// Verificando se lemos alguma condição para a jogada ser aplicável, senão tivermos lido não poderemos usar essa jogada.
    if(jogada.getCondAplicavel().size() == 0){
        addLogJogadasComErros("A jogada nao possui condicoes para ser aplicavel");
        return false;
    }

    return true;
}

bool Carregador::lerCondFinaliza(fstream &arq, Jogada &jogada){
    string valorArq;                               /// Guarda o valor lido do arquivo.
    string modo;                                   /// Iniciando o modo que a jogada será finalizada.

    arq >> valorArq;                               /// Lendo o próximo valor do arquivo.

    while(!isupper(*valorArq.begin())){            /// Verificando se a próxima palavra é uma palavra-chave

        modo = valorArq;                           /// Pegando o modo das condições da jogada.

        arq >> valorArq;                           /// Lendo o próximo valor do arquivo.

        map<string, bool> condicoes;               /// Condições para que a jogada seja finalizada.
        bool valor;                                /// Variável que irá armazenar o valor para os fatos serem aplicados

        /// Verificando se o caractere lido foi '|', senão iremos continuar lendo as condições.
        while(*valorArq.begin() != '|' && !isupper(*valorArq.begin())){

            /// Verificando se a condicao tem o caractere '!', para podermos definir o valor da condição.
            if(ispunct(*valorArq.begin())){
                valorArq.erase(valorArq.begin());  /// Removendo o sinal da condicao.
                valor = false;                     /// Setando o valor da condição como false.
            }else{
                valor = true;                      /// Setando o valor da condição com true.
            }

            condicoes[valorArq] = valor;           /// Setando o valor da fato no vetor das condições

            arq >> valorArq;                       /// Lendo o próximo valor do arquivo.
        }

        jogada.addCondFinaliza(modo, condicoes);   /// Adicionando o modo e as condições correspondentes a jogada.

        /// Verificando se estamos lendo um palavra chave se estivermos temos que passar para a proxima etapa da leitura do arquivo;
        if(isupper(*valorArq.begin()))
            break;

        arq >> valorArq;
    }

    /// Verificando se lemos alguma condição para a jogada ser finalizada, se não tivermos lido, então não poderemos usar essa jogada.
    if(jogada.getCondFinaliza().size() > 0){

        /// Pegando todas as condições da jogada e verificando se a jogada possue condições realmente.
        foreach(Finaliza finaliza, jogada.getCondFinaliza()){

            /// Verificando se tem condições, senão tiver não poderemos usar essa jogada
            if(finaliza.condicoes.size() == 0){
                addLogJogadasComErros("O modo " + finaliza.modo + " nao possui condicoes.");
                return false;
            }
        }

    }else{
        addLogJogadasComErros("A jogada nao possui condicoes para ser finalizada.");
        return false;
    }

    return true;
}

bool Carregador::lerRobosAdv(fstream &arq, Jogada &jogada){
    string valorArq;                                         /// Variável que armazena o valor lido do arquivo.

    arq >> valorArq;                                         /// Pegando o próximo valor a ser lido.

    /// Analisar se iremos pegar alguma informação de um robô da equipe adversária para podermos usar na jogada.
    while(valorArq == "ROBOADV"){

        arq >> valorArq;                                     /// Lendo a posicao do vetor do robô adversário.

        /// Verificando se digitamos o id do robô, senão então não poderemos usar essa jogada.
        if(!isdigit(*valorArq.begin())){
            addLogJogadasComErros("O id do robo adv esta errado.");
            return false;
        }
        int posVetor = atoi(valorArq.c_str());               /// Pegando o id do robô que indica a posição do vetor dos robôs

        arq >> valorArq;                                     /// Lendo o próximo valor do arquivo.

        /// Verificando se o próximo valor do arquivo é uma palavra-chave se for é pq ele não colocou a condição do robô.
        if(isupper(*valorArq.begin())){
            addLogJogadasComErros("O robo adv nao tem condicoes.");
            return false;
        }
        string condicao = valorArq;                          /// Condição para escolher o robô adversário

        /// Adicionando a condição para pegarmos o robô adv do modelo de mundo
        jogada.addCondRoboAdversario(posVetor, condicao);

        arq >> valorArq;                                     /// Lendo o próximo valor do arquivo.
    }

    return true;
}

bool Carregador::lerTaticaGoleiro(fstream &arq, Jogada &jogada){
    string valorArq; /// Variável que irá armazenar o valor do arquivo a ser lido
    arq >> valorArq; /// pegando o valor do arquivo

    /// Verificando se o valor do arquivo é diferente de Papel, isso indica que o usuário digitou a tática
    /// que o goleiro irá executar
    if(valorArq != "PAPEL1"){

        /// Verificando se a primeira letra é maiúscula
        if(isupper(*valorArq.begin())){

            /// Comparando o valor do arquivo com o nome das táticas para identificar qual será a tática digitada
            /// O laço que irá percorrer o vetor das táticas existentes e verificar qual é a tática que iremos usar e pegar uma nova instância.
            for(unsigned int i = 0; i < taticas.size(); i++){
                string nomeTatica = typeid(*taticas[i]).name();             /// Pegando o nome da tática para comparar com o valor do arquivo.

                /// Removendo os digitos do inicio do nome da tática
                while(isdigit(*nomeTatica.begin())){
                    nomeTatica.erase(nomeTatica.begin());                   /// Removendo o primeiro caractere
                }

                /// Verificando se o nome da tática é igual ao nome da classe da tática.
                if(valorArq == nomeTatica){

                    /// Criando uma nova instancia da própria Tática.
                    jogada.setTaticaGoleiro(taticas[i]->getNovaInstancia());
                    return true;
                }
            }

        }else{
            addLogJogadasComErros("A Tática do Goleiro é inválida");
        }

    }else{
        addLogJogadasComErros("Essa Jogada não possui Tática do Goleiro");
    }

    return false;
}

bool Carregador::lerTaticas(fstream &arq, Jogada &jogada, int papel){
    string valorArq;                                                   /// Iremos guardar os valores do arquivo nessa variavel.

    arq >> valorArq;                                                   /// Lendo o valor do arquivo;

    /// Se a próxima palavra que foi lida for uma dessas é porque o arquivo foi feita errado, então não poderemos usar essa jogada.
    if(!isupper(*valorArq.begin())){
        stringstream out;
        out<< "O Papel " << papel << " nao possui taticas.";
        addLogJogadasComErros(out.str());
        return false;
    }

    int cont = 0;   /// Nós usaremos essa variável pra quando não tiver a palavra 'fim' para finalizar a leitura desse papel

    /// Como não sabemos quantas táticas iremos ler usamos while até acharmos uma palavra-chave ou o cont ficar 100, que resulta em um erro no arquivo.
    while(cont < 100){

        /// O laço que irá percorrer o vetor das táticas existentes e verificar qual é a tática que iremos usar e pegar uma nova instância.
        for(unsigned int i = 0; i < taticas.size(); i++){
            string nomeTatica = typeid(*taticas[i]).name();             /// Pegando o nome da tática para comparar com o valor do arquivo.

            /// Verificando se o nome da tática é igual ao nome da classe da tática.
            if(nomeTatica.find(valorArq) != string::npos){

                Tatica* tatica = taticas[i]->getNovaInstancia(); /// Criando uma nova instancia da própria Tática.

                arq >> valorArq; /// Lendo o próximo valor do arquivo

                /// Lendo os parâmetros da tática, até encontramos uma outra tática ou a palavra 'fim'.
                while(!isupper(*valorArq.begin()) && valorArq != "fim"){

                    if(isalpha(*valorArq.begin())){ /// Verificando se o valor é uma string

                        /// Verificando se o parâmetro string é 'true' que é um booleano
                        if(valorArq == "true"){
                            tatica->addParamBool(true); /// Adicionando um booleano true;

                            /// Verificando se o parâmetro string é 'false' que é um booleano
                        }else if(valorArq == "false"){
                            tatica->addParamBool(false); /// Adicionando um booleano false.

                            /// Se nenhuma das condições acima for satisfeita, então o parâmetro é uma string
                        }else{
                            tatica->addParamString(valorArq); /// Adicionando o parâmetro na tática.
                        }

                        /// Verificando se o valor é um ponto.
                    }else if(*valorArq.begin() == '('){
                        tratarPonto(valorArq, arq, tatica);  /// Adicionando o parâmetro ponto na tática.

                        /// Verificando se o valor é um float.
                    }else if(valorArq.find('.') != string::npos){

                        /// Convertendo o parâmetro string para float
                        float param;
                        stringstream out;
                        out << valorArq;
                        out >> param;

                        tatica->addParamFloat(param);  /// Adicionando o parâmetro float na tática.

                        /// Se nenhum das condições acima for satisfeita, então o parâmetro é um inteiro
                    }else{
                        tatica->addParamInt(atoi(valorArq.c_str()));    /// Adicionando o parâmetro int na tática.
                    }

                    arq >> valorArq;                                    /// Lendo o próximo valor do arquivo para fazer a verificação no while
                }

//                /// Verificando se a tática atual que estamos criando é a Posiciona Defesa, se for iremos criar ela de uma forma diferente
//                if(nomeTatica.find("PosicionaDefesa") != string::npos){

//                    PosicionaDefesa<Poligono>* posicionaDefesa = dynamic_cast<PosicionaDefesa<Poligono>* >(tatica);
//                    tatica = posicionaDefesa->reconstruir();
//                    tatica->setParamString(posicionaDefesa->getParamString());
//                    tatica->setParamFloat(posicionaDefesa->getParamFloat());
//                    tatica->setParamInt(posicionaDefesa->getParamInt());
//                    tatica->setParamPonto(posicionaDefesa->getParamPonto());
//                    tatica->setParamBool(posicionaDefesa->getParamBool());
//                }

                jogada.addTaticaPapel(papel, tatica);                   /// Adicionando a tática ao papel.

                /// Verificando qual foi das condições acima que foi satisfeita.
                if(valorArq == "fim"){                                  /// Se o valor lido for 'fim' é porque não temos mais táticas para ler nesse papel.
                    return true;
                }else{                                                  /// Se o valor lido for uma palavra-chave indica que é outra tática a ser lida.
                    break;
                }

            }
        }
        cont++;
    }

    /// Se o cont for igual a 100 é porque o arquivo foi feito de modo errado, então não podemos usar essa jogada.
    if(cont == 100){
        stringstream out;
        out << "O Papel " << papel << " nao possui a palavra-chave 'fim' ou nao possui taticas com o mesmo nome das taticas do codigo do programa.";
        addLogJogadasComErros(out.str());
        return false;
    }

    return true;
}

void Carregador::tratarPonto(string param, fstream &arq, Tatica* tatica){
    int x, y;                             /// Coordenadas do ponto que iremos ler do arquivo;

    param.erase(param.begin());           /// Removendo o caractere '(' para pegarmos somente o valor e tranformar em ponto.

    /// Lendo os pontos x e y
    x = atoi(param.c_str());              /// Fazendo o casting no param para converter para inteiro, e setar a coordenada x.
    arq >> param;                         /// Lendo o próximo valor do arquivo.

    bool lerRaio = true;                  /// Variável que iremos usar para verificar se iremos ou não ler o raio da área.

    /// Verificando se o temos o caractere parênteses no final do valor de y
    if(*(--param.end()) == ')'){
        param.erase(--param.end());       /// Removendo o parênteses para pergamos somente o valor de y
        lerRaio = false;
    }

    y = atoi(param.c_str());              /// Fazendo o casting no param para converter para inteiro, e setar a coordenada y.
    tatica->addParamPonto(Ponto(x, y));   /// Criando um ponto com coordenadas x, y e adicionando na tática.


    /// Verificando se iremos ou não ler o raio da área.
    if(lerRaio){
        float raio;                       /// Variável que iremos armazenar o valor do raio convertido
        stringstream out;                 /// Variável que iremos usar para fazer o casting de string para float

        arq >> param;                     /// Pegando o próximo valor do arquivo.

        param.erase(--param.end());       /// Removendo o caractere ')' do final da string para podermos fazer o casting no param.
        out << param;                     /// Pegando o valor da string
        out >> raio;                      /// Fazendo o casting de string para float

        tatica->addParamFloat(raio);      /// Adicionando o raio �  tática.
    }
}

void Carregador::imprimirInfoJogadas(Jogada jogada){
    /// Mostrando na tela as informações da jogada que foi carregada.

    cout << "Id: " << jogada.getId() << " - Nome da Jogada: " << jogada.getNome() << endl;


    cout << "Condições para a jogada ser aplicável." << endl;

    map<string, bool> condAplicavel = jogada.getCondAplicavel();
    map<string, bool>::iterator it;

    for(it = condAplicavel.begin(); it != condAplicavel.end(); it++){
        cout << "Condição: " << (*it).first << " - Valor: " << (*it).second << endl;
    }

    cout << "Condições para a Jogada ser Finalizada" << endl;

    vector<Finaliza> condFinaliza = jogada.getCondFinaliza();

    foreach(Finaliza finaliza, condFinaliza){
        cout << "Modo: " << finaliza.modo << endl;

        for(it = finaliza.condicoes.begin(); it != finaliza.condicoes.end(); it++){
            cout << "Condição: " << (*it).first << " - Valor: " << (*it).second << endl;
        }
    }

    cout << "Pos condições da Jogada" << endl;
    if(!jogada.getPosCond().empty()){
        for(it = jogada.getPosCond().begin(); it != jogada.getPosCond().end(); it++){
            cout << "Condicao: " << (*it).first << " - Valor: " << (*it).second << endl;
        }
    }

    cout << "TimeOut: " << jogada.getTimeOut() << endl;

    cout << "Robôs Adversários." << endl;

    vector<string> condRobosAdv = jogada.getCondRobosAdv();

    for(unsigned int i = 0; i < condRobosAdv.size(); i++){
        cout << "ID: " << i << endl;
        cout << "Condição: " << condRobosAdv.at(i) << endl;
    }

    cout << "Tática do Goleiro" << endl;
    jogada.getTaticaGoleiro()->imprimirNome();

    cout << "PAPEIS DA JOGADA" << endl;
    for(int i = 0; i < NUM_MAX_ROBOS_JOGANDO-1; i++){
        vector<Tatica*> papel = jogada.getPapel(i);

        cout << "Táticas do Papel " << i << ":" << endl;

        foreach(Tatica* tatica, papel){
            tatica->imprimirNome();
            cout << "Parâmetros Booleanos:" << endl;
            for(unsigned int i = 0; i < tatica->getParamBool().size(); i++){
                cout << tatica->getParamBool()[i] << endl;
            }
            cout << endl;

            cout << "Parâmetros Floats:" << endl;
            for(unsigned int i = 0; i < tatica->getParamFloat().size(); i++){
                cout << tatica->getParamFloat()[i] << endl;
            }
            cout << endl;

            cout << "Parâmetros Inteiros:" << endl;
            for(unsigned int i = 0; i < tatica->getParamInt().size(); i++){
                cout << tatica->getParamInt()[i] << endl;
            }
            cout << endl;

            cout << "Parâmetros Pontos:" << endl;
            for(unsigned int i = 0; i < tatica->getParamPonto().size(); i++){
                cout << tatica->getParamPonto()[i].x() << ", " << tatica->getParamPonto()[i].y() << endl;
            }
            cout << endl;

            cout << "Parâmetros Strings:" << endl;
            for(unsigned int i = 0; i < tatica->getParamString().size(); i++){
                cout << tatica->getParamString()[i] << endl;
            }
            cout << endl;
        }
    }
}
