/* Copyright (c) 2004 ACM/SIGDA
   Prepared by Valavan Manohararajah, vmanohar@altera.com
   Modifications by Florian Krohm, fkrohm@us.ibm.com
*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_GATE_TYPES	10
#define MAX_GATE_INPUTS	2
#define MAX_CELL_TYPES	20
#define MAX_DAG_NODES	100000

/* Type definitions for gate types/cells/nodes: */
typedef struct node
{
  int _id;    // ID for DAG
  int _type;  // Type of cell, i.e. ID of cell
  int _size;  // Size of _out for malloc/realloc purposes
  int _num_inputs;
  int _num_outputs;
  int _delay; // Best delay found for this node
  int* _in;   // Input nodes
  int* _out;  // Output nodes
} node;

typedef struct simple_cell
{
  int _id;
  int _num_inputs;
  int _delay;
} simple;

typedef struct complex_cell
{
  int _id;
  int _u;
  int _v;
  int _d;
  int _e;
  int _num_inputs;
} complex;

typedef struct cell_library
{
  int _num_simple;
  int _num_complex;
  simple* _s;
  complex* _c;
} cell_lib;

// Just make an array of node for the DAG
// typedef struct DAG
// {
//   int _size;
//   node* _PI;
//   node* _PO;
// } DAG;

// typedef struct forest
// {
//   int _size;
//   DAG* _dags;
// } forest;


/************************************************************************
****************************    Functions    ****************************
************************************************************************/

// Creates a new node 
node new_node(int id, int type)
{
  node ret;
  ret._id = id;
  ret._type = type;
  
  ret._num_inputs = 0;
  ret._num_outputs = 0;
  ret._delay = INT_MAX;   // Initialize at max value
  ret._in = malloc(sizeof(int) * MAX_GATE_INPUTS);

  ret._size = MAX_GATE_INPUTS; // Pick a minimum size to alloc
  ret._out = malloc(sizeof(int) * MAX_GATE_INPUTS);
  
  return ret;
}

// Adds a fanout to a node
void fanout_node(node* n, int out)
{
  if (n->_num_outputs == n->_size)  // Need to reallocate
  {
    n->_size *= 2;  // Double size
    int* buf = realloc(n->_out, sizeof(int) * n->_size);
    if (buf == NULL)  // If realloc not successful, exit
    {
      printf("Error reallocing output list");
      exit(1);
    }
    n->_out = buf;
  }
  n->_out[n->_num_outputs] = out; // Add fanout to list of outputs
  n->_num_outputs += 1;          // Increment # of outputs
}

// Adds a fanout to a node
void fanin_node(node* n, int in)
{
  if (n->_num_inputs == MAX_GATE_INPUTS)
  {
    printf("Already have max inputs to node %d",n->_id);
    exit(2);
  }

  n->_in[n->_num_inputs] = in;
  n->_num_inputs += 1;
}

// Gets the number of inputs of a cell given its ID
int get_num_inputs(int cell_id, cell_lib lib)
{
  if (cell_id < lib._num_simple)
    return lib._s[cell_id]._num_inputs;
  else
    return lib._c[cell_id - lib._num_simple - 1]._num_inputs;
}

// Print simple cell
void print_simple(simple s)
{
  printf("Simple | ID: %d, in: %d, delay: %d\n",s._id,s._num_inputs,s._delay);
}

// Print complex cell
void print_complex(complex c)
{
  printf("Complex | ID: %d, u: %d, v: %d, d: %d, e: %d\n",c._id,c._u,c._v,c._d,c._e);
}

// Print library of cells
void print_lib(cell_lib l)
{
  for (int i = 0; i < l._num_simple; i++)
    print_simple(l._s[i]);
  for (int i = 0; i < l._num_complex; i++)
    print_complex(l._c[i]);
}

// Print node
void print_node(node n)
{
  printf("Node | ID: %d, Type: %d, # in: %d, # out: %d, delay: %d, size: %d\n",
    n._id,n._type,n._num_inputs, n._num_outputs, n._delay, n._size);
  printf("     | In: ");
  for (int i = 0; i < n._num_inputs; i++)
    printf("%d, ", n._in[i]);
  printf("\n     | Out: ");
  for (int i = 0; i < n._num_outputs; i++)
    printf("%d, ", n._out[i]);
  printf("\n");
}

/************************************************************************
***************************    Main Program    **************************
************************************************************************/
int main (int argc, char *argv[])
{
  int max_delay = 0;
  cell_lib LIB; // Library of simple and complex cells
  int ID = 0;   // for assigning _id to simple and complex cells in LIB
  FILE *fp;

  /* Make sure there's a file name argument: */
  if (argc != 2) {
    fprintf (stderr, "usage: main filename\n");
    exit (1);
  }

  /* Open the file: */
  fp = fopen (argv[1], "r");
  if (fp == NULL) {
    fprintf (stderr, "cannot open %s\n", argv[1]);
    exit (1);
  }

  /* Read in gate info */
  /* 
    First line = G = # of simple cells
    Next G lines will be the simple cell info
      Each simple gate is denoted by a # between 0 and G-1
      first # on these lines is # of inputs
      second # is delta t, i.e. delay
  */
  int G, num_inputs, delta;
  fscanf(fp, "%d", &G);
  LIB._num_simple = G;  // Initialize number of simple cells in library
  LIB._s = malloc(sizeof(simple) * G);  // Allocate memory for simple cells in lib

  // Populate library with simple cells
  for (int i = 0; i < G; i++)
  {
    fscanf(fp, "%d %d", &num_inputs, &delta);
    LIB._s[i]._id = ID++;
    LIB._s[i]._num_inputs = num_inputs;
    LIB._s[i]._delay = delta;
  }

  /* Read in cell info */
  /* 
    Next line/single int = C = # of complex cells
    Next C lines will be complex cell info
      First two #s are u and v
        They indicate the type of gates covered by the cell; cell may cover a gate of
         type u and a gate of type v if u feeds v
      Final two #s are d and e
        They indicate the delays associated w/ the cell
        Delay d applies to inputs of u and delay e applies to inputs of v
      Wire connecting u to v will disappear after covering
   */
  int C, u, v, d, e;
  fscanf(fp, "%d", &C);
  LIB._num_complex = C;
  LIB._c = malloc(sizeof(complex) * C);

  // Populate library with complex cells
  for (int i = 0; i < C; i++)
  {
    fscanf(fp, "%d %d %d %d", &u, &v, &d, &e);
    LIB._c[i]._id = ID++;
    LIB._c[i]._u = u;
    LIB._c[i]._v = v;
    LIB._c[i]._d = d;
    LIB._c[i]._e = e;
    LIB._c[i]._num_inputs = get_num_inputs(u, LIB) + get_num_inputs(v, LIB);
  }

  /* Read in the DAG */
  /* 
    Next line has 2 ints, I and N
      I = # of primary inputs in the DAG
        Primary inputs identified by a node # b/w 0 and I-1
      N = # of gates in the DAG
        Gates identified by a # b/w I and I+N-1
    Followed by N lines, each describing a gate
      Single int giving gate type
      Sequence of ints giving gate's fanins
        Gates specified in topological order -> a line that describes gate X will only
         contain fanin nodes w/ identifier < X. Node w/o fanouts connects to primary output.
   */
  int I, N, type, in, num_in;
  fscanf(fp, "%d %d", &I, &N);
  // I + N nodes in the DAG (I primary inputs + N intermediary/output nodes)
  node DAG[I + N];    // Array of nodes to represent the DAG

  for (int n = 0; n < I; n++)
  {
    DAG[n] = new_node(n,-1);
  }
  
  for (int n = I; n < I + N; n++) // Primary inputs aren't gates
  {
    fscanf(fp, "%d", &type); // Get gate type
    DAG[n] = new_node(n, type);
    num_in = get_num_inputs(type, LIB);
    for (int i = 0; i < num_in; i++)
    {
      fscanf(fp, "%d", &in);
      fanin_node(&DAG[n], in);   // Add last read node as input to curr node
      fanout_node(&DAG[in], n);  // Add cur node as output from last read node
    }
    fscanf(fp,"\n"); // Clear newline character
  }

  /* Done with reading the input: */
  fclose (fp);

  /* Compute the best delay solution at each node.
   * Traversal is in topological order.
   * 
   * Need to split DAG into new trees when a node has >1 fanout (maybe?)
   */



  print_lib(LIB);
  for (int i = 0; i < I+N; i++)
    print_node(DAG[i]);

  /* Print the solution: */
  printf ("%d\n", max_delay);

  return 0;
}
