#include <GroveSBT.h>
#include <SoftwareSerial.h>

void f_init() {Serial.println("BT initialized");}
void f_ready() {
  Serial.println("BT ready");
  groveSBT_inq();
}
void f_inq() {Serial.println("BT inquiring");}
void f_cncting() {Serial.println("BT connecting");}
void f_cncted() {Serial.println("BT connected");}


void setup() {
  Serial.begin(9600);
  
  groveSBT_init();
  
  pinMode(13, OUTPUT);
  digitalWrite(13,1);
  
  groveSBT_onReady = f_ready; 
  groveSBT_onInit = f_init;
  groveSBT_onInquiring = f_inq;
  groveSBT_onConnected = f_cncted;
  groveSBT_onConnecting = f_cncting;

//  groveSBT_inq();
  
}

void loop() {
  
  groveSBT_loop();

  if(groveSBT_available())
    Serial.print(groveSBT_read());
  
  if(Serial.available()) Serial.write(Serial.read());

}
