#include "Radio.h"
#include <ControlMessage.h>
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>

#define DOUBLE_SIZE 4
#define N_BYTES sizeof(int16_t)
#define CE_PIN   9
#define CSN_PIN 10


Radio::Radio(bool is_sender) : radio{ CE_PIN, CSN_PIN }, is_sender{ is_sender } {}

void Radio::begin() {
    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    if (is_sender) {
        radio.stopListening();
        radio.setRetries(3, 5);
        radio.openWritingPipe(self_address);
    }
    else {
        radio.openReadingPipe(1, self_address);
        radio.startListening();
    }
}

void copyArray(const char source[], char dest[], const int message_size) {
    for (uint8_t i = 0; i < message_size; i++) {
        dest[i] = source[i];
    }
}

bool compareMessages(const char message1[], const char message2[], const int message_size) {
    for (uint8_t i = 0; i < message_size; i++) {
        if (message1[i] != message2[i]) {
            
            return true;
        }
     }
    return false;
}

bool Radio::hasMessage(){
  if ( radio.available() ) {
    copyArray(data_recieved, previous_data, MESSAGE_SIZE);
    radio.read( &data_recieved, sizeof(data_recieved) );
    return compareMessages(data_recieved, previous_data, MESSAGE_SIZE);
  }
  return false;
}

bool Radio::hasMessage(const char data_expected[MESSAGE_SIZE]) {
    if (hasMessage()) {
        if (strcmp(data_recieved, data_expected) == 0) {
            return true;
        }
        else {
#ifdef DEBUG
            Serial.print("Got an unexpected message: ");
            Serial.println(data_recieved);
#endif
        }
    }
    return false;
}

void fillCharArrayWithInt(char* arr, int16_t val) {
    arr[0] = (val >> 8) & 0xFF;
    arr[1] = val & 0xFF;
}

int16_t charArrayToInt(char* bytes) {
    int16_t output = 0;
    for(uint8_t i = 0; i < N_BYTES; i++){
        output = output << 8 | bytes[i];
    }
    return output;
}

struct Command Radio::getNextMessage(){
    char current_index = 0;

    int type = data_recieved[current_index];
    current_index += 1;

    int16_t x, y, z;
    x = charArrayToInt(&(data_recieved[current_index]));
    current_index += N_BYTES;

    y = charArrayToInt(&(data_recieved[current_index]));
    current_index += N_BYTES;

    z = charArrayToInt(&(data_recieved[current_index]));
    current_index += N_BYTES;

    uint8_t face = data_recieved[current_index];
    current_index += 1;

    uint8_t direction = data_recieved[current_index];

    struct Point point { x, y, z };
    struct Command message { type, point, face, direction};
    return message;
}

bool Radio::sendMessage(const char* message) {

    bool rslt;
    rslt = radio.write(&message, sizeof(message));
#ifdef DEBUG
    Serial.print("Data Sent ");
    if (rslt) {
        Serial.println("  Acknowledge received");
    }
    else {
        Serial.println("  Tx failed");
    }
#endif
    return rslt;

}

bool Radio::sendMessage(const struct Command& message) {
     copyToSendBuffer(message);
    Serial.println("copied to buffer");
     if (!radio.write(&data_to_send, sizeof(data_to_send))) {
         Serial.println("failed");
         return false;
     }
     Serial.println("succeeded");
     return true;
 }


 void Radio::copyToSendBuffer(const struct Command& message) {
     uint8_t current_index = 0;
     uint8_t n_bytes = 2;

     data_to_send[current_index] = message.type;
     current_index += 1;

     fillCharArrayWithInt(&(data_to_send[current_index]), message.pos.x);
     current_index += N_BYTES;

     fillCharArrayWithInt(&(data_to_send[current_index]), message.pos.y);
     current_index += N_BYTES;

     fillCharArrayWithInt(&(data_to_send[current_index]), message.pos.z);
     current_index += N_BYTES;
     
     data_to_send[current_index] = message.face;
     current_index += 1;

     data_to_send[current_index] = message.direction;
 }


