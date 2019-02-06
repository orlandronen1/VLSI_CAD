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
    Vertex* curr_v = root;          // Current Vertex
    Edge* curr_e = starting_edge;   // Current Edge
    Edge* check_e;                  // For looping
    double min_slack= 999999999.0;  // Min slack rising or falling
    double check_slack;             // Slack to check against
    int branches = 1;               // Number of branches left to account for (at least 1 path)
    bool branched = false;          // Whether or not a branch has already been discovered
    bool alt_path = false;          // Whether or not another path has been discovered
    bool comparing_r = rising;      // If comparing rising or falling. Switches every
                                    //  time an inverting gate is encountered.
    int inverting;                  // If current gate is inverting


    while (num_paths < branches)    // While there are paths not fully discovered
    {
        if (pS[num_paths] == NULL)      // If new path, create stack
            pS[num_paths] = new EdgeS;
        else
        {
            // printf("Starting alt path\n");

            curr_e = pS[num_paths]->top();  // Start from top of this stack

            // printf("Alt path start: ");
            // printf(curr_e->Name());
            // printf("\n");

            pS[num_paths]->pop();           // Pop top off to prevent repeat edge
        }

        while (curr_e->NumSources() > 0)
        {
            pS[num_paths]->push(curr_e);    // Push current edge onto the stack
            curr_v = curr_e->FirstSource(); // Update Vertex
            inverting = curr_v->IsInverting();

            // printf("\ncurr_e: ");
            // printf(curr_e->Name());
            // printf("\n");
            // printf("curr_v: ");
            // printf(curr_v->Name());
            // printf("\n");
            // printf("comparing_r: %d\n",comparing_r);
            // printf("inverting: %d\n",inverting);

            // Find minimum slack
            min_slack = 999999999.0;    // Reset min
            check_e = curr_v->FirstInEdge();
            for (int i = 0; i < curr_v->NumInEdges(); i++)
            {
                // printf("check_e: ");
                // printf(check_e->Name());
                // printf("\n");

                if (comparing_r && inverting)
                    check_slack = check_e->Falling().SLACK();
                else if (comparing_r && !inverting)
                    check_slack = check_e->Rising().SLACK();
                else if (!comparing_r && inverting)
                    check_slack = check_e->Rising().SLACK();
                else
                    check_slack = check_e->Falling().SLACK();

                if (check_slack < min_slack)
                    min_slack = check_slack;

                // printf("check_slack: %lf\n",check_slack);
                // printf("min_slack: %lf\n",min_slack);

                check_e = curr_v->NextInEdge();
            }

            // Get next edge
            check_e = curr_v->FirstInEdge();
            for (int i = 0; i < curr_v->NumInEdges(); i++)
            {
                // printf("check_e: ");
                // printf(check_e->Name());
                // printf("\n");

                if (!branched)  // If looking for first available path
                {
                    if (comparing_r && inverting && check_e->Falling().SLACK() == min_slack)
                    {    curr_e = check_e; branched = true;     }
                    else if (comparing_r && !inverting && check_e->Rising().SLACK() == min_slack)
                    {    curr_e = check_e; branched = true;     }
                    else if (!comparing_r && inverting && check_e->Rising().SLACK() == min_slack)
                    {    curr_e = check_e; branched = true;     }
                    else if (!comparing_r && !inverting && check_e->Falling().SLACK() == min_slack)
                    {    curr_e = check_e; branched = true;     }
                }
                else            // If one branch has already been found
                {
                    // printf("Looking for alt branches\n");

                    if (comparing_r && inverting && check_e->Falling().SLACK() == min_slack)
                        alt_path = true;//curr_e = check_e;
                    else if (comparing_r && !inverting && check_e->Rising().SLACK() == min_slack)
                        alt_path = true;//curr_e = check_e;
                    else if (!comparing_r && inverting && check_e->Rising().SLACK() == min_slack)
                        alt_path = true;//curr_e = check_e;
                    else if (!comparing_r && !inverting && check_e->Falling().SLACK() == min_slack)
                        alt_path = true;//curr_e = check_e;

                    if (alt_path)           // If another path is found
                    {
                        // printf("Found alt branch\n");

                        pS[branches] = new EdgeS;
                        *pS[branches] = *pS[num_paths]; // Copy current stack to next slot
                        pS[branches]->push(check_e);    // Push new branch edge onto new stack
                        branches++;         // Increment branch counter
                        alt_path = false;   // Reset flag in case another branch appears
                    }
                }

                check_e = curr_v->NextInEdge();
            }
            
            branched = false;   // Reset flag

            if (inverting)      // Invert if needed
                comparing_r = !comparing_r;
        }

        // printf("Finished a path\n");
        printf("curr_e: ");
        printf(curr_e->Name());
        printf("\n");
        pS[num_paths]->push(curr_e);    // Push final pin onto stack
        num_paths++;    // update # of paths

        // printf("num_paths: %d\n",num_paths);
        // printf("branches: %d\n",branches);
    }

    // printf("final_e: ");
    // printf(curr_e->Name());
    // printf("\n");

    // printf("done\n\n\n");
}

// End
