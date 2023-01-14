#include <SPI.h>
#include "printf.h"
#include "RF24.h"

RF24 radio(8, 7);

uint8_t address[][6] = {"1Node", "2Node"};
float payload = 0.0;
void setup(){
  Serial.begin(9600); //start Serial communication at 9600 buad
  if (!radio.begin()) { //Start and Check if the radio connected to the arduino
    Serial.print("radio not responding!!");
    while (1) {}  // hold in infinite loop //need a restart
  }

  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setDataRate(RF24_250KBPS); // RF24_2MBPS is default.
  radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes

  //Remember to switch between the nodes!!
  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[1]);  // always uses pipe 0
 
  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[0]);  // using pipe 1

  radio.startListening(); //set radio on RX mode  

}

void loop(){
  uint8_t pipe;
  if (radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
    radio.read(&payload, bytes);             // fetch payload from FIFO
    Serial.print(F("Received "));
    Serial.print(bytes);  // print the size of the payload
    Serial.print(F(" bytes on pipe "));
    Serial.print(pipe);  // print the pipe number
    Serial.print(F(": "));
    Serial.println(payload);  // print the payload's value
  }
}