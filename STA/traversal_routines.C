/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include "timing_graph.h"
using namespace::std;

void forward_traversal(Library *pL, VertexQ *pQ) 
{
	/*  
        Want a breadth first traversal -> this way get entire level of circuit before going to further gates
        Queue starts w/ vertices whose in_edges are only connected to PIs

        Check current node
            If current gate is non-inverting
                Get previous nodes' max rising time and add to current's rising time for AT
                Get previous nodes' max falling time and add to current's falling time for AT
            Else
                Get previous nodes' max falling time and add to current's rising time for AT
                Get previous nodes' max rising time and add to current's falling time for AT

        Check what can go in the queue -> all inputs must have AT calculated already
            If all input gates have their ATs calculated, add next gate to queue
            Else, calculate AT for input nodes
	*/

    Vertex curr_v;                  // Current vertex

    while (Q.size() > 0)            // While queue is not empty
    {
        curr_v = Q.front();         // Current vertex is front of queue

        if (curr_v.IsInverting())   // Check if current vertex is inverting
        {
            // Add max of prev edges rising to curr vert's falling for out edge's falling
            // Add max of prev edges falling to curr vert's rising for out edge's rising
        }
        else    // Else, current vertex is not inverting
        {
            // Add max of prev edges rising to curr vert's falling for out edge's falling
            // Add max of prev edges falling to curr vert's rising for out edge's rising
        }

        // Check next vertices. If they have NumInEdgesReady() == NumInEdges(), add to Q

        Q.pop();                    // Pop top of queue
    }    
}

void reverse_traversal(Library *pL, VertexQ *pQ) 
{
    /*  
        ****Using a queue****
        ****Want a breadth first traversal**** -> this way get entire level of circuit before going to further gates. just doing it from the end of the graph to the start.
        Queue starts w/ vertices whose out_edges are POs

        Output nodes should be given their RT based on circuit spec, probably just do a check for if curr node is output or not
            If it is, just set RT to spec

        RT of a node depends on if OUTPUT gate(s) are inverting/non-inverting
        Iterate over all output (next) gates
            Keep track of min
            If next gate is non-inverting
                RT rising = next gate's rising RT - next gate's rising delay
                RT falling = next gate's falling RT - next gate's falling delay
            Else
                RT rising = next gate's falling RT - next gate's falling delay
                RT falling = next gate's rising RT - next gate's rising delay

        If the previous node's outputs all have their RTs calculated, add previous node to queue
            Need to keep an eye out for gates that drive >1 output
    */
}

/* Compute all critical paths */
void traversal_critical_path(EdgeS **pS, int &num_paths, Vertex *root, 
                             Edge *starting_edge, bool rising) 
{
    /*
        root is a vertex connected to PO
        rising = false means find falling-delay critical path, true means rising critical path
    */
}

/* Notes

    When dealing w/ non-inverting gates, add rising to rising and falling to falling
    When dealing w/ inverting gates, add rising to next falling and falling to next rising

    Arrival time (AT) = time elapsed for signal to arrive at certain point
    Required time (RT) = latest time at which a signal can arrive to guarantee timing of output
    Slack = RT - AT

    Critical path is a path b/w input and output w/ maximum delay. Can have different paths for
      rising/falling

    Critical path method:
        AT: traverse from input to output
        RT: traverse from output to input
        Slack: RT - AT
        Critical path: search from output to input -> follow the slack
            Path of lowest slack = critical path

    Edge types (gates) enum vals: 
        inverting are <= 8, non-inverting are >8 (handled in vertex)
    Edges have rising and falling timing information (AT, RT, Slack)
    Edges have list of sources and list of targets
    Edges have int _is_ready

    Vertex has list of in edges and out edges
    Vertex DOESN'T have timing info
    Vertex has field _cell, which is index of cell characteristics in library
    Vertex tells if gate is inverting or not w/ IsInverting()
    Can check how many in or out edges are ready -> use for checking if ready to add to queue

    Pins are treated as edges


    st_main process:
    Populate queue w/ vertices whose in_edges are solely connected to PIs
    forward_traversal(L = cell library, Q = queue w/ vertices)
    Reset ready flag of each edge
    Populate queue w/ vertices whose out_edges are POs
    reverse_traversal(L,Q)
    Calculate slacks
    Populate queue w/ output edges only
    While there are still vertices in the queue:
        traversal_critical_path(EdgeS = stack of edges for critical path,)
*/

// End
