#include "topology.h"

void init_access_points() {
    ap_root = json_object();
}

void deref_access_points() {
    json_decref(ap_root);
}

void display_topology() {
    
}

int ap_exists(const Address ap_addr) {
    json_t *aps = json_object_get(ap_root, "aps");
    if (!json_is_array(aps)) {
        aps = json_array();
    }
    int i=0;
    int size = json_array_size(aps);
    
    for(i=0;i<size;i++){
        json_t* ap = json_array_get(aps, i);
        if (json_is_object(aps)) {
            json_t *addr = json_object_get(ap, "addr");
            if (json_is_integer(addr)) {
                if (json_integer_value(addr) == ap_addr) {
                    return i;
                }
            }    
        }
    }
    return -1;
}

int ap_add_node(const Address ap, const Address node) {
   
}

int ap_remove_node(const Address ap, const Address node) {

}

const char* addr_to_string(const Address ap) {
    
}