/* 
 * File:   test-topology.c
 * Author: adam
 *
 * Created on August 6, 2012, 8:34 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include "topology.h"

/*
 * 
 */
int main(int argc, char** argv) {
    init_access_points();
    /*
        int i,j;
        for( i=0;i<1;i++){
            for( j=0;j<2;j++){
            printf("Added:%d\n",ap_add_node(i+10,j+1));
            }
        }
    
     */
    printf("Added:%d\n", ap_add_node(4, 0x01234567));
    printf("Added:%d\n", ap_add_node(4, 0x01234568));
    display_topology();
    deref_access_points();


    return (EXIT_SUCCESS);
}

