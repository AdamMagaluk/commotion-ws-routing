/*
 * libwebsockets-test-server - libwebsockets test implementation
 *
 * Copyright (C) 2010-2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "commotion.h"

#define LOCAL_HOST_ADDR 0x7F000001


static struct option options[] = {
    { "help", no_argument, NULL, 'h'},
    { "port", required_argument, NULL, 'p'},
    { "ssl", no_argument, NULL, 's'},
    { "killmask", no_argument, NULL, 'k'},
    { "interface", required_argument, NULL, 'i'},
    { "closetest", no_argument, NULL, 'c'},
    { NULL, 0, 0, 0}
};

#include "topology.h"
#define LOCAL_RESOURCE_PATH  "commotion-ws/";

int main(int argc, char **argv) {

    topology_init();


    int n = 0;
    const char *cert_path =
            LOCAL_RESOURCE_PATH"/libwebsockets-test-server.pem";
    const char *key_path =
            LOCAL_RESOURCE_PATH"/libwebsockets-test-server.key.pem";
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 1024 +
            LWS_SEND_BUFFER_POST_PADDING];
    int port = 7681;
    int use_ssl = 0;
    struct libwebsocket_context *context;
    int opts = 0;
    char interface_name[128] = "";
    const char *interface = NULL;
#ifdef LWS_NO_FORK
    unsigned int oldus = 0;
#endif

    fprintf(stderr, "libwebsockets test server\n"
            "(C) Copyright 2010-2011 Andy Green <andy@warmcat.com> "
            "licensed under LGPL2.1\n");

    while (n >= 0) {
        n = getopt_long(argc, argv, "ci:khsp:", options, NULL);
        if (n < 0)
            continue;
        switch (n) {
            case 's':
                use_ssl = 1;
                break;
            case 'k':
                opts = LWS_SERVER_OPTION_DEFEAT_CLIENT_MASK;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'i':
                strncpy(interface_name, optarg, sizeof interface_name);
                interface_name[(sizeof interface_name) - 1] = '\0';
                interface = interface_name;
                break;
            case 'h':
                fprintf(stderr, "Usage: test-server "
                        "[--port=<p>] [--ssl]\n");
                exit(1);
        }
    }

    if (!use_ssl)
        cert_path = key_path = NULL;

    context = libwebsocket_create_context(port, interface, _socket_protocols,
            libwebsocket_internal_extensions,
            cert_path, key_path, -1, -1, opts);
    if (context == NULL) {
        fprintf(stderr, "libwebsocket init failed\n");
        return -1;
    }

    buf[LWS_SEND_BUFFER_PRE_PADDING] = 'x';

#ifdef LWS_NO_FORK

    /*
     * This example shows how to work with no forked service loop
     */

    fprintf(stderr, " Using no-fork service loop\n");

    n = 0;
    while (n >= 0) {

        /*
         * This example server does not fork or create a thread for
         * websocket service, it all runs in this single loop.  So,
         * we have to give the websockets an opportunity to service
         * "manually".
         *
         * If no socket is needing service, the call below returns
         * immediately and quickly.  Negative return means we are
         * in process of closing
         */

        n = libwebsocket_service(context, 50);
    }

#else


    /*
     * This example shows how to work with the forked websocket service loop
     */

    fprintf(stderr, " Using forked service loop\n");

    /*
     * This forks the websocket service action into a subprocess so we
     * don't have to take care about it.
     */

    n = libwebsockets_fork_service_loop(context);
    if (n < 0) {
        fprintf(stderr, "Unable to fork service loop %d\n", n);
        return 1;
    }

    while (1) {
        sleep(2);

        /*
         * This broadcasts to all dumb-increment-protocol connections
         * at 20Hz.
         *
         * We're just sending a character 'x', in these examples the
         * callbacks send their own per-connection content.
         *      
         * You have to send something with nonzero length to get the
         * callback actions delivered.
         *
         * We take care of pre-and-post padding allocation.
         */

        // unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
        // sprintf((char *) p, "1234567890");
        // libwebsockets_broadcast(&protocols[PROTOCOL_COMMOTION_WS],p, 10);
    }
#endif
    libwebsocket_context_destroy(context);

    topology_deref();

    return 0;
}
