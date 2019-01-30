/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include "timing_graph.h"
using namespace::std;

void forward_traversal(Library *pL, VertexQ *pQ) 
{
	/*  
        ****Using a queue****
        ****Want a breadth first traversal**** -> this way get entire level of circuit before going to further gates

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
}

void reverse_traversal(Library *pL, VertexQ *pQ) 
{
    /*  
        ****Using a queue****
        ****Want a breadth first traversal**** -> this way get entire level of circuit before going to further gates. just doing it from the end of the graph to the start.
        
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
      Critical path: search from output to input (highest R/F)
*/

// End
