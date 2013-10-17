/** 
 * @brief Bluetooth Grove 0.9 Interface for Arduino
 *
 * @see http://www.seeedstudio.com/wiki/Grove_-_Serial_Bluetooth
 * @author M.H.Serans
 */
#ifndef GROVESBT_H
#define GROVESBT_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include "CircularBuffer.h"

#ifndef CR
 #define CR 0x0D
#endif

#ifndef LF 
 #define LF 0x0A
#endif

/* COMPILE OPTIONS */
// #define DEBUG(x) Serial.println(x) /* debug on */
#define DEBUG(X) /* debug off */

/* Grove Serial Bluetooth Status */
#define BT_INIT         0
#define BT_READY        1
#define BT_INQUIRING    2
#define BT_CONNECTING   3
#define BT_CONNECTED    4
static byte bt_status = BT_INIT;

/* Input status */
#define BT_INPUT_INIT     0
#define BT_INPUT_LF       1
#define BT_INPUT_CR       2
#define BT_INPUT_COMMAND  3
static byte bt_input_status = BT_INPUT_INIT;

/* Command table
 * used for interpretating Grove's chip notifications */
#define COMMAND_INDEX_OK      0
#define COMMAND_INDEX_BTSTATE 1
#define COMMAND_INDEX_CONNECT 2
#define COMMAND_TABLE_LEN     3
static const char command_table[3][10] = { "OK", "BTSTATE:", "CONNECT:" };

/* Buffers */

/* c_serial_buffer
 * stores all de data received via bluetooth that is destinated to the end user.
 * (that is, excluding all configuration and status notifications from the chip)
*/
static circularBuffer c_serial_buffer;

/* command_buffer 
 * stores Grove's cips notifications are stored. Having a separate buffer makes 
 * management and interpretation easier.
 * @see command_table
 */
#define COMMAND_BUFFER_LEN 10
static struct  {
    char data[COMMAND_BUFFER_LEN];
    byte tail;
} command_buffer;

/* Serial Port Setup */
// alias for serial functions, for easy switching between SW & HW serial implementation
#ifdef GBT_SW_SERIAL
    #define SERIAL_OUT(x) ss.print(x)
    #define SERIAL_IN     ss.read();
    static SoftwareSerial ss = SoftwareSerial(10,11);
#else
    #define SERIAL_OUT(x) Serial.print(x)
    #define SERIAL_IN     Serial.read();
#endif

static boolean checkOK(char c);
static void readChar(char c);
void groveSBT_loop();
void groveSBT_send(char* text);
static void interpretateCommand(char *command);
boolean groveSBT_available();
char groveSBT_read();
void groveSBT_write();
void groveSBT_sendCommand();
inline byte groveSBT_status() { return bt_status; }
void groveSBT_inq();

/* callbacks */
static void dummy() {}

void (*groveSBT_onInit)()       = dummy;
void (*groveSBT_onReady)()      = dummy;
void (*groveSBT_onInquiring)()  = dummy;
void (*groveSBT_onConnected)()  = dummy;
void (*groveSBT_onConnecting)() = dummy;
void (*groveSBT_onNewLine)()    = dummy;

/* function to be called by the main program's loop */
char c;
void groveSBT_loop() {
    // @to-do using SoftwareSerial.available() seems to crash sometimes
    c = SERIAL_IN
    
    if(c>0) {
        /* 
        Serial.print("[[");
        Serial.print(c);
        Serial.print("]]");
        /* */
        switch(bt_input_status) {
            case BT_INPUT_INIT:
                if(c == CR) 
                    bt_input_status = BT_INPUT_CR;
                break;

            case BT_INPUT_CR:
                if(c == LF) 
                    bt_input_status = BT_INPUT_LF;
                else if(c != CR) 
                    bt_input_status = BT_INPUT_INIT;
                break;

            case BT_INPUT_LF:
                if(c == CR) {
                    bt_input_status = BT_INPUT_CR;
                } else if(c == '+' || bt_status != BT_CONNECTED) {
                    bt_input_status = BT_INPUT_COMMAND;
                    command_buffer.tail=0;
                    command_buffer.data[0]='\0';
                } else {
                    bt_input_status = BT_INPUT_INIT;
                }
                break;
            
            case BT_INPUT_COMMAND:
                if(c == CR) {
                    interpretateCommand(command_buffer.data);
                    bt_input_status = BT_INPUT_INIT;
                } else {
                    if(command_buffer.tail < COMMAND_BUFFER_LEN-1) {
                        command_buffer.data[command_buffer.tail] = c;
                        command_buffer.data[command_buffer.tail+1] = '\0';
                        command_buffer.tail++;
                    }
                }
                break;
        }
        
        if(bt_input_status != BT_INPUT_COMMAND && bt_status == BT_CONNECTED) {
            c_buffer_push(c, &c_serial_buffer);
            if(c == '\n') groveSBT_onNewLine();
        }
        
    }
}

/**
 * Commands:        Actions
 *  OK              -none-
 *  BTSTATE:X       change bt_status
 *  CONNECT:OK      -none-
 */
void interpretateCommand(char *command){

    byte i;
    byte j;
    
    for(i=0; i<COMMAND_TABLE_LEN; i++) {
        for(j=0; command_table[i][j] == command[j]; j++) {
            if(command_table[i][j+1]=='\0') {
                j++;
                if(i == COMMAND_INDEX_BTSTATE) {
                    bt_status = command[j]-'0';
                    
                    if(bt_status>4) {
		                bt_status=1;
		                return;
	                }

                    if(bt_status == BT_INIT)             groveSBT_onInit();
                    else if (bt_status == BT_READY)      groveSBT_onReady();
                    else if (bt_status == BT_INQUIRING)  groveSBT_onInquiring();
                    else if (bt_status == BT_CONNECTING) groveSBT_onConnecting();
                    else if (bt_status == BT_CONNECTED)  groveSBT_onConnected();
                    
                    return;
                }
            }
        }
    }
    

}

/* returns the number of bytes available to be read */
byte groveSBT_available() {
   return (c_buffer_len(&c_serial_buffer));
}

char groveSBT_read() {
    char c=-1;
    c_buffer_pull(&c, &c_serial_buffer);
    return c;
}

void groveSBT_write(int txt) {
    if(bt_status == BT_CONNECTED) SERIAL_OUT(txt);
}

void groveSBT_write(char *txt) {
    if(bt_status == BT_CONNECTED) SERIAL_OUT(txt);
}

void groveSBT_init() {
    #ifdef GBT_SW_SERIAL
    ss.begin(9600);
    #else
    Serial.begin(9600);
    #endif
    
    c_buffer_init(&c_serial_buffer);
}

void groveSBT_inq(){
    SERIAL_OUT("\r\n+INQ=1\r\n");
}

boolean groveSBT_buffer_equals_string(char *txt) {
    return c_buffer_equals_string(&c_serial_buffer,txt);
}
#endif
