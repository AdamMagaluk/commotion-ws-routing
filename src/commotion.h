/* 
 * File:   commotion.h
 * Author: adam
 *
 * Created on July 30, 2012, 10:54 PM
 */

#ifndef COMMOTION_H
#define	COMMOTION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>

// LibWebsockets api
#include <libwebsockets.h>

// Json parsing using libjansson
#include <jansson.h>

// Commotion topology managment.
#include "topology.h"

#ifdef	__cplusplus
extern "C" {
#endif

// Base websocket protocol name
#define COMMOTION_PROTOCOL_NAME "commotion-ws"
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




/**
 * Initial callback for http connection before websocket protocol takes over.
 *  Currently does nothing and allows the websocket to connect.
 */
extern int callback_http(struct libwebsocket_context *context,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);

/**
 * Main entry point for the protocol
 *  - On connection it handles adding client to topology and updates topology
 *  - On data parses protocol message
 *  - On broadcast sends data to node
 *  - On disconnect removes client from topology and updates topology. 
 */
extern int commotion_ws_callback(struct libwebsocket_context *context,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason,
        void *user, void *in, size_t len);

/**
 * Used by libwebsockets to define callbacks for each protocol
 */
extern struct libwebsocket_protocols _socket_protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "http-only", /* name */
        callback_http, /* callback for initial http connection*/
        0 /* per_session_data_size */
    },
    {
        COMMOTION_PROTOCOL_NAME, 
        commotion_ws_callback, // Main callback for server
        sizeof (struct per_session_data__ws_client),
    },
    {
        NULL, NULL, 0 /* End of list */
    }
};


/**
 * Temp function to get local address to server.
 * @todo FIX ME it should be calling function to local ip.
 */
struct Address getLocalAddress() {
    struct Address a;
    a.addr = LOCAL_HOST_ADDR;
    a.id = 0;
    return a;
}


/*
 * this is just an example of parsing handshake headers, you don't need this
 * in your code unless you will filter allowing connections by the header
 * content
 * 
 * Taken from the demo application in libwebsockets, could be used to filter
 * clients.
 */
extern void commotion_handshake_info(struct lws_tokens *lwst);


/**
 * Handle all messages from the clients
 *  - Parses the json and tests if it is a protocol,
 *  - Calls each message type callback for specific commands
 */
static int handle_client_data(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, void *in, size_t len);


/**
 * Handle client_connect message, 
 *   - Checks if data has the app parameter and if so adds to the topology
 *     updates the topology if successful
 */
static int msg_client_connect(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root);

/**
 * Handles the libwebsockets closed event.
 *   @todo should only handle message to close socket and close socket and call
 * a seprate function that both the msg dissconect and socket close call.
 * 
 *  - Updates topology.
 */
static int msg_client_disconnect(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user);

/**
 * Handles forward message.
 *  - Tries to forward message to the destination client if client exists in 
 * topology.
 * 
 *  @todo - Possibly add error callback the messages the sending client when 
 * dest is not found or fails to send.
 */
static int msg_forward_data(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root, void *in, size_t len);

/**
 * Handles broadcast message.
 *  - Broadcasts the message to all active clients using libsockets broadcast
 * function, modifies packet to add source address.
 */
static int msg_broadcast_msg(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root, void *in, size_t len);


/**
 * Handles request topology
 *  - Responds to node with topology
 */
static int msg_req_topology(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root);

/**
 * Makes a json_t object with protocol, must free return value.
 */
static json_t* make_msg(int mt, char* src, char* dst, json_t* mdata);

/**
 *  Broadcasts topology to local clients.
 *  @todo - Forward to all other aps.
 */
extern void update_topology();

#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_H */

