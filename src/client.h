/* 
 * File:   client.h
 * Author: adam
 *
 * Created on September 23, 2012, 12:27 PM
 */

#ifndef CLIENT_H
#define	CLIENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#include <libwebsockets.h>
#include "protocol.h"

    /**
     */
    extern int client_callback_http(struct libwebsocket_context *context,
            struct libwebsocket *wsi,
            enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len) {
        return 0;
    }

    /**
     */
    extern int client_ws_callback(struct libwebsocket_context *context,
            struct libwebsocket *wsi,
            enum libwebsocket_callback_reasons reason,
            void *user, void *in, size_t len) {
        return 0;
    }


    /**
     * Used by libwebsockets to define callbacks for each protocol
     */
    extern struct libwebsocket_protocols _client_protocols[] = {
        /* first protocol must always be HTTP handler */
        {
            "http-only", /* name */
            client_callback_http, /* callback for initial http connection*/
            0 /* per_session_data_size */
        },
        {
            COMMOTION_AP_PROTOCOL_NAME,
            client_ws_callback, // Main callback for server
            sizeof (struct per_session_data__ws_client),
        },
        {
            NULL, NULL, 0 /* End of list */
        }
    };

    extern int send_msg_to_ap(const uint32_t addr, json_t* msg) {


        int port = 7681;
        int use_ssl = 0;
        struct libwebsocket_context *context;
        struct libwebsocket *wsi_dumb;
        int ietf_version = -1;

        context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
                _client_protocols, libwebsocket_internal_extensions,
                NULL, NULL, -1, -1, 0);
        if (context == NULL) {
            fprintf(stderr, "Creating libwebsocket context failed\n");
            return 1;
        }

        const char *address = addr_to_string(ntohl(addr));
        fprintf(stderr, "Trying to connect to %s\n", address);
        wsi_dumb = libwebsocket_client_connect(context, address, port, use_ssl,
                "/", address, address,
                COMMOTION_AP_PROTOCOL_NAME, ietf_version);

        if (wsi_dumb == NULL) {
            fprintf(stderr, "libwebsocket dumb connect failed\n");
            return -1;
        }

        fprintf(stderr, "Websocket to %s connections opened\n", address);



        /*
            char *retData = json_dumps(msg, JSON_COMPACT);
            if (retData != NULL) {
                unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + strlen(retData) + LWS_SEND_BUFFER_POST_PADDING];
                unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
                int n;
                n = sprintf((char *) p, "%s", retData);

                n = libwebsockets_broadcast(&_socket_protocols[PROTOCOL_COMMOTION_WS], p, n);
                if (n < 0) {
                    fprintf(stderr, "ERROR writing to socket\n");
                }

                free(retData);
            } else {
                fprintf(stderr, "ERROR failed to dump json data\n");
            }
            json_decref(msg);
         */
    }

#ifdef	__cplusplus
}
#endif

#endif	/* CLIENT_H */

