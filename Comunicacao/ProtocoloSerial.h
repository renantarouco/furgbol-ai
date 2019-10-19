#ifndef PROTOCOLOSERIAL_H
#define PROTOCOLOSERIAL_H

#include "Comando.h"
#include "Definitions.h"


/**
  \class ProtocoloSerial

    \brief  Classe responsável pela organização e serialização do pacote de envio serial
  */

class ProtocoloSerial
{
public:

    ProtocoloSerial();

    void setId(unsigned char _id);
    void limpaPacote();


    void setVelocidadePercentualRodas(int numRoda, unsigned char _vel);
    void setVelocidadePercentualRodas(vector <unsigned char> _vel);

    void setDirecaoRodas(int numRoda, Direcao _dir);
    void setDirecaoRodas(vector<Direcao> _dir);

    void setVelocidadePercentualDriblador(unsigned char _vel);
    void setDirecaoDriblador(Direcao _dir);

    void setTipoChute(Comando::TipoChute _tipoChute);

    void setNivelChute(unsigned char _nivelPercentual);

    void setEnableDrible(bool _enable);

    void setEnableChute(bool _enable);

    void serializa(vector <unsigned char> & buffer) const;

    friend ostream & operator<<(ostream & out, ProtocoloSerial pct);

    inline void setPkgId(uint8_t pkg_id) { pkg_id_ = pkg_id; }
    inline void setMsgType(uint8_t msg_type) { msg_type_ = msg_type; }
    inline void setVelX(uint8_t vel_x) { velocity_x_ = vel_x; }
    inline void setVelY(uint8_t vel_y) { velocity_y_ = vel_y; }
    inline void setVelTheta(uint8_t vel_theta) { velocity_theta_ = vel_theta; }
    inline void setDirX(uint8_t dir_x) { direction_x_ = dir_x; }
    inline void setDirY(uint8_t dir_y) { direction_y_ = dir_y; }
    inline void setDirTheta(uint8_t dir_theta) { direction_theta_ = dir_theta; }
    inline void setDribbler(uint8_t dribbler) { dribbler_ = dribbler; }
    inline void setKick(uint8_t kick) { kick_ = kick; }

private:
    unsigned char id;
    vector <unsigned char> velocidadePercentualRodas;
    unsigned char velocidadePercentualDriblador;
    vector <Direcao> direcaoRodas;
    Direcao direcaoDriblador;
    Comando::TipoChute tipoChute;
    unsigned char nivelChute;
    unsigned char enableChute;
    bool enableDrible;

    void printData() const;

    //para o modelo cinemático

    float raio;                     //Raio do Robo
    float raioDaRoda;               //Raio da roda do robo

    cv::Mat_ <float> matrizRotacao;     //Matriz para transformar o sist de coordenadas do global para o do robo
    cv::Mat_ <float> M;                 //Matriz de transformação de velocidades vx,vy,vgiro para as rodas
    cv::Mat_ <float> pInvM;             //Matriz pseudo inversa da matriz M

    static const int VEL_MAX_RODAS = 126;  //!< DESCOBRIR ESSE VALOR

    uint8_t pkg_id_;
    uint8_t msg_type_;
    uint8_t velocity_x_;
    uint8_t velocity_y_;
    uint8_t velocity_theta_;
    uint8_t direction_x_;
    uint8_t direction_y_;
    uint8_t direction_theta_;
    uint8_t dribbler_;
    uint8_t kick_;
};

#endif // PROTOCOLOSERIAL_H
