#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <LiquidCrystal_I2C.h>

RF24 radio(8, 7);
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t address[][6] = {"1Node", "2Node"};

float payload = 0.0;
unsigned int sentPackages = 0;
unsigned int lostPackages = 0;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(4, 0);
  lcd.print("Starting");

  lcd.setCursor(2, 1);
  lcd.print("nRF24 Tester");
  delay(2000);

  if (!radio.begin()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("radio not responding!!");
    while (1) {}  // hold in infinite loop
  }

  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes

  //Remember to switch between the nodes!!
  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);  // always uses pipe 0
 
  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.stopListening(); //set radio on TX mode

  //making a first transmission, to confrim a connection.
  bool report = false;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting..");
  unsigned long start_timer = millis();
  while(!report && millis() - start_timer < 15000){ //15 Second timeout
    report = radio.write(&payload, sizeof(float));
  }
  if(!report){ //if no receiver, received a transmission, then print timeout, and end the program.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("15Sec timeout");
    delay(1500);
    return;
  }

  //initialize lcd home screen
  lcd.clear();
  printPackageLost(0);
  printAck(0);
  printLatancy(0);
}

void loop() {
  //starting a timer
  unsigned long start_timer = millis();                
  bool report = radio.write(&payload, sizeof(float)); //Transmit data & save status of the transmition
  payload += 0.01;
  unsigned long end_timer = millis();
  sentPackages++;
  if(report){ //if the transmition succeed.
    printAck(sentPackages-lostPackages); //Acknowledge packets
    printPackageLost(lostPackages); //Lost packets
    printLatancy(end_timer-start_timer); //latancy(How much to transmit the data, and get a confirmation)
  }else { //if the transmition faild
    lostPackages++; //increment the lost packages counter
    printAck(sentPackages-lostPackages); //update Acknowledges packets
    printPackageLost(lostPackages); //update Lost packets
  }
  delay(1500);
}

void printPackageLost(int b){ 
  lcd.setCursor(0, 0);
  lcd.print("Lost:");
  lcd.print(b);
}

void printAck(int b){
  lcd.setCursor(9, 0);
  lcd.print("ACK:");
  lcd.print(b);
}

void printLatancy(long b){
  lcd.setCursor(0, 1);
  lcd.print("Latancy:");
  lcd.print(b);
  lcd.print("ms  ");
}
