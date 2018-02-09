#include <IRremote.h>
/*
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <boarddefs.h>
*/


//store inputes from Serial monitor
char input;

//store signal values
const int commands=1;
long irValues[commands];
char irKeys[commands];
//set-up mode or sender mode
bool isAssociated=false;
//in order to send later
int signalType;

//IRLED
const int IRled=3;
//receiver on pin 3
const int receiveSignalIR=5;
//instantiate IR manager object
IRrecv irrecv(receiveSignalIR);
//create variable to store results
decode_results decodedSignal;
IRsend irsend;

char inserter(String sentence){
  Serial.println(sentence);
  Serial.print("Enter: ");
  //wait input from the serial monitor
  while(Serial.available() == 0) { } 
  input=Serial.read();
  Serial.println();
  return input;
}

void setup() {
  Serial.begin(9600);
    //start read signals
  irrecv.enableIRIn();
}

void registerSignal(){
  int counter=0;
  while(counter<commands){
    char remoteKey;
    //check if the key has been already inserted
    bool checkRepetition=true;
    while (checkRepetition){
      checkRepetition=false;
      remoteKey=inserter("Insert the remote control key");
      for(int i=0; i<sizeof(irKeys); i++){
        if(remoteKey==irKeys[i]){
          Serial.print("Key already inserted");
          checkRepetition=true;
        }
      }
    }
    Serial.println("Now press the button on the remote 1 time");
    while(irrecv.decode(&decodedSignal) == 0) { } 
    if (irrecv.decode(&decodedSignal)==true){
      signalType=decodedSignal.decode_type;
      Serial.println("Check if you can press more than one button at time");
      long value=decodedSignal.value;
      irValues[counter]=value;
      irKeys[counter]=remoteKey;
      Serial.print("Key ");
      Serial.print(remoteKey);
      Serial.println(" Saved!!");
      counter +=1;
      delay(1000);
      irrecv.resume();
    }
  }
  Serial.println("Association finished, now you can control the device associated with the remote control inserted");
  isAssociated=true;
}

void sendSignal(){
  for (int i=0; i<commands; i++){
    Serial.print(irKeys[i]);
    Serial.print(", ");
  }
  Serial.println();
  bool found=false;
  long valueRetrieved;
  while (!found){
    input=inserter("Select key to send");
    for (int i=0; i<commands; i++){
      if(irKeys[i]==input){
        valueRetrieved=irValues[i];
        found=true;
      }
    }
    if (!found){
      Serial.println("Command not registered, the button must be in the list");
    }
  }
  Serial.print("Signal type: ");
  Serial.println(signalType);
  switch(signalType)
  { 
    case -1:
    Serial.println("UNKNOWN Signal, please re-associate the commands");
    isAssociated=false;
    break;
    case 3:
    irsend.sendNEC(valueRetrieved, 20);
    break;
    case 4:
    irsend.sendSony(valueRetrieved, 20);
    break;
    case 5:
    irsend.sendPanasonic(0x4004, valueRetrieved);
    break;
    case 6:
    irsend.sendJVC(valueRetrieved, 20, 0);
    break;
    case 7:
    irsend.sendSAMSUNG(valueRetrieved, 20);
    break;
    case 10:
    irsend.sendLG(valueRetrieved, 20);
    break;
    
  }
  irrecv.enableIRIn();
  irrecv.resume();
  while(irrecv.decode(&decodedSignal)==0){ }
  if (irrecv.decode(&decodedSignal)==true){
    for (int i=0; i<commands; i++){
      if(irValues[i]==decodedSignal.value){
        Serial.print("Command");
        Serial.print(irKeys[i]);
        Serial.println("pressed!!");
      }
    }
  }
  delay(1000);
}
void loop() {
  if (!isAssociated){
    Serial.println("Starting with associate the remote controller");
    registerSignal();
  }
  for (int i=0; i<commands; i++){
    Serial.print(irKeys[i]);
    Serial.print(':');
    Serial.println(irValues[i]);
  }
  sendSignal();
}
