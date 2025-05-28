#define PINO_RX 13
#define PINO_CTS 12
#define PINO_RTS 8
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

#include "Temporizador.h"

volatile int flag = 0;
volatile bool bitsRecebidos[9];

// Calcula bit de paridade - Par ou impar
bool bitParidade(char dado) {
  bool bitsDado[8];
  bool quantidadeDeUm = false;

  for (int i = 0; i < 8; i++) {
    bitsDado[i] = (dado >> i) & 1;
    if (bitsDado[i]) quantidadeDeUm = !quantidadeDeUm;
  }

  return quantidadeDeUm;
}

// Rotina de interrupcao do timer1
ISR(TIMER1_COMPA_vect) {
  Serial.print("Flag: ");
  Serial.println(flag);
  if (flag < 18) {
    if (flag % 2 == 1) {
      bool bit = digitalRead(PINO_RX);
      bitsRecebidos[flag / 2] = bit;
      Serial.print("Bit recebido: ");
      Serial.println(bit);
    }
    
    Serial.print("Flag Incrementada: ");
    Serial.println(flag);
  } else {
    Serial.println("Exit 1");
    paraTemporizador();
    digitalWrite(PINO_CTS, LOW);
    
  }
  flag++;
}

void setup() {
  noInterrupts();
  Serial.begin(9600);

  // Configura pinos
  pinMode(PINO_RX, INPUT);
  pinMode(PINO_CTS, OUTPUT);
  pinMode(PINO_RTS, INPUT);
  digitalWrite(PINO_CTS, LOW);

  // Configura timer
  configuraTemporizador(BAUD_RATE*2);

  interrupts();
}

void loop() {
  if (digitalRead(PINO_RTS) == HIGH) {
    Serial.println("RTS detectado, iniciando recepção");
    digitalWrite(PINO_CTS, HIGH);

    iniciaTemporizador();
    while (flag <= 19)
    {
      
    }
    
    flag = 0;

    // Reconstrói caractere
    char caractere = 0;
    for (int i = 0; i < 8; i++) {
      if (bitsRecebidos[i])
        caractere |= (1 << i);
    }

    Serial.print("Caractere recebido: ");
    Serial.println(caractere);

    bool paridadeEsperada = bitParidade(caractere);
    bool paridadeRecebida = bitsRecebidos[8];

    if (paridadeEsperada == paridadeRecebida) {
      Serial.println("Paridade correta!");
    } else {
      Serial.println("Erro de paridade!");
    }
    
  } else {
    digitalWrite(PINO_CTS, LOW);
  }

}