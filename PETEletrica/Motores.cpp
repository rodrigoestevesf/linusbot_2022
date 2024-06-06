#include <stdlib.h>
#include <PETEletrica.h>
#include <Arduino.h>
//============================PARA O LINUS NORMAL, PONTE H L298N=======================
Motores::Motores(int IN1, int IN2, int IN3, int IN4) {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    _IN1 = IN1;
    _IN2 = IN2;
    _IN3 = IN3;
    _IN4 = IN4;
}

void Motores::set_motors(int vel_esq, int vel_dir) {
    if(vel_dir >= 0 && vel_esq >= 0) {
        analogWrite(_IN1, 0);
        analogWrite(_IN2, vel_dir);
        analogWrite(_IN3, 0);
        analogWrite(_IN4, vel_esq);
    }
    if(vel_dir >= 0 && vel_esq < 0) {
        vel_esq = -vel_esq;
        analogWrite(_IN1, 0);
        analogWrite(_IN2, vel_dir);
        analogWrite(_IN3, vel_esq);
        analogWrite(_IN4, 0);
    }
    if(vel_dir < 0 && vel_esq >= 0) {
        vel_dir = -vel_dir;
        analogWrite(_IN1, vel_dir);
        analogWrite(_IN2, 0);
        analogWrite(_IN3, 0);
        analogWrite(_IN4, vel_esq);
    }
    if(vel_dir < 0 && vel_esq < 0) {
        vel_dir = -vel_dir;
        vel_esq = -vel_esq;
        analogWrite(_IN1, vel_dir);
        analogWrite(_IN2, 0);
        analogWrite(_IN3, vel_esq);
        analogWrite(_IN4, 0);
    }
}
//==================================== PARA O LINUS FIXO - PONTE H L293D ====================================
MotoresFixo::MotoresFixo(int ENE, int END, int IN3, int IN4, int IN1, int IN2) {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENE, OUTPUT);
    pinMode(END, OUTPUT);
    _IN1 = IN1;
    _IN2 = IN2;
    _IN3 = IN3;
    _IN4 = IN4;
    _ENE = ENE;
    _END = END;
}

void MotoresFixo::set_motorsFixo(int vel_esq, int vel_dir) {
    if(vel_dir >= 0 && vel_esq >= 0) {
        digitalWrite(_IN1, 0);//pra frente
        digitalWrite(_IN2, 1);
        analogWrite(_END, vel_dir);

        digitalWrite(_IN3, 0);//pra frente
        digitalWrite(_IN4, 1);
        analogWrite(_ENE, vel_esq);
    }
    if(vel_dir >= 0 && vel_esq < 0) {
        vel_esq = -vel_esq;
        digitalWrite(_IN1, 0);//pra frente
        digitalWrite(_IN2, 1);        
        analogWrite(_END, vel_dir);
        
        digitalWrite(_IN3, 1);//pra trás
        digitalWrite(_IN4, 0);        
        analogWrite(_ENE, vel_esq);
    }
    if(vel_dir < 0 && vel_esq >= 0) {
        vel_dir = -vel_dir;
        digitalWrite(_IN1, 1);//pra trás
        digitalWrite(_IN2, 0);        
        analogWrite(_END, vel_dir);
        
        digitalWrite(_IN3, 0);//pra frente
        digitalWrite(_IN4, 1);        
        analogWrite(_ENE, vel_esq);
    }
    if(vel_dir < 0 && vel_esq < 0) {
        vel_dir = -vel_dir;
        vel_esq = -vel_esq;
        digitalWrite(_IN1, 1);//pra frente
        digitalWrite(_IN2, 0);
        analogWrite(_END, vel_dir);

        digitalWrite(_IN3, 1);//pra frente
        digitalWrite(_IN4, 0);
        analogWrite(_ENE, vel_esq);
    }
}