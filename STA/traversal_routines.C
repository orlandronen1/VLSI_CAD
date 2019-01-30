/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include "timing_graph.h"
using namespace::std;

void forward_traversal(Library *pL, VertexQ *pQ) 
{
}

void reverse_traversal(Library *pL, VertexQ *pQ) 
{
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
