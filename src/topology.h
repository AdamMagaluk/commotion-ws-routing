/* 
 * File:   commotion.h
 * Author: adam
 *
 * Created on July 30, 2012, 10:54 PM
 */

#ifndef COMMOTION_TOPOLOGY_H
#define	COMMOTION_TOPOLOGY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <jansson.h>


#ifdef	__cplusplus
extern "C" {
#endif

    struct Address {
        uint32_t addr;
        int id;
    };

#define LOCAL_HOST_ADDR 0x7F000001
#define FIELD_ROOT_APS "aps"
#define FIELD_ADDR "addr"
#define FIELD_ADDR_TEXT "addrt"
#define FIELD_HOSTNAME "hostname"
#define FIELD_ID "id"
#define FIELD_CLIENTS "clients"
#define FIELD_PROTOCOLS "protocols"


    json_t* ap_root;

    void init_access_points();

    char* ap_dump_topology();

    json_t* ap_root_topology();

    int ap_add_node(struct Address ap, struct Address node, const char* hostname);

    int ap_node_ap(struct Address ap, struct Address node);

    int ap_node_protocols(struct Address ap_addr, struct Address node_node, json_t* protocols);

    int ap_node_exists(struct Address ap, struct Address node);

    int ap_remove_node(struct Address ap, struct Address node);

    int ap_exists(struct Address ap_addr);

    int remove_ap(struct Address ap_addr);

    int add_ap(struct Address ap_addr);


    void display_topology();

    void deref_access_points();

    const char* addr_to_string(uint32_t ap);

    // Checks if they are the same.
    int compare_client(json_t* left, json_t* right);

    struct Tree {
        int a;
    };


#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_TOPOLOGY_H */

