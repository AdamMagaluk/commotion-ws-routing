#include <string.h>

#include "topology.h"

/**
 * Initializes the root object, should be called at begening of program.
 */
void topology_init() {
    ap_root = json_object();
}

/**
 * Frees memory used by topology, called at end of execution.
 */
void topology_deref() {
    json_decref(ap_root);
}

/**
 * Api call to return the root topology node.
 * @return 
 */
json_t* topology_root() {
    return ap_root;
}

/**
 * Adds the given access point
 * @param ap_addr
 * @return 
 */
int topology_add_ap(const struct Address ap_addr) {
    int idx = topology_ap_index(ap_addr);
    if (idx >= 0) {
        return idx;
    }

    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    if (!json_is_array(aps)) {
        return -1;
    }
    json_t *ap = json_object();
    if (ap == NULL) return -2;
    json_t* addr = json_integer(ap_addr.addr);
    if (addr == NULL) return -3;
    json_t* addrt = json_string(addr_to_string(ap_addr.addr));
    if (addrt == NULL) return -4;
    json_object_set_new(ap, FIELD_ADDR, addr);
    json_object_set_new(ap, FIELD_ADDR_TEXT, addrt);
    if (json_array_append_new(aps, ap) != 0) {
        return -4;
    }

    return topology_ap_index(ap_addr);
}

/**
 * Returns the index of the access point
 * @param ap_addr
 * @return -1 if not found, index otherwise
 */
int topology_ap_index(const struct Address ap_addr) {
    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    if (!json_is_array(aps)) {
        aps = json_array();
        json_object_set_new(ap_root, FIELD_ROOT_APS, aps);
    }
    int i = 0;
    int size = json_array_size(aps);
    for (i = 0; i < size; i++) {
        json_t* ap = json_array_get(aps, i);
        if (json_is_object(ap)) {
            json_t *addr = json_object_get(ap, FIELD_ADDR);
            if (json_is_integer(addr)) {
                if (json_integer_value(addr) == ap_addr.addr) {
                    return i;
                }
            }
        }
    }
    return -1;
}

/**
 * Removes access point
 * @param ap_addr
 * @return 0 on success
 */
int topology_remove_ap(const struct Address ap_addr) {
    int idx = topology_ap_index(ap_addr);
    if (idx < 0) return 0;

    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    return json_array_remove(aps, idx);
}

/**
 * Adds a Node with the given hostname to the supplied access point.
 * @param ap - Acccess point to add node to.
 * @param node - Node to add.
 * @param hostname - Hostname of node
 * 
 * @return 0 on success
 */
int topology_add_node(const struct Address ap_addr, const struct Address node, const char* hostname) {
    int idx = topology_add_ap(ap_addr);
    if (idx < 0) return -1;

    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    json_t* ap = json_array_get(aps, idx);
    if (json_is_object(ap)) {
        json_t *clients = json_object_get(ap, FIELD_CLIENTS);
        if (!json_is_array(clients)) {
            clients = json_array();
            json_object_set_new(ap, FIELD_CLIENTS, clients);
        }
        json_t *client = json_object();
        if (client == NULL) return -2;
        json_t* addr = json_integer(node.addr);
        if (addr == NULL) return -3;
        json_t* host = json_string(hostname);
        if (addr == NULL) return -4;
        json_t* addrt = json_string(addr_to_string(node.addr));
        if (addrt == NULL) return -5;
        json_t* idd = json_integer(node.id);
        if (idd == NULL) return -6;
        json_object_set_new(client, FIELD_ADDR, addr);
        json_object_set_new(client, FIELD_HOSTNAME, host);
        json_object_set_new(client, FIELD_ADDR_TEXT, addrt);
        json_object_set_new(client, FIELD_ID, idd);
        if (json_array_append_new(clients, client) != 0) {
            return -4;
        }
        return 0;
    } else {
        return -2;
    }
}

/**
 * Updates the protocol of hte given node to the json array of protocols
 *  - If protocols already exists adds any new protocols.
 *  - If no protocols for the node exists object is created and all are added.
 * 
 * @param ap_addr - Access point of node
 * @param node_node - Node
 * @param protocols - Json array of protocols
 * @return 0 on success
 */
int topology_update_node_apps(const const struct Address ap_addr, const const struct Address node_addr, json_t* protocols) {
    int apidx = topology_ap_index(ap_addr);
    if (apidx < 0) {
        return -1;
    }
    int idx = topology_node_index(ap_addr, node_addr);
    if (idx < 0) {
        return -2;
    }

    if (!json_is_array(protocols)) {
        return -3;
    }

    int added = 0;
    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    json_t* ap = json_array_get(aps, apidx);
    if (json_is_object(ap)) {
        json_t *clients = json_object_get(ap, FIELD_CLIENTS);
        json_t* client = json_array_get(clients, idx);

        json_t *proto_list = json_object_get(client, FIELD_PROTOCOLS);
        if (!json_is_array(proto_list)) {
            json_object_set(client, FIELD_PROTOCOLS, protocols);
            return json_array_size(protocols);
        }
        int i = 0;
        for (i = 0; i < json_array_size(protocols); i++) {

            json_t* left = json_array_get(protocols, i);
            if (!json_is_string(left)) continue;

            int found = 0;
            int j = 0;
            for (j = 0; j < json_array_size(proto_list); j++) {
                json_t* right = json_array_get(proto_list, j);
                if (!json_is_string(right)) continue;

                if (strcmp(json_string_value(left), json_string_value(right)) == 0) {
                    found = 1;
                    break;
                }
            }

            if (found == 0) {
                if (json_array_append(proto_list, left) == 0)added++;
            }
        }
    }

    return added;
}

/**
 * Returns the index of the node if it exists in the access points client
 * list.
 * @param access point
 * @param node
 * @return index of client list, -1 if not  found. 
 */
int topology_node_index(const struct Address ap_addr, const struct Address node_addr) {
    int idx = topology_ap_index(ap_addr);
    if (idx < 0) return -1;

    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    json_t* ap = json_array_get(aps, idx);
    if (json_is_object(ap)) {
        json_t *clients = json_object_get(ap, FIELD_CLIENTS);
        if (json_is_array(clients)) {
            int i = 0;
            int size = json_array_size(clients);
            for (i = 0; i < size; i++) {
                json_t* node = json_array_get(clients, i);
                if (json_is_object(node)) {
                    json_t *addr = json_object_get(node, FIELD_ADDR);
                    json_t *ID = json_object_get(node, FIELD_ID);
                    if (json_is_integer(addr)) {
                        if (json_integer_value(addr) == node_addr.addr && json_integer_value(ID) == node_addr.id) {
                            return i;
                        }
                    }
                }
            }
        }
    }
    return -1;

}

/**
 * Returns the access point that has the node given.
 *  @todo potential for errors.
 * 
 * @param node 
 * @return 
 */
struct Address topology_return_ap_for_node(const struct Address node) {
    struct Address addr;
    addr.addr = LOCAL_HOST_ADDR;
    addr.id = 0;
    if (topology_node_index(addr, node) > 0) {
        return addr;
    } else {
        // @todo find ap with node.
        return addr;
    }
}

/**
 * Removes the node from the access point.
 * @param ap
 * @param node
 * @return 0 on success
 */
int topology_remove_node(const struct Address ap_addr, const struct Address node_addr) {
    int idx = topology_ap_index(ap_addr);
    if (idx < 0) return 0;

    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    json_t* ap = json_array_get(aps, idx);
    if (json_is_object(ap)) {
        json_t *clients = json_object_get(ap, FIELD_CLIENTS);
        if (json_is_array(clients)) {
            int nodeIdx = topology_node_index(ap_addr, node_addr);
            if (nodeIdx >= 0) return json_array_remove(clients, nodeIdx);
        }
    }
    return -1;
}

/**
 * Iterates overall aps, and runs the user specified callback on it.
 * @param callback
 */
void topology_iterate_ap(void(*callback)(const json_t* ap)) {
    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    if (json_is_array(aps)) {
        int i;
        for (i = 0; i < json_array_size(aps); i++) {
            json_t *ap;
            ap = json_array_get(aps, i);
            if (!json_is_object(ap)) {
                fprintf(stderr, "topology_iterate_ap : ap %d not a object\n", i + 1);
                continue;
            }
            // Run user callback on ap
            callback(ap);
        }
    }
}




/**
 * Sets the address fields to the given address.
 * @param node
 * @param addr
 */

void addr_struct_to_json(const const struct Address node, json_t* addr) {
    if (!json_is_object(addr)) return;
    json_object_set_new(addr, FIELD_ADDR, json_integer(node.addr));
    json_object_set_new(addr, FIELD_ID, json_integer(node.id));
}

/**
 * Compares to address structures
 * Return 1 if equal
 */
int topology_compare_address(const struct Address a, const struct Address b) {
    if (a.addr == b.addr && a.id == b.id) return 1;
    else return 0;
}

const char* addr_to_string(uint32_t ap) {
     
    const int b1 = ((ap >> (3 * 8)) & 0xff);
    const int b2 = ((ap >> (2 * 8)) & 0xff);
    const int b3 = ((ap >> (1 * 8)) & 0xff);
    const int b4 = ((ap >> (0 * 8)) & 0xff);

    char* buf = (char*) malloc(16);
    ;
    sprintf(buf, "%d.%d.%d.%d", b1, b2, b3, b4);
    return buf;
}

 int string_to_addr(const char* addr, uint32_t* addrint) {
    struct sockaddr_in antelope;
    int ret = inet_aton(addr, &antelope.sin_addr); // store IP in antelope
    *addrint = antelope.sin_addr.s_addr;
    return ret;
}