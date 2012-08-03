
#include "commotion.h"

// Check if t is valid msg type
// Return 0 on success, all else is not valid.
inline int is_valid_msg_type(int t){
    switch(t){
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
                    ret=msg_client_connect(context,wsi,user,root);
                    break;
                case COMMOTION_MSG_CLIENT_DISSCONNECTED:
                case COMMOTION_MSG_FORWARD_MSG:
                    return 0;
                case COMMOTION_MSG_REQ_TOPOLOGY:
                    ret=msg_req_topology(context,wsi,user,root);
                    break;
                default:
                    fprintf(stderr, "error: Message type not valid\n");
            }
            
        }
        json_decref(root);
    }
    return ret;
}


static int msg_client_connect(struct libwebsocket_context *context,
        struct libwebsocket *wsi, void *user, json_t *root) {

    struct per_session_data__ws_client* pss = user;
    
    fprintf(stdout, "log: Client connected\n");

    json_t *data, *protocols;
    data = json_object_get(root,CWS_FIELD_MSG_DATA);
    if(!json_is_object(data))
    {
        fprintf(stderr, "error: missing data arg or not object\n");
        return 0;
    }
    
    protocols = json_object_get(data,MSG_REGISTER_FIELD_PROTOCOLS);
    if(!json_is_array(protocols))
    {
        fprintf(stderr, "error: missing protocols in message client connect\n");
        return 0;
    }

    int i = 0;
    for (i = 0; i < json_array_size(protocols); i++) {

        json_t* proto = json_array_get(protocols, i);
        if (!json_is_string(proto)) {
            fprintf(stderr, "error:protocol not a string\n");
            continue;
        }

        const char *proto_name = json_string_value(proto);
        fprintf(stderr, "log:protocol '%s' added.\n", proto_name);
        if (strlen(proto_name) < MAX_PROTOCOl_NAME_SIZE) {
            strcpy(pss->protocols[pss->protocol_len], proto_name);
            pss->protocol_len++;
        }

    }

    return 0;
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
    
    json_t* retRoot = json_array();

    int i;
    for(i = 0; i < pss->protocol_len; i++) {
        json_array_append_new(retRoot, json_string(pss->protocols[i]));
    }

    char *retData = json_dumps(retRoot,JSON_COMPACT);
    if (retData != NULL) {

        unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + sizeof (retData) + LWS_SEND_BUFFER_POST_PADDING];
        unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];

        int n;
        n = sprintf((char *) p, "%s", retData);
        n = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
        if (n < 0) {
            fprintf(stderr, "ERROR writing to socket");
        }
        free(retData);
    }

    
    json_decref(retRoot);
    
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

    switch (reason) {

        case LWS_CALLBACK_ESTABLISHED:
            fprintf(stderr, "callback_dumb_increment: LWS_CALLBACK_ESTABLISHED\n");
            pss->protocol_len=0;
            break;

            /*
             * in this protocol, we just use the broadcast action as the chance to
             * send our own connection-specific data and ignore the broadcast info
             * that is available in the 'in' parameter
             */

        case LWS_CALLBACK_BROADCAST:
            n = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
            if (n < 0) {
                fprintf(stderr, "ERROR writing to socket");
                return 1;
            }
            break;

        case LWS_CALLBACK_RECEIVE:
            return handle_client_data(context, wsi, user, in, len);
            break;
        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            commotion_handshake_info((struct lws_tokens *) (long) user);
            /* you could return non-zero here and kill the connection */
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