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
#include <time.h>
#include <sys/time.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <syslog.h>

// LibWebsockets api
#include <libwebsockets.h>

// Json parsing using libjansson
#include <jansson.h>

// Commotion topology management.
#include "protocol.h"
#include "client.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* Utility variables */

#define TRACE_ERROR    0, __FILE__, __LINE__
#define TRACE_WARNING  1, __FILE__, __LINE__
#define TRACE_NORMAL   2, __FILE__, __LINE__
#define TRACE_INFO     3, __FILE__, __LINE__

extern int traceLevel;

/* Utility functions */

extern void traceEvent(int eventTraceLevel, char* file, int line, char * format, ...);

/* Protocol functions */

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
 * Main entry point for inter ap protocol
 */
extern int commotion_ap_callback(struct libwebsocket_context *context,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason,
        void *user, void *in, size_t len);

/**
 * Handle all messages from the access points
 */
static int handle_ap_data(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, void *in, size_t len);


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
        COMMOTION_AP_PROTOCOL_NAME, 
        commotion_ap_callback, // Main callback for server
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

static int msg_ap_topology_update(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root, void *in, size_t len);

static int msg_ap_forward_msg(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root, void *in, size_t len);

/**
 * Makes a json_t object with protocol, must free return value.
 */
static json_t* make_msg(int mt, char* src, char* dst, json_t* mdata);




/**
 *  Broadcasts topology to local clients.
 *  @todo - Forward to all other aps.
 */
extern void update_topology();

extern void update_toplolgy_on_local_clients();

extern void update_toplolgy_on_remote_ap();

#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_H */

