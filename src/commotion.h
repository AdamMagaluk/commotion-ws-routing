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
#include <jansson.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define COMMOTION_PROTOCOL_NAME "commotion-ws"
#define CWS_FIELD_MSG_TYPE "mt"
#define CWS_FIELD_MSG_DATA "d"
#define MSG_REGISTER_FIELD_PROTOCOLS "p"
    
#define MAX_NUMBER_OF_PROTOCOLS 10
#define MAX_PROTOCOl_NAME_SIZE 50
    
    enum commotion_msg_type {
        COMMOTION_MSG_CLIENT_REGISTERED,
        COMMOTION_MSG_CLIENT_DISSCONNECTED,
        COMMOTION_MSG_FORWARD_MSG,
        COMMOTION_MSG_REQ_TOPOLOGY
    };

    // Check if t is valid msg type
    // Return 0 on success, all else is not valid.
    inline int is_valid_msg_type(int t);
    
    /*
     * one of these is auto-created for each connection and a pointer to the
     * appropriate instance is passed to the callback in the user parameter
     *
     * for this example protocol we use it to individualize the count for each
     * connection.
     */
    struct per_session_data__ws_client {
        char protocols[MAX_NUMBER_OF_PROTOCOLS][MAX_PROTOCOl_NAME_SIZE];
        int protocol_len;
    };

    
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

    
#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_H */

