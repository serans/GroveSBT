GroveSBT
========

Grove Serial Bluetooth - Arduino Interface

Usage:
 * Include both GroveSBT.h and SoftwareSerial.h in your sketch
 * Call groveSBT_init() to initialize the controller
 * Include groveSBT_loop() in the main loop
 * Use callbacks to control the BT status
 * Use groveSBT_inq to begin inquiring, so that the device is discoverable
 * Once the connection is stablished, use groveSBT_available(), groveSBT_read() and groveSBT_write() to communicate.
