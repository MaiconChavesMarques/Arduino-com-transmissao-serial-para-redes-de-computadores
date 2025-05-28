#define PINO_TX 13
#define PINO_CTS 12
#define PINO_RTS 8
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

#include "Temporizador.h"

volatile int flag = 0;
volatile bool bitsParaEnviar[9]; //Esse é o dado final

// Calcula bit de paridade - Par ou impar
bool bitParidade(char dado) {
  bool bitsDado[8];

  bool quantidadeDeUm = false;

  for(int i = 0; i < 8; i++) {
    bitsDado[i] = (dado >> i) & 1;

    if(bitsDado[i] == 1)
      quantidadeDeUm = !quantidadeDeUm;
  }
  
  return quantidadeDeUm;
}

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){
  //>>>> Codigo Aqui <<<<
  //Serial.println("interrompidouuu");
  //Serial.println(flag);
  if(flag < 18){
    if(flag % 2 == 0) {
      Serial.print("Bit enviado: ");
      Serial.println(bitsParaEnviar[flag/2]);
      if(bitsParaEnviar[flag/2] == 1){
        digitalWrite(PINO_TX, HIGH);
      }else{
        digitalWrite(PINO_TX, LOW);
      }
    }
    flag = flag + 1;
  }else{
    flag = flag + 1;
    paraTemporizador();
    digitalWrite(PINO_RTS, LOW);
  }
}

// Executada uma vez quando o Arduino reseta
void setup(){
  //desabilita interrupcoes
  noInterrupts();
  // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
  Serial.begin(9600);
  // Inicializa TX ou RX
  //>>>> Codigo Aqui <<<<
  pinMode(PINO_TX, OUTPUT);
  pinMode(PINO_CTS, INPUT);
  pinMode(PINO_RTS, OUTPUT);
  digitalWrite(PINO_RTS, LOW);
  digitalWrite(PINO_TX, LOW);
  // Configura timer
  //>>>> Codigo Aqui <<<<
  configuraTemporizador(BAUD_RATE*2);
  // habilita interrupcoes
  interrupts();
}

// O loop() eh executado continuamente (como um while(true))
void loop ( ) {
  //>>>> Codigo Aqui <<<<
  if (Serial.available() > 0) {
    char caractere = Serial.read(); // Lê 1 caractere
    Serial.print("Recebi: ");
    Serial.println(caractere);

    bool dados[8];
    for(int i = 0; i < 8; i++) {
      dados[i] = (caractere >> i) & 1;
    }

    for (int i = 0; i < 8; i++) {
      bitsParaEnviar[i] = dados[i];
    }

    bitsParaEnviar[8] = bitParidade(caractere);
    for(int i = 0; i < 9; i++){
      Serial.print(bitsParaEnviar[i]);
    }

    digitalWrite(PINO_RTS, HIGH);
    while (!digitalRead(PINO_CTS))
    {}
    iniciaTemporizador();
    while (flag <= 18 && digitalRead(PINO_CTS) == HIGH)
    {}
    flag = 0;
  }
}
