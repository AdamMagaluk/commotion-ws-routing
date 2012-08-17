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

#include <libwebsockets.h>
#include <private-libwebsockets.h>
#include <jansson.h>

#include "topology.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define COMMOTION_PROTOCOL_NAME "commotion-ws"
#define CWS_FIELD_MSG_TYPE "mt"
#define CWS_FIELD_MSG_DATA "d"
#define CWS_FIELD_SRC "src"
#define CWS_FIELD_DST "dst"
#define MSG_REGISTER_FIELD_PROTOCOLS "p"
    
#define MAX_NUMBER_OF_PROTOCOLS 10
#define MAX_PROTOCOl_NAME_SIZE 50

    enum demo_protocols {
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
        char client_name[128];
        char client_ip[128];
        struct Address addr;
    };
    
    
    extern int callback_http(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);
    
    extern int commotion_ws_callback(struct libwebsocket_context *context,
            struct libwebsocket *wsi,
            enum libwebsocket_callback_reasons reason,
            void *user, void *in, size_t len);
    
      /* list of supported protocols and callbacks */

      extern struct libwebsocket_protocols _socket_protocols[] = {
        /* first protocol must always be HTTP handler */

        {
            "http-only", /* name */
            callback_http, /* callback */
            0 /* per_session_data_size */
        },
        {
            COMMOTION_PROTOCOL_NAME,
            commotion_ws_callback,
            sizeof (struct per_session_data__ws_client),
        },
        {
            NULL, NULL, 0 /* End of list */
        }
    };


/* this protocol server (always the first one) just knows how to do HTTP */
extern int callback_http(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len)
{
	switch (reason) {
	case LWS_CALLBACK_HTTP: break;
	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
		break;
	default:
		break;
        }

        return 0;
    }





    
    enum commotion_msg_type {
        COMMOTION_MSG_CLIENT_REGISTERED,
        COMMOTION_MSG_CLIENT_DISSCONNECTED,
        COMMOTION_MSG_FORWARD_MSG,
        COMMOTION_MSG_REQ_TOPOLOGY,
        COMMOTION_MSG_TOPOLOGY_UPDATE
    };

    
    // Check if t is valid msg type
    // Return 0 on success, all else is not valid.
    inline int is_valid_msg_type(int t);

  
    struct Address getLocalAddress(){
        struct Address a;
        a.addr = LOCAL_HOST_ADDR;
        a.id = 0;
        return a;
    }
    
    /**
     * Main Callback for commotion and libwebsockets
     * @param context
     * @param wsi
     * @param reason
     * @param user
     * @param in
     * @param len
     * @return 
     */
    extern int commotion_ws_callback(struct libwebsocket_context *context,
            struct libwebsocket *wsi,
            enum libwebsocket_callback_reasons reason,
            void *user, void *in, size_t len);


    /*
     * this is just an example of parsing handshake headers, you don't need this
     * in your code unless you will filter allowing connections by the header
     * content
     */

    extern void commotion_handshake_info(struct lws_tokens *lwst);


    /**
     * Handle all client data
     * @param context
     * @param wsi
     * @param user
     * @param in
     * @param len
     * @return 
     */
    static int handle_client_data(struct libwebsocket_context *context,
            struct libwebsocket *wsi, void *user, void *in, size_t len);


    /**
     * Handle client_connect
     * @param context
     * @param wsi
     * @param user
     * @param root
     * @return 
     */
    static int msg_client_connect(struct libwebsocket_context *context,
            struct libwebsocket *wsi, void *user, json_t *root);
    
    extern int msg_client_disconnect(struct libwebsocket_context *context,
            struct libwebsocket *wsi, void *user);
    
    static int msg_forward_data(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root, void *in, size_t len);

    /**
     * Handle client_connect
     * @param context
     * @param wsi
     * @param user
     * @param root
     * @return 
     */
    static int msg_req_topology(struct libwebsocket_context *context,
            struct libwebsocket *wsi, void *user, json_t *root);


    static json_t* make_msg(int mt, char* src, char* dst,json_t* mdata);
    
    extern void update_topology();
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_H */

