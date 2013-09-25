/**
 * GroveSBT controler example
 *
 * Connects with a bluetooth device and acts as a bridge, sending everithing
 * received via Serial port through BT, and vice versa.
 *
 * Hardware Setup:
 * -Grove's Tx pin must be connected to Arduino's pin 10
 * -Grove's Rx pin must be connected to Arduino's pin 11
 *
 * @author MH Serans
 */
#include <SoftwareSerial.h>
#include <GroveSBT.h>

/**
 * Callbacks
 */
void f_init() { Serial.println("BT initialized"); }
void f_inq() {Serial.println("BT inquiring");}
void f_cncting() {Serial.println("BT connecting");}
void f_cncted() {Serial.println("BT connected");}

/* groveSBT_onReady callback is useful for making the device inquirable, because
 * it gets called on startup, and every time the device is disconnected
 */
void f_ready() { 
  Serial.println("BT ready"); 
  groveSBT_inq(); 
}

/* groveSBT_onNewLine callback is useful for line-based analysis of the input.
 *
 * In this example, we just send every line received via BT through the HW 
 * Serial, unless "VERSION" is received, in which case we send the current 
 * software version back via BT 
 */
void f_newline() {
  if( groveSBT_buffer_equals_string("VERSION\n")
    ) {
    groveSBT_write("GroveSBT interface version 0.1\n");
    groveSBT_init(); // removes all data from buffer
  } else {
    while(groveSBT_available()) {
      Serial.print(groveSBT_read());
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  groveSBT_init();
  
  groveSBT_onReady = f_ready; 
  groveSBT_onInit = f_init;
  groveSBT_onInquiring = f_inq;
  groveSBT_onConnected = f_cncted;
  groveSBT_onConnecting = f_cncting;
  groveSBT_onNewLine = f_newline;
}

void loop() {
  
  groveSBT_loop();

  if(Serial.available())
    groveSBT_write(Serial.read());

}
