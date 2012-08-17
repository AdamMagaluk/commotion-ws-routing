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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef	__cplusplus
extern "C" {
#endif

    struct Address {
        uint32_t addr;
        int id;
    };

// @todo get rid of this
#define LOCAL_HOST_ADDR 0x7F000001
    

#define FIELD_ROOT_APS "aps"
#define FIELD_ADDR "addr"
#define FIELD_ADDR_TEXT "addrt"
#define FIELD_HOSTNAME "hostname"
#define FIELD_ID "id"
#define FIELD_CLIENTS "clients"
#define FIELD_PROTOCOLS "protocols"

    // Root for topology, all data is added to this base object
    json_t* ap_root;

    /**
     * Initializes the root object, should be called at begening of program.
     */
    extern void topology_init();

    /**
     * Frees memory used by topology, called at end of execution.
     */
    extern void topology_deref();

    /**
     * Api call to return the root topology node.
     * @return 
     */
    extern json_t* topology_root();
    
    /**
     * Adds the given access point
     * @param ap_addr
     * @return 
     */
    extern int topology_add_ap(const struct Address ap_addr);

    /**
     * Returns the index of the access point
     * @param ap_addr
     * @return -1 if not found, index otherwise
     */
    extern int topology_ap_index(const struct Address ap_addr);
    
    /**
     * Removes access point
     * @param ap_addr
     * @return 0 on success
     */
    extern int topology_remove_ap(const struct Address ap_addr);
    
    /**
     * Adds a Node with the given hostname to the supplied access point.
     * @param ap - Acccess point to add node to.
     * @param node - Node to add.
     * @param hostname - Hostname of node
     * 
     * @return 0 on success
     */
    extern int topology_add_node(struct Address ap, struct Address node, const char* hostname);

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
    extern int topology_update_node_apps(const struct Address ap_addr,const  struct Address node_node, json_t* apps);

    /**
     * Returns the index of the node if it exists in the access points client
     * list.
     * @param access point
     * @param node
     * @return index of client list, -1 if not  found. 
     */
    extern int topology_node_index(const struct Address ap, const struct Address node);

    /**
     * Returns the access point that has the node given.
     *  @todo potential for errors.
     * 
     * @param node 
     * @return 
     */
    extern struct Address topology_return_ap_for_node(const struct Address node);

    /**
     * Removes the node from the access point.
     * @param ap
     * @param node
     * @return 0 on success
     */
    extern int topology_remove_node(const struct Address ap, const struct Address node);

    
    
    
    
    /**
     * Compares to address structures
     * Return 1 if equal
     */
    int topology_compare_address(const struct Address a,const struct Address b);

    /**
     * Sets the address fields to the given address.
     * @param node
     * @param addr
     */
    void addr_struct_to_json(const struct Address node, json_t* addr);
    
    /**
     * Returns string ip address of 32 bit ip
     * @param ap
     * @return 
     */
    const char* addr_to_string(const uint32_t ap);
    static int string_to_addr(const char* addr, uint32_t* addrint);


#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_TOPOLOGY_H */

