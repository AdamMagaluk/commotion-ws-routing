/* 
 * File:   test-topology.c
 * Author: adam
 *
 * Created on August 6, 2012, 8:34 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "topology.h"


/*
 * 
 */
int main(int argc, char** argv) {
    
    topology_init();
    const int maxx=3;
    const int maxy=5;
    int i, j;
    for (i = 0; i < maxx; i++) {
        for (j = 0; j < maxy; j++) {
            struct Address a,b;
            a.addr = i;
            a.id = 0;
            b.addr = j;
            a.id = 0;
            if(topology_add_node(a, b,"test") != 0){
                printf("Exited on %d %d\n",i,j);
            }
        }
    }
    
    for (i = 0; i < maxx; i++) {
        struct Address a,b;
            a.addr = i;
            a.id = 0;
        assert(topology_ap_index(a) == i);
        for (j = 0; j < maxy; j++) {
                 b.addr = j;
            b.id = 0;
            assert(topology_node_index(a,b) == j);
        }
    }
    
    struct Address a,b;
    a.addr = 3;
    a.id = 0;
    b.addr = 1;
    b.id =0;
    assert(topology_ap_index(a) == -1);
    assert(topology_node_index(a,b) == -1);
    b.addr = 5;
    assert(topology_node_index(a,b) == -1);
    
    topology_deref();

    return (EXIT_SUCCESS);
}

