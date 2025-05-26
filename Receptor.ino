#define PINO_RX 2
#define PINO_TX 3
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

#include "Temporizador.h"

volatile int flag = 0;
volatile bool bitsRecebidos[9]; // 8 bits + 1 bit de paridade

// Calcula bit de paridade (par ou ímpar)
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

// Rotina de interrupção do timer
ISR(TIMER1_COMPA_vect){
  if(flag < 18){
    if(flag % 2 == 1) {
      bitsRecebidos[flag / 2] = digitalRead(PINO_RX);
    }
    flag++;
  } else {
    paraTemporizador();
  }
}

void setup(){
  noInterrupts();
  Serial.begin(9600);
  pinMode(PINO_TX, OUTPUT);
  pinMode(PINO_RX, INPUT);
  digitalWrite(PINO_TX, HIGH); // linha ociosa em HIGH
  configuraTemporizador(2); // interrupções no dobro do baud rate
  interrupts();
}

void loop(){
  // Espera pelo bit de start (nível LOW)
  if (digitalRead(PINO_RX) == LOW){
    flag = 0;
    iniciaTemporizador();
    while(flag < 18) {} // espera a recepção completa

    // Monta o caractere
    char caractere = 0;
    for(int i = 0; i < 8; i++){
      caractere |= (bitsRecebidos[i] << i);
    }

    // Verifica paridade
    bool paridadeCalculada = bitParidade(caractere);
    bool paridadeRecebida = bitsRecebidos[8];

    if(paridadeCalculada == paridadeRecebida){
      Serial.print("Recebido: ");
      Serial.println(caractere);
    } else {
      Serial.println("Erro de paridade!");
    }
  }
}
