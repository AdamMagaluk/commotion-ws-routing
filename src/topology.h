/* 
 * File:   commotion.h
 * Author: adam
 *
 * Created on July 30, 2012, 10:54 PM
 */

#ifndef COMMOTION_H
#define	COMMOTION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <jansson.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define FIELD_ROOT_APS "aps"
#define FIELD_ADDR "addr"
#define FIELD_CLIENTS "clients"
    
    typedef uint32_t Address;
    
    json_t* ap_root;

    void init_access_points();
    
    int ap_add_node(const Address ap,const Address node);
    
    int ap_remove_node(const Address ap,const Address node);
    
    int ap_exists(const Address ap_addr);
    
    
    int remove_ap(const Address ap_addr);
    int add_ap(const Address ap_addr);
    
    void display_topology();
    
    void deref_access_points();
    
    const char* addr_to_string(const Address ap);
    
#ifdef	__cplusplus
}
#endif

#endif	/* COMMOTION_H */

