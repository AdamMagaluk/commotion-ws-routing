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
    { "daemon", no_argument, NULL, 'd'},
    { NULL, 0, 0, 0}
};

extern int useSyslog;

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


    int fork_as_daemon=0;

    while (n >= 0) {
        n = getopt_long(argc, argv, "ci:khsp:d", options, NULL);

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
                printf("libwebsockets test server\n"
                       "(C) Copyright 2010-2011 Andy Green <andy@warmcat.com> "
                       "licensed under LGPL2.1\n");
                printf("Usage: test-server "
                        "[--port=<p>] [--ssl] [-d Deamonize]\n");
                exit(1);
            case 'd':
                fork_as_daemon = 1;
                break;
        }
    }

    if (!use_ssl)
        cert_path = key_path = NULL;

    context = libwebsocket_create_context(port, interface, _socket_protocols,
            libwebsocket_internal_extensions,
            cert_path, key_path, -1, -1, opts);
    if (context == NULL) {
        traceEvent(TRACE_ERROR, "libwebsocket init failed");
        return -1;
    }

    buf[LWS_SEND_BUFFER_PRE_PADDING] = 'x';

    init_client_contex();



    #ifndef WIN32
        if (fork_as_daemon) {
            useSyslog=1; // send traceEvent to syslog
            daemon(0, 0);
            freopen( "/dev/null", "r", stdin);
            freopen( "/dev/null", "w", stdout);
            freopen( "/dev/null", "w", stderr);
        }
    #endif


    /*
     * This example shows how to work with no forked service loop
     */

    traceEvent(TRACE_NORMAL, "Using no-fork service loop");

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

    return 0;
}
