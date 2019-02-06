/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include "timing_graph.h"
using namespace::std;

void forward_traversal(Library *pL, VertexQ *pQ) 
{
    Vertex* curr_v;     // Current Vertex
    Edge* curr_e;       // Current Edge
    double max_r;       // Max rising time to current node
    double max_f;       // Max falling time to current node
    double out_r;       // Rising time for out edges
    double out_f;       // Falling time for out edges

    while (pQ->size() > 0)        // While queue is not empty
    {
        curr_v = pQ->front();     // Current vertex is front of queue
        max_r = 0.0;              // Reset maxes
        max_f = 0.0;

        // Get max rising and falling times coming into this Vertex
        curr_e = curr_v->FirstInEdge();
        // For all in edges
        for (int i = 0; i < curr_v->NumInEdges(); i++, curr_e = curr_v->NextInEdge())
        {
            // Check for higher ATs
            if (curr_e->Rising().AT() > max_r)
                max_r = curr_e->Rising().AT();
            if (curr_e->Falling().AT() > max_f)
                max_f = curr_e->Falling().AT();
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
            curr_e->Rising().AT() = out_r;
            curr_e->Falling().AT() = out_f;

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

        pQ->pop();  // Pop top of queue
    }    
}

void reverse_traversal(Library *pL, VertexQ *pQ) 
{
    Vertex* curr_v;     // Current Vertex
    Edge* curr_e;       // Current Edge
    double min_r;       // Min rising time to current node
    double min_f;       // Min falling time to current node
    double in_r;        // Rising time for in edges
    double in_f;        // Falling time for in edges

    while (pQ->size() > 0)  // While queue is not empty
    {
        curr_v = pQ->front();   // Current vertex is front of queue
        min_r = 999999999.0;    // Reset mins to arbitrarily large value
        min_f = 999999999.0;

        // Get min rising and falling times coming from this Vertex
        curr_e = curr_v->FirstOutEdge();
        // For all out edges
        for (int i = 0; i < curr_v->NumOutEdges(); i++, curr_e = curr_v->NextOutEdge())
        {
            // Check for higher ATs
            if (curr_e->Rising().RT() < min_r)
                min_r = curr_e->Rising().RT();
            if (curr_e->Falling().RT() < min_f)
                min_f = curr_e->Falling().RT();
        }

        // Get Vertex's characteristics
        int type = curr_v->TheCell();
        double cell_r, cell_f;
        pL->QueryCell(type, EDGE_R, cell_r);
        pL->QueryCell(type, EDGE_F, cell_f);

        if (curr_v->IsInverting())   // If current vertex is inverting
        {
            in_r = min_f - cell_f;
            in_f = min_r - cell_r;
        }
        else    // Else, current vertex is not inverting
        {
            in_r = min_r - cell_r;
            in_f = min_f - cell_f;
        }

        // Update RT for all in edges
        curr_e = curr_v->FirstInEdge();
        for (int i = 0; i < curr_v->NumInEdges(); i++, curr_e = curr_v->NextInEdge())
        {
            // Avoid overwriting more critical results
            if (curr_e->Rising().RT() > in_r)
                curr_e->Rising().RT() = in_r;
            if (curr_e->Falling().RT() > in_f)
                curr_e->Falling().RT() = in_f;

            // Mark edge as ready
            curr_e->IsReady() = 1;
        }

        // Check next vertices. If they have NumInEdgesReady() == NumInEdges(), add to pQ
        curr_e = curr_v->FirstInEdge();
        for (int i = 0; i < curr_v->NumInEdges(); i++, curr_e = curr_v->NextInEdge())
        {
            Vertex* in_v = curr_e->FirstSource();
            for (int j = 0; j < curr_e->NumSources(); j++, in_v = curr_e->NextSource())
            {
                if (in_v->NumOutEdges() == in_v->NumOutEdgesReady())
                    pQ->push(in_v);
            }
        }

        pQ->pop();  // Pop top of queue
    }
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
