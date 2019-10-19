#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

enum ArduinoCommand
{
  HIPC = 'a', ACK = 'b'
};
typedef enum ArduinoCommand ArduinoCommand;

#define CE 7 //define o pino 7 como CE
#define CSN 8 //define o pino 8 como CS
RF24 radio(CE, CSN); // Define quais serão os pinos do arduino usados para conectar o CE e o CS no rádio

uint8_t dados[5];
//dado = {id, velocidade linear, velocidade angular, direção linear, direção angular}

const uint64_t pipe[6] = {0xF1F1F1F1E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL, 0xF0F0F0F0E6LL};  // Endereços do radio

uint8_t c;
int cont;
int i;

byte package_buffer[5];
ArduinoCommand command = ACK;
byte command_[] = {(byte) command};
int serial_size;
char dump;

void setup() {
  Serial.begin(500000);
  Serial.write(command_, sizeof(byte));
  radio.begin();
  radio.setChannel(111);
  radio.openWritingPipe(pipe[0]);
}

void loop() {
  serial_size = Serial.available();
  if (serial_size >= 5) {
    Serial.readBytes(package_buffer, 5);
    for (int i = 0; i < serial_size - 5; ++i) dump = Serial.read();
    for (i = 0; i < 5; i++) dados[i] = (uint8_t)package_buffer[i];
    radio.write(&dados, sizeof(dados));
    Serial.write(command_, sizeof(byte));
  }
}
