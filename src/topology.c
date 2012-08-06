#include "topology.h"

void init_access_points() {
    ap_root = json_object();
}

void deref_access_points() {
    json_decref(ap_root);
}

void display_topology() {
    json_t *aps = json_object_get(ap_root,FIELD_ROOT_APS);
    if (!json_is_array(aps)) {
        return;
    }
    int i = 0;
    int j = 0;
    int size = json_array_size(aps);
    for (i = 0; i < size; i++) {
        json_t* ap = json_array_get(aps, i);
        if (json_is_object(ap)) {
            json_t *addr = json_object_get(ap,FIELD_ADDR);
            if (json_is_integer(addr)) {
                printf("Access Point 0x%04x\n", json_integer_value(addr));

                json_t *clients = json_object_get(ap,FIELD_CLIENTS);
                if (json_is_array(clients)) {
                    int size = json_array_size(clients);
                    for (j = 0; j < size; j++) {
                        json_t* node = json_array_get(clients, j);
                        if (json_is_object(node)) {
                            json_t *naddr = json_object_get(node,FIELD_ADDR);
                            if (json_is_integer(naddr)) {
                                printf("        Node 0x%04x\n", json_integer_value(naddr));
                            }
                        }
                    }

                }
            }
        }
    }

}

int remove_ap(const Address ap_addr) {
}

int add_ap(const Address ap_addr) {
    int idx = ap_exists(ap_addr);
    if (idx >= 0) {
        return idx;
    }

    json_t *aps = json_object_get(ap_root,FIELD_ROOT_APS);
    if (!json_is_array(aps)) {
        printf("aps is not an array obj\n");
        return -1;
    }
    json_t *ap = json_object();
    if (ap == NULL) return -2;
    json_t* addr = json_integer(ap_addr);
    if (addr == NULL) return -3;
    json_object_set_new(ap,FIELD_ADDR, addr);
    if (json_array_append_new(aps, ap) != 0) {
        return -4;
    }

    return ap_exists(ap_addr);
}

int ap_exists(const Address ap_addr) {
    json_t *aps = json_object_get(ap_root,FIELD_ROOT_APS);
    if (!json_is_array(aps)) {
        aps = json_array();
        json_object_set_new(ap_root, FIELD_ROOT_APS, aps);
    }
    int i = 0;
    int size = json_array_size(aps);
    for (i = 0; i < size; i++) {
        json_t* ap = json_array_get(aps, i);
        if (json_is_object(ap)) {
            json_t *addr = json_object_get(ap,FIELD_ADDR);
            if (json_is_integer(addr)) {
                if (json_integer_value(addr) == ap_addr) {
                    return i;
                }
            }
        }
    }
    return -1;
}

int ap_add_node(const Address ap_addr, const Address node) {
    int idx = add_ap(ap_addr);
    if (idx < 0) return -1;

    json_t *aps = json_object_get(ap_root,FIELD_ROOT_APS);
    json_t* ap = json_array_get(aps, idx);
    if (json_is_object(ap)) {
        json_t *clients = json_object_get(ap,FIELD_CLIENTS);
        if (!json_is_array(clients)) {
            clients = json_array();
            json_object_set_new(ap,FIELD_CLIENTS, clients);
        }



        json_t *client = json_object();
        if (client == NULL) return -2;
        json_t* addr = json_integer(node);
        if (client == NULL) return -3;
        json_object_set_new(client,FIELD_ADDR, addr);
        if (json_array_append_new(clients, client) != 0) {
            return -4;
        }
        

        return 0;
    }

    return idx;
}

int ap_remove_node(const Address ap, const Address node) {

}

const char* addr_to_string(const Address ap) {

}