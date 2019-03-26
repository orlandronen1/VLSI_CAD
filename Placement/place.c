/* Copyright (c) 2002 ACM/SIGDA */

/* ------------------------------------------------------------------------ */
/* INCLUDES								    */
/* ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ------------------------------------------------------------------------ */
/* LOCAL DEFINES                                                            */
/* ------------------------------------------------------------------------ */

#define START_EDGES 2

/* ------------------------------------------------------------------------ */
/* LOCAL TYPE DEFINITIONS                                                   */
/* ------------------------------------------------------------------------ */

typedef struct node
{
    float x;        // x coord
    float y;        // y coord
    int num_edges;  // Number of connections
    int size;       // Size of edges list
    int* edges;     // Connections to other nodes
} node;

/* ------------------------------------------------------------------------ */
/* FUNCTION DEFINITIONS                                                     */
/* ------------------------------------------------------------------------ */

node new_node()
{
    node ret;
    ret.x = 0;
    ret.y = 0;
    ret.num_edges = START_EDGES;
    ret.edges = malloc(sizeof(int) * START_EDGES);

    return ret;
}

void add_edge(node n1, node n2)
{

}


int
main(int argc, char *argv[])
{
    float move_thresh;
    int num_fixed, num_floating, num_edges;

    return EXIT_SUCCESS;
}
