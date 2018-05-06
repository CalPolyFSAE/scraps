/*
 * Script for testing single BMS cell module.
 * Connect BMS ground to Arduino ground.
 * Connect BMS power to variable power supply.
 * Connect outpin to DOWNSTREAM module input.
 * Connect inpin to UPSTREAM module input.
 */

#define outpin 9 // downstream tx
#define inpin 8 // upstream rx


char in[80];


// Raw binary strings 
String message0 = "11100000 00000000 10101010 10110000 11011110 00101101 11010000 00011000 10101111"; // Start?
String message1 = "11100000 00000000 10101010 10110010 00011110 01101111 11000111 01111011 10011111"; // Some kind of balance instruction, pulls current around 4.1V
String message2 = "11100000 00000000 10101010 10110110 00011110 01000100 11011111 11111000 00000011"; // ???
String message3 = "11100000 00000000 10101010 10110100 00001110 01111101"; // Request temperature

// RX buffer  
int messagein[9] = {0};

// Set pin high-z to send 1
void one(){
  pinMode(outpin, INPUT);
}

// Set pin low to send 0
void zero(){
  digitalWrite(outpin, 0);
  pinMode(outpin, OUTPUT);
} 


void setup(void){
  Serial.begin(9600);
  Serial.readBytes(in, 80);
  pinMode(outpin, INPUT);
  pinMode(inpin, INPUT_PULLUP);
}


void loop(void){
  
  // Start message?
  delay(500);
  sendToBMS(message0);
  
  // ???
  delay(500);
  sendToBMS(message2);
  
  // Request temperature
  delay(500);
  sendToBMS(message3);
  
  // Wait until BMS 
  while(digitalRead(inpin));
  
  // Wait until end of start bits
  delayMicroseconds(100);
  
  // Read voltage from BMS; assumes 115us period
  for(char i = 0; i < 8; i++){
    delayMicroseconds(60);
    messagein[i] = 1 ? digitalRead(inpin) : 0;
    delayMicroseconds(55);
  }
  
  // Convert binary value to voltage 
  int value = 0;
  for(char i = 0; i < 8; i++)
    value += messagein[i] * pow(2, i);
  float voltage = value * 0.01 + 2.03;
  Serial.println(voltage);
}

// Send binary message from string to cell module
// BMS clock is defined here. 100us delay yields about 115us period.
void sendToBMS(String message){
  for(int i = 0; i < message.length(); i++){
    while(message[i] != '0' && message[i] != '1') i++;
    if(message[i] == '0') zero();
    else one();
    delayMicroseconds(100);
  }
}
