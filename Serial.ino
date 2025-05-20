#define PINO_RX 0
#define PINO_TX 1
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

#include "Temporizador.h"
#include <bits/stdc++.h>
#include <bitset>
using namespace std;

volatile int flag = 0;
volatile bool bitsParaEnviar[9]; //Esse é o dado final

// Calcula bit de paridade - Par ou impar
bool bitParidade(char dado) {
  bitset<8> dadoBinario(dado);

  bool quantidadeDeUm = false; // Para o bit de paridade par

  for(int i = 0; i < dadoBinario.size(); i++) {
      if(dadoBinario[i] == 1)
          quantidadeDeUm = !quantidadeDeUm;
  }
  
  return quantidadeDeUm;
}

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){
  //>>>> Codigo Aqui <<<<
  if(flag < 9){
    if(bitsParaEnviar[flag] == 1){
      digitalWrite(PINO_TX, HIGH);
    }else{
      digitalWrite(PINO_TX, LOW);
    }
    flag = flag + 1;
  }else{
    paraTemporizador();
  }

  //Passou 1 segundo eu suponho que preciso enviar meu bit
  //Envio o proximo bit da lista, se tiver bit ainda
  //bits enviados = bits enviados + 1
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
  pinMode(PINO_RX, INPUT);
  digitalWrite(PINO_TX, LOW);
  // Configura timer
  //>>>> Codigo Aqui <<<<
  configuraTemporizador(1);
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
    bitset<8> dados(caractere);
    for (int i = 0; i < 8; i++) {
      bitsParaEnviar[i] = dados[i];
    }
    bitsParaEnviar[8] = bitParidade(caractere);
    for(int i = 0; i < 9; i++){
      Serial.print(bitsParaEnviar[i]);
    }
    digitalWrite(PINO_TX, HIGH);
    while (!digitalRead(PINO_RX))
    {}
    iniciaTemporizador();
    while (flag < 9)
    {}
    digitalWrite(PINO_TX, LOW);
    flag = 0;
  }
  //Se veio
  //preparo o dado
  //coloco tx como 1
  //enquanto rx for igual a 0 fico preso
  //inicia Temporizador()
  //if(quantidade de bits transmitido for 8)
  //paraTemporizador
}
