/*
AUTOR: RODRIGO ESTEVES FERREIRA
EMAIL: rodrigo.ferreira2022@engenharia.ufjf.br

Esse código foi o utilizado para a competição de robôs seguidores de linha 'Linus Bot', promovido pelo PET Engenharia Elétrica da UFJF
Com o mesmo fui vencedor da edição de 2022.1 em tempo record. O robô segue uma linha branca e tem que desviar de obstáculos ao longo
da pista, sendo esses sinalizados por uma faixa perpendiculara linha que o mesmo tem que seguir. Eles utilizam o seguintes componentes:

Arduino Uno
Dois motores DC (3-6)V
Ponte H L298N
Sensor ultrasônico HC-SR04
Array de sensores opticos Tcrt5000
*/


#include <PETEletrica.h> 

//========================Declarando variaveis========================

//---------Variaveis constantes

//Parametros do carro
const int vel_ref = 145,        //Velocidade referencia do carro (0 a 255)
          tag_ref = 650,        //Referencia da media dos sensores quando passa pela tag
          dist_ref = 20;         //Distancia de referencia para o carro desviar (cm)
          

const float sensors_ref = 2000,   //Referencia da posicao do sensor quando a linha esta no meio
            kp = 0.055,            //Parametro do controle proporcional
            ki = 0,
            kd = 0;
          
//Pinos conectados a ponte H
const int IN1=3,
          IN2=5,
          IN3=6,
          IN4=9;

//Pinos conectados ao sensor ultrassonico
const int pino_trigger = 12, 
          pino_echo = 13;

//Pinos conectados ao sensor infravermelho
const unsigned char s_pin[] = {A0, A1, A2, A3, A4}; //Analogo a {14, 15, 16, 17, 18} (ver pinout)
                                          //           A0  A1  A2  A3  A4


//---------Variaveis auxiliares

double mediaInfra,    //Armazena a media dos sensores para detectar a tag
      dist,          //Armazena a distancia lida pelo sensor ultrassonico (cm)
      erro,           //Armazena o valor da diferença entre a posicao atual e a referencia d linha no meio
      lastError=0,       //Armazena o valor anterior do erro
      ajuste;             //Armazena o valor que fara o controle de angulacao do carro de acordo com o kp e o erro acima

double P=0, I=0, D=0;

unsigned int posicao,    //Armazena a posicao do carro referente a linha
             sensores[5]; //Armazena o valor dos sensores para calcular a posicao


int s[5]; //Array que recebe os valores lidos por cada sensor

bool tag = 0; //Armazena se ele leu ou nao a tag

long microsec; //Variavel utilizada pelo ultrassonico para ver o tempo de propagacao

//========================Criando objetos========================

SensorUltrassonico ultra(pino_trigger, pino_echo); //Cria objeto do sensor ultrassonico

SensorInfravermelhoRC infra(s_pin, 5, 2000, QTR_NO_EMITTER_PIN); //Cria objeto do sensor infravermelho

Motores motor(IN1, IN2, IN3, IN4); //Cria objeto dos motores (ponte H)


//========================Funcoes auxiliares========================

//Calibra o sensor infravermelho
void calibrar()
{
  //Calibra por 1s (10ms*100)
  for(int i = 0; i < 100; i++)
  {
    infra.calibrate();
    delay(10);
  }
}

//Atualiza a media aritimetica dos sensores
void atualizaMedInfra()
{ 
  //A media aritimetica dos sensores sera equivalente a largura da linha em que ele esta, ou nao, percorrendo.
  //Podendo visualizar linha e tag pelo valor

  for(int i=0; i<5; i++)  s[i] = analogRead(s_pin[i]);
  mediaInfra = ((s[0] + s[1] + s[2] + s[3] + s[4]) / 5);
}

//Desvia do obstaculo
void desvia()
{
  //Nessa configuracao, o carro contorna o obstaculo em um movimento circular, ajeita a angulacao, fechando-a,
  //e segue reto ate encontrar a linha, e volta a segui-la

  //ALTERACOES NOS VALORES SAO RECOMENDADOS DEVIDO A VARIACAO DE BATERIA, MOTOR, PONTE H...

  motor.set_motors(170, 0); //Abertura para contornar
  delay(600);
  motor.set_motors(75, 190); //Contorna
  delay(800);
  motor.set_motors(0, 170); //Fecha o angulo para encontrar a linha
  delay(400);
  motor.set_motors(170, 170); //Segue reto

  atualizaMedInfra(); //atualiza a media dos sensores para while a seguir
  while(mediaInfra <= 200)  atualizaMedInfra(); //Segue reto ate encontrar a linha
}

//Atualiza as variaveis do sensor ultrassonico
void atualizaUltra()
{  
  microsec = ultra.timing(); //Calcula tempo de propagacao do sinal
  dist = ultra.convert(microsec, SensorUltrassonico::CM); //Calcula com base no tempo, a distancia em CM
}

//Atualiza as variaveis de posicao e as dependentes dela, ligadas ao controle proporcional
void atualizaControle()
{
  posicao = infra.readLine(sensores); //Ve a posicao relativa a linha
  erro = sensors_ref - posicao; //Ve o erro de acordo com a referencia

  P = erro*kp;

  I += erro*ki; 

  D = (erro - lastError)*kd;
  lastError = erro;

  ajuste = P + I + D; //Calcula a diferença de velocidade a ser aplicada nos motores para ajeitar a angulacao
}

//========================Funcoes principais========================

void setup() 
{  
  calibrar();
}


void loop() 
{
  atualizaControle();  
  motor.set_motors(vel_ref - ajuste, vel_ref + ajuste); //Ajeita a angulacao apartir do controle
  atualizaMedInfra();

  //Se visualizar a tag registra na variavel
  if (mediaInfra >= tag_ref) tag=1; 

  //Se tiver visto a tag, comeca a ver a ver a distancia do objeto, e assim que estiver
  //na distancia estabelecida, ele desvia
  if(tag) 
  {
      atualizaUltra();

      if(dist <= dist_ref)
      {
        desvia();
        tag = 0; //Desviando, nao ha mais obstaculo, ate que seja indicado por uma nova tag
      }
  }
}
