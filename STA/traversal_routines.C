/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include "timing_graph.h"
using namespace::std;

void forward_traversal(Library *pL, VertexQ *pQ) 
{
    Vertex* curr_v;     // Current Vertex
    Edge* curr_e;       // Current Edge
    TimingInfo r;       // Rising timing info
    TimingInfo f;       // Falling timing info
    double max_r;       // Max rising time to current node
    double max_f;       // Max falling time to current node
    double out_r;       // Rising time for out edges
    double out_f;       // Falling time for out edges

    while (pQ->size() > 0)        // While queue is not empty
    {
        curr_v = pQ->front();     // Current vertex is front of queue
        max_r = 0.0;            // Reset maxes
        max_f = 0.0;

        // Get max rising and falling times coming into this Vertex
        curr_e = curr_v->FirstInEdge();
        // For all in edges
        for (int i = 0; i < curr_v->NumInEdges(); i++, curr_e = curr_v->NextInEdge())
        {
            // Get TimingInfo for curr_e
            r = curr_e->Rising();
            f = curr_e->Falling();

            // Check for higher ATs
            if (r.AT() > max_r)
                max_r = r.AT();
            if (f.AT() > max_f)
                max_f = f.AT();
        }

        // Get Vertex's characteristics
        int type = curr_v->TheCell();
        double cell_r, cell_f;
        pL->QueryCell(type, EDGE_R, cell_r);
        pL->QueryCell(type, EDGE_F, cell_f);

        if (curr_v->IsInverting())   // If current vertex is inverting
        {
            out_r = max_f + cell_r;
            out_f = max_r + cell_f;
        }
        else    // Else, current vertex is not inverting
        {
            out_r = max_r + cell_r;
            out_f = max_f + cell_f;
        }

        // Update AT for all out edges
        curr_e = curr_v->FirstOutEdge();
        for (int i = 0; i < curr_v->NumOutEdges(); i++, curr_e = curr_v->NextOutEdge())
        {
            r = curr_e->Rising();
            r.AT() = out_r;
            f = curr_e->Falling();
            f.AT() = out_f;
            // Mark edge as ready
            curr_e->IsReady() = 1;
        }

        // Check next vertices. If they have NumInEdgesReady() == NumInEdges(), add to pQ
        curr_e = curr_v->FirstOutEdge();
        for (int i = 0; i < curr_v->NumOutEdges(); i++, curr_e = curr_v->NextOutEdge())
        {
            Vertex* out_v = curr_e->FirstTarget();
            for (int j = 0; j < curr_e->NumTargets(); j++, out_v = curr_e->NextTarget())
            {
                if (out_v->NumInEdges() == out_v->NumInEdgesReady())
                    pQ->push(out_v);
            }
        }

        pQ->pop();                    // Pop top of queue
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
