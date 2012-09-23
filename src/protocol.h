/* 
 * File:   protocol.h
 * Author: adam
 *
 * Created on September 23, 2012, 12:39 PM
 */

#ifndef PROTOCOL_H
#define	PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "topology.h"

// Base websocket protocol name
#define COMMOTION_PROTOCOL_NAME "commotion-ws"
    
// Protocol name for inter-accesspoint communication for updating topology
#define COMMOTION_AP_PROTOCOL_NAME "commotion-ap"
    
// Message type
#define CWS_FIELD_MSG_TYPE "mt"
// Source address, not required for all packets
#define CWS_FIELD_SRC "src"
// Destination address, not required for all packets.
#define CWS_FIELD_DST "dst"
//Data payload of packet
#define CWS_FIELD_MSG_DATA "d"

/**
 * Commotion message types
 */
enum commotion_msg_type {
    COMMOTION_MSG_CLIENT_REGISTERED, // After connection client sends this with apps
    COMMOTION_MSG_CLIENT_DISSCONNECTED, // Force dissconnect
    COMMOTION_MSG_FORWARD_MSG, // Foward message to destination
    COMMOTION_MSG_REQ_TOPOLOGY, // Request the topology
    COMMOTION_MSG_TOPOLOGY_UPDATE, // Message sent when topology changes
    COMMOTION_MSG_BROADCAST_MSG // Broadcast a message to all clients
};

// In COMMOTION_MSG_CLIENT_REGISTERED payload will have p:[String] for all apps
#define MSG_REGISTER_FIELD_APPS "p"

// For libwebsockets structure size
#define MAX_NUMBER_OF_PROTOCOLS 3
// For libwebsockets structure size
#define MAX_PROTOCOl_NAME_SIZE 50

enum libwebsockets_protocols {
    /* always first */
    PROTOCOL_HTTP = 0,

    PROTOCOL_COMMOTION_WS,
    
    PROTOCOL_COMMOTION_AP,
    
    /* always last */
    DEMO_PROTOCOL_COUNT
};

/*
 * one of these is auto-created for each connection and a pointer to the
 * appropriate instance is passed to the callback in the user parameter
 *
 * for this example protocol we use it to individualize the count for each
 * connection.
 */
struct per_session_data__ws_client {
    char client_name[128]; // hostname of client
    char client_ip[128]; // clients ip in text
    struct Address addr; // Topology Address
};

#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_H */

