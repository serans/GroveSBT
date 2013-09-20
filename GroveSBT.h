/** 
 * @brief Bluetooth Grove 0.9 Interface for Arduino
 *
 * @see http://www.seeedstudio.com/wiki/Grove_-_Serial_Bluetooth
 * @author MH Serans
 */
#ifndef GROVESBT_H
#define GROVESBT_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include "CircularBuffer.h"

#define CR 0x0D
#define LF 0x0A

#define BT_INIT         0
#define BT_READY        1
#define BT_INQUIRING    2
#define BT_CONNECTING   3
#define BT_CONNECTED    4

#define BT_INPUT_INIT     0
#define BT_INPUT_LF       1
#define BT_INPUT_CR       2
#define BT_INPUT_COMMAND  3

#define COMMAND_BUFFER_LEN 10

#define COMMAND_INDEX_OK      0
#define COMMAND_INDEX_BTSTATE 1
#define COMMAND_INDEX_CONNECT 2

#define COMMAND_TABLE_LEN     3

//#define DEBUG(x) Serial.println(x)
#define DEBUG(X) /* debug off */


//use software serial
static SoftwareSerial ss = SoftwareSerial(10,11);
#define SERIAL_OUT(x) ss.write(x)
#define SERIAL_IN     ss.read();

static const char command_table[3][10] = { "OK", "BTSTATE:", "CONNECT:" };

static byte bt_status = BT_INIT;
static byte bt_input_status = BT_INPUT_INIT;

static circularBuffer buffer;

static struct  {
    char data[COMMAND_BUFFER_LEN];
    byte tail;
} commandBuffer;

static boolean checkOK(char c);
static void readChar(char c);

//functions
void groveSBT_loop();
void groveSBT_send(char* text);
static void interpretateCommand(char *command);
boolean groveSBT_available();
char groveSBT_read();
void groveSBT_print();
void groveSBT_sendCommand();
inline byte groveSBT_status() { return bt_status; }
void groveSBT_inq();

//callbacks
static void dummy() {}

void (*groveSBT_onInit)()       = dummy;
void (*groveSBT_onReady)()      = dummy;
void (*groveSBT_onInquiring)()  = dummy;
void (*groveSBT_onConnected)()  = dummy;
void (*groveSBT_onConnecting)() = dummy;


//function to be called by the main program's loop
char c;
void groveSBT_loop() {
    c = SERIAL_IN
    if(c>0) {
        switch(bt_input_status) {
            case BT_INPUT_INIT:
                if(c == CR) {bt_input_status = BT_INPUT_CR; DEBUG("CR");}
                if(bt_status == BT_CONNECTED) c_buffer_push(c, &buffer);
                break;

            case BT_INPUT_CR:
                if(c == LF) {bt_input_status = BT_INPUT_LF; DEBUG("LF");}
                else if(c != CR) bt_input_status = BT_INPUT_INIT;
                if(bt_status == BT_CONNECTED) c_buffer_push(c, &buffer);
                break;

            case BT_INPUT_LF:
                if(c == CR) bt_input_status = BT_INPUT_CR;
                else if(c == '+' || bt_status != BT_CONNECTED) {
                    
                    bt_input_status = BT_INPUT_COMMAND;
                    commandBuffer.tail=0;
                    commandBuffer.data[0]='\0';
                } else {
                    DEBUG("no command found, instead:");
                    DEBUG(c);
                    c_buffer_push(c, &buffer);
                    bt_input_status = BT_INPUT_INIT;
                }
                break;
            
            case BT_INPUT_COMMAND:
                if(c == CR) {
                    DEBUG("INTERPRETATE");
                    interpretateCommand(commandBuffer.data);
                    bt_input_status = BT_INPUT_INIT;
                } else {
                    if(commandBuffer.tail < COMMAND_BUFFER_LEN-1) {
                        commandBuffer.data[commandBuffer.tail] = c;
                        commandBuffer.data[commandBuffer.tail+1] = '\0';
                        commandBuffer.tail++;
                        DEBUG(c);
                    }
                }
                break;
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
                goto command_found; //that's right, I'm using goto.
            }
        }
    }
    
    return;
    
    command_found:
        DEBUG("FOUND! ");
        DEBUG(i);DEBUG(j);
        if(i == COMMAND_INDEX_BTSTATE) {
            bt_status = atoi(&command[j]);
            if(bt_status<0 || bt_status>4) {
		bt_status=1;
		return;
	    }
	    DEBUG("STATUS:");
            DEBUG(bt_status);
            if(bt_status == BT_INIT)             groveSBT_onInit();
            else if (bt_status == BT_READY)      groveSBT_onReady();
            else if (bt_status == BT_INQUIRING)  groveSBT_onInquiring();
            else if (bt_status == BT_CONNECTING) groveSBT_onConnecting();
            else if (bt_status == BT_CONNECTED)  groveSBT_onConnected();
        }

}

boolean groveSBT_available() {
   return (c_buffer_len(&buffer)>0);
}

char groveSBT_read() {
    char c=-1;
    c_buffer_pull(&c, &buffer);
    return c;
}

void groveSBT_print(char *txt) {
    SERIAL_OUT(txt);
}

void groveSBT_init() {
    ss.begin(9600);
}

void groveSBT_inq(){
    SERIAL_OUT("\r\n+INQ=1\r\n");
}
#endif
