#Commotion C Websocket Routing Server
 
This Makefile makes a shared library called libcommotionws which holds
all the base code for the application. I did this to make unit testing easier.
It makes the main binary and a test application i was using to test the topology
code. The main binary has the name wsroutingserver.

##Dependencies

* libwebsockets : C Websocket server  
    * Used as the basis of the program and does most of the heavy lifting with
regards to the websocket for us. All we do is define callbacks for events the lib
gives us.

* libjanson : C implementation of json.
    * To parse incoming protocol message from the clients as every
thing is based in json. 
    * For holding topology information, I made the choice to use json structs
for topology information for two reasons, one it sped up development as
implementing complex tree structures in C can be a pain and in order to format the
topology information for the client it has to be in json making json dumps of the 
topology a one line operation.  
 
##Important files:

* commotion.c/h - Has all socket event callbacks and client protocol information.
Handles adding and removing clients from the topology.

* topology.c/h - Has all functions to add/remove/find/get/ nodes in the topology.

* main.c - Basic setup of a libwebsocket server straight from libwebsockets example.
Uses a forked architecture. 
