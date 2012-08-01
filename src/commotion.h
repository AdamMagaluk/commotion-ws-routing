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

#ifdef	__cplusplus
extern "C" {
#endif

#define COMMOTION_PROTOCOL_NAME "commotion-ws"

#define CWS_FIELD_MSG_TYPE "m"

    enum commotion_msg_type {
        COMMOTION_MSG_NODE_JOINED,
        COMMOTION_MSG_SEND,
        COMMOTION_MSG_NODE_DISSCONNECTED
    };

    /*
     * one of these is auto-created for each connection and a pointer to the
     * appropriate instance is passed to the callback in the user parameter
     *
     * for this example protocol we use it to individualize the count for each
     * connection.
     */
    struct per_session_data__ws_client {
        int number;
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


    static int handle_client_data(struct libwebsocket_context *context,
            struct libwebsocket *wsi, void *user, void *in, size_t len);


#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_H */

