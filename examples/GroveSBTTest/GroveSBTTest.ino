#include <GroveSBT.h>

void f_ready() {Serial.println("BT ready");}
void f_init() {Serial.println("BT initialized");}
void f_inq() {Serial.println("BT inquiring");}
void f_cncting() {Serial.println("BT connecting");}
void f_cncted() {Serial.println("BT connected");}


void setup() {
  Serial.begin(9600);
  Serial.println("BEGIN");
  
  groveSBT_onReady = f_ready; 
  groveSBT_onInit = f_init;
  groveSBT_onInquiring = f_inq;
  groveSBT_onConnected = f_cncted;
  groveSBT_onConnecting = f_cncting;
}

void loop() {
  
  groveSBT_loop();

  if(groveSBT_available())
    Serial.print(groveSBT_read());

}
