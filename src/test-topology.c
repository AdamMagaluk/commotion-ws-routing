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

void test(char* a, char* b){
    printf("A:%s B:%s\n",a,b);
}

/*
 * 
 */
int main(int argc, char** argv) {
    char a[128]="adasd";
    char b[128]="asdasd";
    
    test(a,b);
    
    return 0;
    
    init_access_points();
    const int maxx=3;
    const int maxy=5;
    int i, j;
    for (i = 0; i < maxx; i++) {
        for (j = 0; j < maxy; j++) {
            if(ap_add_node(i, j,"test") != 0){
                printf("Exited on %d %d\n",i,j);
            }
        }
    }
    
    for (i = 0; i < maxx; i++) {
        assert(ap_exists(i) == i);
        for (j = 0; j < maxy; j++) {
            assert(ap_node_exists(i,j) == j);
        }
    }
    
    
    assert(ap_exists(0x0003) == -1);
    assert(ap_node_exists(0x0003,1) == -1);
    assert(ap_node_exists(0x0000,5) == -1);
    
    display_topology();
    deref_access_points();

    return (EXIT_SUCCESS);
}

