#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE 7
#define CHANNEL 111
#define CSN 8
#define ACK_MSG 255
#define PACKAGE_SIZE 5

RF24 radio(CE, CSN);
const uint64_t pipe[6] = {0xF1F1F1F1E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL, 0xF0F0F0F0E6LL};  // Endereços do radio
byte serial_buffer[PACKAGE_SIZE];
uint8_t radio_buffer[PACKAGE_SIZE];

int serial_available;
byte dump;

void setup()
{
  radio.begin();
  radio.setChannel(CHANNEL);
  radio.openWritingPipe(pipe[0]);
  Serial.begin(500000);
  Serial.write(ACK_MSG);
}

void loop() {
  serial_available = Serial.available();
  if (serial_available >= PACKAGE_SIZE) {
    Serial.readBytes(serial_buffer, PACKAGE_SIZE);
    for (int i = 0; i < serial_available - PACKAGE_SIZE; ++i) dump = Serial.read();
    for (int i = 0; i < PACKAGE_SIZE; i++) radio_buffer[i] = serial_buffer[i];
    radio.write(&radio_buffer, PACKAGE_SIZE);
    Serial.write(ACK_MSG);
  }
}

void read_serial_package() {
  Serial.readBytes(serial_buffer, PACKAGE_SIZE);
  dump_serial();
}

void dump_serial() {
  for (int i = 0; i < Serial.available(); ++i) Serial.read();
}

void send_radio_package() {
  serial_to_radio_buffer();
  radio.write(&radio_buffer, PACKAGE_SIZE);
  acknowledge_serial();
}

void serial_to_radio_buffer() {
  for (int i = 0; i < PACKAGE_SIZE; i++) radio_buffer[i] = serial_buffer[i];
}

void acknowledge_serial() {
  Serial.write(ACK_MSG);
}