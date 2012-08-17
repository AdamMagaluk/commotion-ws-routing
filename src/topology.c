#include <string.h>

#include "topology.h"

void init_access_points() {
    ap_root = json_object();
}

void deref_access_points() {
    json_decref(ap_root);
}

char* ap_dump_topology(){
    return json_dumps(ap_root,JSON_COMPACT);
}
json_t* ap_root_topology(){
    return ap_root;
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
                            json_t* hostname = json_object_get(node,FIELD_HOSTNAME);
                            if (json_is_integer(naddr)) {
                                
                                char* buf = addr_to_string(json_integer_value(naddr));
                                const char* hm = json_string_value(hostname);
                                printf("        Node %s %s\n",buf,hm);
                                free(buf);
                            }
                        }
                    }

                }
            }
        }
    }

}

int ap_node_protocols(struct Address ap_addr,struct Address node_addr,json_t* protocols){
    int apidx = ap_exists(ap_addr);
    if(apidx <0){
        return -1;
    }
    int idx = ap_node_exists(ap_addr,node_addr);
    if(idx <0){
        return -2;
    }
    
    if (!json_is_array(protocols)) {
        return -1;
    }
    
    int added =0;
    json_t *aps = json_object_get(ap_root,FIELD_ROOT_APS);
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
            
            json_t* left = json_array_get(protocols,i);
            if(!json_is_string(left)) continue;
            
            int found = 0;
            int j = 0;
            for (j = 0; j < json_array_size(proto_list); j++) {
                json_t* right = json_array_get(proto_list, j);
                if(!json_is_string(right)) continue;
                
                if(strcmp(json_string_value(left),json_string_value(right))==0){
                    found = 1;
                    break;
                }
            }
            
            if(found==0){
                if(json_array_append(proto_list,left)==0)added++;
            }
        }
    }
    
    return added;
}

int remove_ap(struct Address ap_addr) {
    int idx = ap_exists(ap_addr);
    if (idx < 0) return 0;

    json_t *aps = json_object_get(ap_root, FIELD_ROOT_APS);
    return json_array_remove(aps, idx);
}

int add_ap(struct Address ap_addr) {
    int idx = ap_exists(ap_addr);
    if (idx >= 0) {
        return idx;
    }

    json_t *aps = json_object_get(ap_root,FIELD_ROOT_APS);
    if (!json_is_array(aps)) {
        return -1;
    }
    json_t *ap = json_object();
    if (ap == NULL) return -2;
    json_t* addr = json_integer(ap_addr.addr);
    if (addr == NULL) return -3;
    json_t* addrt = json_string(addr_to_string(ap_addr.addr));
    if (addrt == NULL) return -4;
    json_object_set_new(ap,FIELD_ADDR, addr);
    json_object_set_new(ap,FIELD_ADDR_TEXT, addrt);
    if (json_array_append_new(aps, ap) != 0) {
        return -4;
    }

    return ap_exists(ap_addr);
}

int ap_exists(struct Address ap_addr) {
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
                if (json_integer_value(addr) == ap_addr.addr) {
                    return i;
                }
            }
        }
    }
    return -1;
}

int ap_add_node(struct Address ap_addr, struct Address node,const char* hostname) {
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
        json_t* addr = json_integer(node.addr);
        if (addr == NULL) return -3;
        json_t* host = json_string(hostname);
        if (addr == NULL) return -4;
        json_t* addrt = json_string(addr_to_string(node.addr));
        if (addrt == NULL) return -5;
        json_t* idd = json_integer(node.id);
        if (idd == NULL) return -6;
        json_object_set_new(client,FIELD_ADDR, addr);
        json_object_set_new(client,FIELD_HOSTNAME, host);
        json_object_set_new(client,FIELD_ADDR_TEXT, addrt);
        json_object_set_new(client,FIELD_ID, idd);
        if (json_array_append_new(clients, client) != 0) {
            return -4;
        }

        return 0;
    }
    return idx;
}

int ap_node_exists(struct Address ap_addr, struct Address node_addr) {
    int idx = ap_exists(ap_addr);
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

int ap_remove_node(struct Address ap_addr, struct Address node_addr) {
    int idx = ap_exists(ap_addr);
    if (idx < 0) return 0;
    
    json_t *aps = json_object_get(ap_root,FIELD_ROOT_APS);
    json_t* ap = json_array_get(aps, idx);
    if (json_is_object(ap)) {
        json_t *clients = json_object_get(ap, FIELD_CLIENTS);
        if (json_is_array(clients)) {
            int nodeIdx = ap_node_exists(ap_addr,node_addr);
            if(nodeIdx >=0) return json_array_remove(clients, nodeIdx);
        }
    }
    return -1;
}


struct Address return_ap_for_node(struct Address node) {
    struct Address addr;
    addr.addr = LOCAL_HOST_ADDR;
    addr.id = 0; 
    if(ap_node_exists(addr,node) > 0){
       return addr; 
    }else{
        // @todo find ap with node.
        return addr;
    }
}

void addr_struct_to_json(const struct Address node,json_t* addr){
    json_object_set_new(addr, FIELD_ADDR,json_integer( node.addr));
    json_object_set_new(addr, FIELD_ID, json_integer(node.id));
}

int compare_address(struct Address a,struct Address b){
    if( a.addr == b.addr && a.id ==b.id  ) return 1;
    else return 0;
}

const char* addr_to_string(uint32_t ap) {
    
     const int b1=((ap >> (3 * 8)) & 0xff);
     const int b2=((ap >> (2 * 8)) & 0xff);
     const int b3=((ap >> (1 * 8)) & 0xff);
     const int b4=((ap >> (0 * 8)) & 0xff);
     
     char* buf=(char*)malloc(16);;
     sprintf(buf,"%d.%d.%d.%d",b1,b2,b3,b4);
     return buf;
}