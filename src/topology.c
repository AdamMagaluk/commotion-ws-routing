#include "topology.h"

void init_access_points() {
    ap_root = json_object();
}

void deref_access_points() {
    json_decref(ap_root);
}

void display_topology() {
    
}

int ap_exists(const Address ap){
   json_t *mt = json_object_get(ap_root,"aps");
   
}

int ap_add_node(const Address ap, const Address node) {
   
}

int ap_remove_node(const Address ap, const Address node) {

}

const char* addr_to_string(const Address ap) {
    
}