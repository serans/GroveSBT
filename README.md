GroveSBT
========

Arduino controller for the [Grove Serial Bluetooth module](http://www.seeedstudio.com/wiki/Grove_-_Serial_Bluetooth)

![GroveSBT](http://www.seeedstudio.com/wiki/images/thumb/6/66/Twigbt00.jpg/400px-Twigbt00.jpg "Grove Serial Bluetooth")

Installation:
Just put the ``GroveSBT`` folder into your arduino's libraries directories. On linux, this typically means:
  * ``$HOME/sketchbook/libraries`` or
  * ``usr/share/arduino/libraries`` if you want to make it available to all users

Usage:
 * Include both ``<GroveSBT.h>`` and ``<SoftwareSerial.h>`` in your sketch
 * Call ``groveSBT_init()`` to initialize the controller
 * Include ``groveSBT_loop()`` in the main loop
 * Use callbacks to respond to events:
   * ``groveSBT_onInit`` when the BT is turned on
   * ``groveSBT_onReady`` when the BT is ready to receive instructions
   * ``groveSBT_onInquiring`` when the BT is inquiring
   * ``groveSBT_onConnecting`` when a connection is being stablished
   * ``groveSBT_onConnected`` when there's active connection. When disconnected, returns to the ready state and hence launches ``groveSBT_onReady``
   * ``groveSBT_onNewLine`` invoked every time a LF character is read
 * Use ``groveSBT_inq()`` to begin inquiring, so that the device is discoverable.
 * Once the connection is stablished, use ``groveSBT_available()``, ``groveSBT_read()`` and ``groveSBT_write()`` to communicate.


Licence
========
All source code in this project is
  * (c) MH Serans, unless stated otherwise
  * Available under the MIT licence, unless stated otherwise (see LICENCE.txt)
