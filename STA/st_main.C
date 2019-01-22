/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "timing_graph.h"

using namespace::std;

extern void forward_traversal(Library *pL, VertexQ *pQ);
extern void reverse_traversal(Library *pL, VertexQ *pQ);
extern void traversal_critical_path(EdgeS **ps, int &num_paths, Vertex *root,
				    Edge *starting_edge, bool rising);

/* main routine 
   One command-line arguments: <netlist>

   DO NOT CHANGE ANY PRINTING ROUTINES IN THE MAIN FUNCTION!!!

 */
int main(int argc, char *argv[]) 
{
  VertexQ Q;        // queue of vertices to be processed
  TimingGraph G;    // timing graph to work on
  Library L=Library(1.0, 85);   // cell library to use
  ifstream F; // FILE *F;
  
  if ( argc < 2 ) {
    fprintf(stderr, "Usage: %s <netlist>\n", argv[0]);
    return EXIT_FAILURE;
  }

  if ( argc == 2 ) {
    // F=fopen("library.dat","r");
    try {
      F.open("library.dat", ios::in);
      if (!F) throw std::runtime_error("Unable to open lib file");
    } catch(exception& e) {
      cout << e.what() << endl;
      return EXIT_FAILURE;
    }
    L.LoadLibrary(F);   // load the cell library
    if(F.eof()) F.clear();
    F.close(); // fclose(F);

    // F=fopen(argv[1], "r");
    try {
      F.open(argv[1], ios::in);
      if (!F) throw std::runtime_error("Unable to open netlist file");
    } catch(exception& e) {
      cout << e.what() << endl;
      return EXIT_FAILURE;
    }
    G.LoadGraph(F, &L);    // load the timing graph
    if(F.eof()) F.clear();
    F.close(); // fclose(F);
  }

  /* Populate the queue with those vertices whose in_edges are ready
     i.e, those solely connected to the PIs */
  Vertex *v1;
  for (v1=G.FirstVertex(); v1; v1=G.NextVertex()) {
    if (v1->NumInEdges() == v1->NumInEdgesReady())  Q.push(v1);
  }

  /* Forward traversal to calculate AT */
  forward_traversal(&L, &Q);

  /* Reset _is_ready flag of each edge so that another traversal can be 
     applied */
  G.Reset();

  /* Populate the queue with those vertices whose out_edges are PO pins.
     Note that is this a slightly different method compared to the method 
     used before forward_traversal is applied. */
  for (v1=G.FirstVertex(); v1; v1=G.NextVertex()) {
    Edge *e=v1->FirstOutEdge();
    if ( e->IsPin() ) {
      Pin *p = (Pin*)e;
      if (p->Type() == PO ) Q.push(v1);
    }
  }

  /* Reverse traversal to propogate RT */
  reverse_traversal(&L, &Q);

  /* Walk through each edge to calculate the slacks */
  for (Edge *ee=G.FirstEdge(); ee; ee=G.NextEdge()) {
    ee->Rising().SLACK()  = ee->Rising().RT()  - ee->Rising().AT();
    ee->Falling().SLACK() = ee->Falling().RT() - ee->Falling().AT();
  }

  /* Printing - don't change */
  for (Pin* p=G.FirstPin(PO); p; p=G.NextPin(PO)) {
    printf("AT PO: %s -> %.1f/%.1f\n", p->Name(), p->Rising().AT(), 
	   p->Falling().AT() );
  }

  for (Pin* p=G.FirstPin(PI); p; p=G.NextPin(PI)) {
    printf("RT PI: %s -> %.1f/%.1f\n", p->Name(), p->Rising().RT(), 
	   p->Falling().RT() );
  }

  for (Edge *ee=G.FirstEdge(); ee; ee=G.NextEdge()) {
    printf("SLACK %s: %+.0f/%+.0f\n", ee->Name(), ee->Rising().SLACK(),
	   ee->Falling().SLACK());
  }

  /* Populate the queue with output edges only */
  for (v1=G.FirstVertex(); v1; v1=G.NextVertex()){
    Edge *e=v1->FirstOutEdge();
    if ( e->IsPin() ) {
      Pin *p = (Pin*)e;
      if (p->Type()==PO ) Q.push(v1);
    }
  }

  Edge *e, *pe;
  EdgeS *array_critical_paths[10000];
  int num_paths=0;
  while ( Q.size() > 0 ) {
    v1 = Q.front();
    Q.pop();
    
    EdgeS *es;

    // Assume there is only one out edge, which is always true for our circuits
    pe = v1->FirstOutEdge(); 

    /* Given a starting vertex connected to PO, reverse traversal to find the
       most critical path and store them in the reversed order in the stack */
    traversal_critical_path(&(array_critical_paths[0]), num_paths, v1,
			    pe, false); // falling edge

    /* Print the content in the stack */
    for (int i=0; i<num_paths; i++) {
      es = array_critical_paths[i];
      e = es->top();
      es->pop();
      printf("CRIT PATH %s(F): ", pe->Name());
      printf("%s", e->Name());
      while ( es->size() > 0 ) {
	e = es->top();
	es->pop();
	printf("->%s", e->Name());
      }
      printf("\n");
    }
    // clean the allocated memory
    for (int i=0; i<num_paths; i++) {
      es = array_critical_paths[i];
      delete es;
      array_critical_paths[i] = 0;
    }

    /* Repeat, but this time for rising edge */
    num_paths = 0;
    traversal_critical_path(&(array_critical_paths[0]), num_paths, v1,
			    pe, true); 

    /* Print the content in the stack */
    for (int i=0; i<num_paths; i++) {
      es = array_critical_paths[i];
      e = es->top();
      es->pop();
      printf("CRIT PATH %s(R): ", pe->Name());
      printf("%s", e->Name());
      while ( es->size() > 0 ) {
	e = es->top();
	es->pop();
	printf("->%s", e->Name());
      }
      printf("\n");
    }
    for (int i=0; i<num_paths; i++) {
      es = array_critical_paths[i];
      delete es;
      array_critical_paths[i] = 0;
    }

  }
  return EXIT_SUCCESS;
}
