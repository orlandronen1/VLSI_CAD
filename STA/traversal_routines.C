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
        num_paths = # of paths, increment with each branch
        pS = array of stacks 
        starting_edge = 
        rising = false means find falling-delay critical path, true means rising critical path
    */

    // Keep in mind inverting gates!!!
    // Have to track multiple paths somehow

    Vertex* curr_v = root;          // Current Vertex
    Edge* curr_e = starting_edge;   // Current Edge
    Edge* check_e;                  // For looping
    double min_slack= 999999999.0;  // Min slack rising or falling
    int branches = 0;               // Number of branches left to account for

//     do
//     {   
//         printf("1\n");
//         if (pS[num_paths] == NULL)
//             pS[num_paths] = new EdgeS;

//         while (curr_e->NumSources() > 0)    // While not at a pin
//         {
//             printf("2\n");
//             // Get min of available edges
//             check_e = curr_v->FirstInEdge();
//             for (int i = 0; i < curr_v->NumInEdges(); i++, check_e = curr_v->NextInEdge())
//             {
//                 if (rising)
//                 {
//                     if (check_e->Rising().SLACK() < min_slack)
//                         min_slack = check_e->Rising().SLACK();
//                 }
//                 else
//                 {
//                     if (check_e->Falling().SLACK() < min_slack)
//                         min_slack = check_e->Falling().SLACK();
//                 }
//             }
// printf("3\n");
//             // Check # of edges that match min that haven't been traversed, increment branches
//             check_e = curr_v->FirstInEdge();
//             for (int i = 0; i < curr_v->NumInEdges(); i++, check_e = curr_v->NextInEdge())
//             {
//                 if (rising)
//                 {
//                     if (check_e->Rising().SLACK() == min_slack && !check_e->IsReady())
//                         branches++;
//                 }
//                 else
//                 {
//                     if (check_e->Falling().SLACK() == min_slack && !check_e->IsReady())
//                         branches++;
//                 }
//             }
// printf("4\n");
//             // Find next edge to take
//             check_e = curr_v->FirstInEdge();
//             for (int i = 0; i < curr_v->NumInEdges(); i++)
//             {
//                 printf("check1\n");
//                 printf(check_e->Name());
//                 printf("\n");
//                 if (rising)
//                 {
//                     if (check_e->Rising().SLACK() == min_slack && !check_e->IsReady())
//                         goto pushStack;
//                 }
//                 else
//                 {
//                     if (check_e->Falling().SLACK() == min_slack && !check_e->IsReady())
//                         goto pushStack;
//                 }
//                 check_e = curr_v->NextInEdge();
//             }
//  pushStack: 
//             printf("check2\n");
//             printf(check_e->Name());
//             printf("\n");
// printf("5\n");
//             //curr_e = check_e;
//             curr_e->IsReady() = 1;          // Set edge taken as ready
//             printf("6\n");
//             pS[num_paths]->push(curr_e);    // Push current edge to queue
//             printf("7\n");
//             curr_v = curr_e->FirstSource(); // Update curr_v to source of traversed edge
//             printf("8\n");
//             branches--;                     // Decrement branches
//             min_slack = 999999999.0;        // Reset min
//             printf("branches: %d\n",branches);
//         }

//         num_paths++;    // Finished a path, increment number of paths

//     } while (branches > 0);     // While more branches to account for


    while (curr_e->NumSources() > 0)
    {
        if (pS[num_paths] == NULL)
            pS[num_paths] = new EdgeS;
        
        printf(curr_e->Name());
        printf("\n");

        pS[num_paths]->push(curr_e);



        curr_v = curr_e->FirstSource();
        curr_e = curr_v->FirstInEdge();
    }

    printf("done\n");
}

// End
