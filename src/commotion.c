
#include <netinet/in.h>

#include "commotion.h"
#define LOCAL_HOST_ADDR 0x7F000001

// Check if t is valid msg type
// Return 0 on success, all else is not valid.

inline int is_valid_msg_type(int t) {
    switch (t) {
        case COMMOTION_MSG_CLIENT_REGISTERED:
        case COMMOTION_MSG_CLIENT_DISSCONNECTED:
        case COMMOTION_MSG_FORWARD_MSG:
        case COMMOTION_MSG_REQ_TOPOLOGY:
            return 0;
        default:
            return -1;
    }
}

static int handle_client_data(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, void *in, size_t len) {
    
    printf("handle_client_data 1\n");
    
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
                case COMMOTION_MSG_FORWARD_MSG:
                    return 0;
                case COMMOTION_MSG_REQ_TOPOLOGY:
                    ret = msg_req_topology(context, wsi, user, root);
                    break;
                default:
                    fprintf(stderr, "error: Message type not valid\n");
            }

        }
        json_decref(root);
    }
    
    printf("handle_client_data END\n");
    
    return ret;
}

static int msg_client_connect(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root) {

    struct per_session_data__ws_client* pss = user;

    fprintf(stdout, "log: Client connected %d\n",libwebsocket_get_socket_fd(wsi) );

    json_t *data, *protocols;
    data = json_object_get(root, CWS_FIELD_MSG_DATA);
    if (!json_is_object(data)) {
        fprintf(stderr, "error: missing data arg or not object\n");
        return 0;
    }

    protocols = json_object_get(data, MSG_REGISTER_FIELD_PROTOCOLS);
    if (!json_is_array(protocols)) {
        fprintf(stderr, "error: missing protocols in message client connect\n");
        return 0;
    }
    ap_node_protocols(getLocalAddress(), pss->addr, protocols);
    return 0;
}

int msg_client_disconnect(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user) {

    struct per_session_data__ws_client* pss = user;
    fprintf(stdout, "log: Client %s disconnected %d\n", pss->client_name,pss->addr.id);
    ap_remove_node(getLocalAddress(), pss->addr);

    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 10 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    sprintf((char *) p, "0987654321");
    //libwebsockets_broadcast(&protocols[PROTOCOL_COMMOTION_WS], p, 10);
    commotion_broadcast(&protocols[PROTOCOL_COMMOTION_WS], context, p, 10);
}

/**
 * Handle request topology
 * @param context
 * @param wsi
 * @param user
 * @param root
 * @return 
 */
static int msg_req_topology(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root) {

    struct per_session_data__ws_client* pss = user;
    
    
    json_t* msg = make_msg(COMMOTION_MSG_TOPOLOGY_UPDATE,"127.0.0.1",pss->client_ip,ap_root_topology());
    
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

int commotion_ws_callback(struct libwebsocket_context *context,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason,
        void *user, void *in, size_t len) {
    
    int n;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    struct per_session_data__ws_client *pss = user;

    int socket=libwebsocket_get_socket_fd(wsi);
    switch (reason) {

        case LWS_CALLBACK_ESTABLISHED:
            
            libwebsockets_get_peer_addresses(socket, pss->client_name,
                    sizeof (pss->client_name), pss->client_ip, sizeof (pss->client_ip));

            struct sockaddr_in antelope;
            inet_aton(pss->client_ip, &antelope.sin_addr);
            pss->addr.addr = htonl(antelope.sin_addr.s_addr);
            pss->addr.id = socket;

            int ret = ap_add_node(getLocalAddress(), pss->addr, pss->client_name);


            break;

        case LWS_CALLBACK_BROADCAST:
            printf("                                    LWS_CALLBACK_BROADCAST %d %d\n",pss->addr.id,len);
            if (libwebsocket_write(wsi,p, n, LWS_WRITE_TEXT) < 0) {
            }
            
            if (libwebsocket_write(wsi,in, len, LWS_WRITE_TEXT) < 0) {
            }
            break;
        case LWS_CALLBACK_RECEIVE:
            return handle_client_data(context, wsi, user, in, len);
            break;
        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            commotion_handshake_info((struct lws_tokens *) (long) user);
            break;
        case LWS_CALLBACK_CLOSED:
            msg_client_disconnect(context, wsi, user);
            break;
        default:
            break;
    }

    return 0;
}

/*
 * this is just an example of parsing handshake headers, you don't need this
 * in your code unless you will filter allowing connections by the header
 * content
 */

void
commotion_handshake_info(struct lws_tokens *lwst) {
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

static int commotion_broadcast(const struct libwebsocket_protocols *protocol,struct libwebsocket_context *context,unsigned char *buf, size_t len) {
    
    int n,m;
    struct libwebsocket *wsi;
    for (n = 0; n < FD_HASHTABLE_MODULUS; n++) {
        for (m = 0; m < context->fd_hashtable[n].length; m++) {
            wsi = context->fd_hashtable[n].wsi[m];

            if (wsi->mode != LWS_CONNMODE_WS_SERVING)
                continue;

            /*
             * never broadcast to
             * non-established connections
             */
            if (wsi->state != WSI_STATE_ESTABLISHED)
                continue;

            /* only broadcast to guys using
             * requested protocol
             */
            if (wsi->protocol != protocol)
                continue;

            wsi->protocol->callback(context, wsi,
                    LWS_CALLBACK_BROADCAST,
                    wsi->user_space,
                    buf, len);
        }
    }

    return 0;
}