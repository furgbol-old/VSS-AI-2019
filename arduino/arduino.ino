
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE 7 //define o pino 7 como CE
#define CSN 8 //define o pino 8 como CS

RF24 radio(CE, CSN); // Define quais serão os pinos do arduino usados para conectar o CE e o CS no rádio

uint8_t dados[5];
int i;

//dado = {id, velocidade linear, velocidade angular, direção linear, direção angular}

const uint64_t pipe[6] = {0xF1F1F1F1E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL, 0xF0F0F0F0E6LL};  // Endereços do radio
uint8_t c;

int cont;

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(111);
  radio.openWritingPipe(pipe[0]); // Começa a enviar msg para os robos

}

void loop() {
  if(Serial.available()) {
    c = Serial.read();
    
    if(c >= 128) {
      cont = 1;
      dados[0] = c - 128;
    } else if (cont > 0) {
      dados[cont] = c;
      cont++;
    }
    
    if (cont == sizeof(dados)) {
      cont = 0;

      Serial.println("--->TeamProtocolMessage{");
      Serial.print("Robot ID: ");
      Serial.println(dados[0]);
      Serial.print("Linear Velocity: ");
      Serial.println(dados[1]);
      Serial.print("Angular Velocity: ");
      Serial.println(dados[2]);
      Serial.print("Linear Direction: ");
      Serial.println(dados[3]);
      Serial.print("Angular Direction: ");
      Serial.println(dados[4]);
      Serial.println("}");
      
      radio.write(&dados, sizeof(dados));
    }
  }
}
