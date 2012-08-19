
// For getting ip address structures
#include <netinet/in.h>

#include "commotion.h"

#define LOCAL_HOST_ADDR 0x7F000001

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
        void *user, void *in, size_t len) {

    // Get client data
    struct per_session_data__ws_client *pss = user;
    // Get fd of socket.
    int socket=libwebsocket_get_socket_fd(wsi);
    
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            // Get ip and hostname and store it in client data
            libwebsockets_get_peer_addresses(socket, pss->client_name,
                    sizeof (pss->client_name), pss->client_ip, sizeof (pss->client_ip));

            // Get ip of client as int.
            struct sockaddr_in antelope;
            inet_aton(pss->client_ip, &antelope.sin_addr);
            pss->addr.addr = htonl(antelope.sin_addr.s_addr);
            pss->addr.id = socket;
            
            // Add node to topology
            topology_add_node(getLocalAddress(), pss->addr, pss->client_name);
            break;

        case LWS_CALLBACK_BROADCAST:
            if (libwebsocket_write(wsi,(unsigned char*)in, len, LWS_WRITE_TEXT) < 0) {
                // @todo Handle error when write fails
            }
            break;
        case LWS_CALLBACK_RECEIVE:
            // Send to client data callback
            return handle_client_data(context, wsi, user, in, len);
            break;
        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            commotion_handshake_info((struct lws_tokens *) (long) user);
            break;
        case LWS_CALLBACK_CLOSED:
            // Send to client disconnects
            msg_client_disconnect(context, wsi, user);
            break;
        default:
            break;
    }
    return 0;
}

/**
 * Handle all messages from the clients
 *  - Parses the json and tests if it is a protocol,
 *  - Calls each message type callback for specific commands
 */
static int handle_client_data(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, void *in, size_t len) {
    
    int ret = 0;
    json_t *root;
    json_error_t error;
    root = json_loadb(in, len, 0, &error);
    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    } else {
        json_t *mt = json_object_get(root, CWS_FIELD_MSG_TYPE);
        if (!json_is_integer(mt)) {
            fprintf(stderr, "error: Message type not found\n");
        } else {

            //Get the message type
            const int msg_type = json_integer_value(mt);
            switch (msg_type) {
                case COMMOTION_MSG_CLIENT_REGISTERED:
                    ret = msg_client_connect(context, wsi, user, root);
                    break;
                case COMMOTION_MSG_CLIENT_DISSCONNECTED:
                    return 0;
                case COMMOTION_MSG_FORWARD_MSG:
                    ret = msg_forward_data(context, wsi, user, root,in,len);
                    break;
                case COMMOTION_MSG_REQ_TOPOLOGY:
                    ret = msg_req_topology(context, wsi, user, root);
                    break;
                case COMMOTION_MSG_BROADCAST_MSG:
                    ret = msg_broadcast_msg(context, wsi, user, root,in,len);
                    break;
                default:
                    fprintf(stderr, "error: Message type not valid\n");
            }

        }
        json_decref(root);
    }
    return ret;
}

/**
 * Handle client_connect message, 
 *   - Checks if data has the app parameter and if so adds to the topology
 *     updates the topology if successful
 */
static int msg_client_connect(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root) {

    struct per_session_data__ws_client* pss = user;

    fprintf(stdout, "log: Client connected %d\n",libwebsocket_get_socket_fd(wsi) );

    json_t *data, *apps;
    data = json_object_get(root, CWS_FIELD_MSG_DATA);
    if (!json_is_object(data)) {
        fprintf(stderr, "error: missing data arg or not object\n");
        return 0;
    }

    apps = json_object_get(data, MSG_REGISTER_FIELD_APPS);
    if (!json_is_array(apps)) {
        fprintf(stderr, "error: missing protocols in message client connect\n");
        return 0;
    }
    topology_update_node_apps(getLocalAddress(), pss->addr, apps);
    
    // Update topology on connected clients
    update_topology();
    
    return 0;
}

/**
 * Handles the libwebsockets closed event.
 *   @todo should only handle message to close socket and close socket and call
 * a seprate function that both the msg dissconect and socket close call.
 * 
 *  - Updates topology.
 */
static int msg_client_disconnect(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user) {

    struct per_session_data__ws_client* pss = user;
    fprintf(stdout, "log: Client %s disconnected %d\n", pss->client_name,pss->addr.id);
    topology_remove_node(getLocalAddress(), pss->addr);
    
    // Update topology on connected clients
    update_topology();
}


/**
 * Handles forward message.
 *  - Tries to forward message to the destination client if client exists in 
 * topology.
 * 
 *  @todo - Possibly add error callback the messages the sending client when 
 * dest is not found or fails to send.
 */

static int msg_forward_data(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root, void *in, size_t len) {
    

    json_t *data, *dst,*dst_id,*dst_ip;
    dst = json_object_get(root, CWS_FIELD_DST);
    if (!json_is_object(dst)) {
        fprintf(stderr, "error: msg_forward_data: missing destination in message\n");
        return 0;
    }
    
    struct Address addr_st;
    dst_ip = json_object_get(dst, "ip");
    dst_id = json_object_get(dst, "id");
    if (!json_is_integer(dst_ip) || !json_is_integer(dst_id)) {
        fprintf(stderr, "error: msg_forward_data: destination object failed\n");
        return 0;
    }
    addr_st.addr = json_integer_value(dst_ip);
    addr_st.id = json_integer_value(dst_id);

    struct Address host = topology_return_ap_for_node(addr_st);
    struct Address localHost = getLocalAddress();

    data = json_object_get(root, CWS_FIELD_MSG_DATA);
    if (!json_is_object(data)) {
        fprintf(stderr, "error: missing data arg or not object\n");
        return 0;
    }
    
    // Append source address to message
    struct per_session_data__ws_client *pss = user;
    json_t* source=json_object();
    addr_struct_to_json(pss->addr,source);
    json_object_set_new(root,CWS_FIELD_SRC,source);
    
    // Handle for locally connected clients
    if(topology_compare_address(localHost, host) == 1) {
        struct libwebsocket * nodews = wsi_from_fd(context, addr_st.id);
        if (nodews != NULL) {
            // Because we appended the new source address we have to remake packet
            char *retData = json_dumps(root, JSON_COMPACT);
            if (retData != NULL) {
                unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + strlen(retData) + LWS_SEND_BUFFER_POST_PADDING];
                unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
                int n;
                n = sprintf((char *) p, "%s", retData);
                if (libwebsocket_write(nodews, p, n, LWS_WRITE_TEXT) < 0) {
                    fprintf(stderr, "error: failed to send..\n");
                }
                free(retData);
            } else {
                fprintf(stderr, "ERROR failed to dump json data\n");
            }
        }
    } else {
        //@todo Handle for remote hosts

    }
    return 0;
}

/**
 * Handles broadcast message.
 *  - Broadcasts the message to all active clients using libsockets broadcast
 * function, modifies packet to add source address.
 */
static int msg_broadcast_msg(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root, void *in, size_t len){
    
    // Append source address to message
    struct per_session_data__ws_client *pss = user;
    json_t* source=json_object();
    addr_struct_to_json(pss->addr,source);
    json_object_set_new(root,CWS_FIELD_SRC,source);    
    

    char *retData = json_dumps(root, JSON_COMPACT);
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
}

/**
 * Handles request topology
 *  - Responds to node with topology
 */
static int msg_req_topology(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root) {

    struct per_session_data__ws_client* pss = user;
    json_t* msg = make_msg(COMMOTION_MSG_TOPOLOGY_UPDATE,"127.0.0.1",pss->client_ip,topology_root());
    char* retData = json_dumps(msg, JSON_COMPACT);
    if (retData != NULL) {
        unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + sizeof (retData) + LWS_SEND_BUFFER_POST_PADDING];
        unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
        int n;
        n = sprintf((char *) p, "%s", retData);
        n = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
        if (n < 0) {
            fprintf(stderr, "ERROR writing to socket\n");
        }
        free(retData);
    } else {
        fprintf(stderr, "ERROR failed to dump json data\n");
    }
    
    json_decref(msg);
    return 0;
}


/**
 * Makes a json_t object with protocol, must free return value.
 */
static json_t* make_msg(int mt,  char* src,  char* dst, json_t* mdata) {
    json_t* msg = json_object();
    json_object_set_new(msg, CWS_FIELD_MSG_TYPE, json_integer(mt));

    if (src != NULL)
        json_object_set(msg, CWS_FIELD_SRC, json_string(src));
    if (dst != NULL)
        json_object_set(msg, CWS_FIELD_DST, json_string(dst));

    if (mdata != NULL)
        json_object_set(msg, CWS_FIELD_MSG_DATA, mdata);
    
    return msg;
}

/**
 *  Broadcasts topology to local clients.
 *  @todo - Forward to all other aps.
 */
void update_topology() {
    json_t* msg = make_msg(COMMOTION_MSG_TOPOLOGY_UPDATE, "127.0.0.1", "255.255.255.255", topology_root());
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
}

/*
 * this is just an example of parsing handshake headers, you don't need this
 * in your code unless you will filter allowing connections by the header
 * content
 * 
 * Taken from the demo application in libwebsockets, could be used to filter
 * clients.
 */
void commotion_handshake_info(struct lws_tokens *lwst) {
    int n;
    static const char *token_names[WSI_TOKEN_COUNT] = {
        /*[WSI_TOKEN_GET_URI]		=*/ "GET URI",
        /*[WSI_TOKEN_HOST]		=*/ "Host",
        /*[WSI_TOKEN_CONNECTION]	=*/ "Connection",
        /*[WSI_TOKEN_KEY1]		=*/ "key 1",
        /*[WSI_TOKEN_KEY2]		=*/ "key 2",
        /*[WSI_TOKEN_PROTOCOL]		=*/ "Protocol",
        /*[WSI_TOKEN_UPGRADE]		=*/ "Upgrade",
        /*[WSI_TOKEN_ORIGIN]		=*/ "Origin",
        /*[WSI_TOKEN_DRAFT]		=*/ "Draft",
        /*[WSI_TOKEN_CHALLENGE]		=*/ "Challenge",

        /* new for 04 */
        /*[WSI_TOKEN_KEY]		=*/ "Key",
        /*[WSI_TOKEN_VERSION]		=*/ "Version",
        /*[WSI_TOKEN_SWORIGIN]		=*/ "Sworigin",

        /* new for 05 */
        /*[WSI_TOKEN_EXTENSIONS]	=*/ "Extensions",

        /* client receives these */
        /*[WSI_TOKEN_ACCEPT]		=*/ "Accept",
        /*[WSI_TOKEN_NONCE]		=*/ "Nonce",
        /*[WSI_TOKEN_HTTP]		=*/ "Http",
        /*[WSI_TOKEN_MUXURL]	=*/ "MuxURL",
    };

    for (n = 0; n < WSI_TOKEN_COUNT; n++) {
        if (lwst[n].token == NULL)
            continue;

        fprintf(stderr, "    %s = %s\n", token_names[n], lwst[n].token);
    }
}

/**
 * Initial callback for http connection before websocket protocol takes over.
 *  Currently does nothing and allows the websocket to connect.
 */
extern int callback_http(struct libwebsocket_context *context,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_HTTP: break;
        case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
            break;
        default:
            break;
    }
    return 0;
}