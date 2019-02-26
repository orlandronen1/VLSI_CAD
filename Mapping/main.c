/* Copyright (c) 2004 ACM/SIGDA
   Prepared by Valavan Manohararajah, vmanohar@altera.com
   Modifications by Florian Krohm, fkrohm@us.ibm.com
*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_GATE_TYPES	10
#define MAX_GATE_INPUTS	2
#define MAX_CELL_TYPES	20
#define MAX_DAG_NODES	100000

/* Type definitions for gate types/cells/nodes: */
typedef struct node
{
  int _id;    // ID for DAG
  int _type;  // Type of cell, i.e. ID of cell
  int _num_inputs;
  int _num_outputs;
  int _delay;
  int* _in;
  int* _out;
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
} complex;

typedef struct cell_library
{
  int _num_simple;
  int _num_complex;
  simple* _s;
  complex* _c;
} cell_lib;

typedef struct DAG
{
  int _size;
  node* _PI;
  node* _PO;
} DAG;

typedef struct forest
{
  int _size;
  DAG* _dags;
} forest;


/************************************************************************
****************************    Functions    ****************************
************************************************************************/

// Creates a new node from 
node new_node(int id, int type, int in, int out, int delay)
{
  node ret;
  ret._id = id;
  ret._num_inputs = in;
  ret._num_outputs = out;
  ret._delay = delay;
  ret._in = malloc(sizeof(int) * in);
  ret._out = malloc(sizeof(int) * out);
  return ret;
}

// Print simple cell
void print_simple(simple s)
{
  printf("Simple | ID: %d  in: %d  delay: %d\n",s._id,s._num_inputs,s._delay);
}

// Print complex cell
void print_complex(complex c)
{
  printf("Complex | ID: %d  u: %d  v: %d  d: %d  e: %d\n",c._id,c._u,c._v,c._d,c._e);
}

// Print library of cells
void print_lib(cell_lib l)
{
  for (int i = 0; i < l._num_simple; i++)
    print_simple(l._s[i]);
  for (int i = 0; i < l._num_complex; i++)
    print_complex(l._c[i]);
}

/************************************************************************
***************************    Main Program    **************************
************************************************************************/
int main (int argc, char *argv[])
{
  int max_delay = 0;
  cell_lib LIB;
  int ID = 0; // for assigning _id to simple and complex cells in LIB
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
  // printf("%d\n", G);
  LIB._num_simple = G;  // Initialize number of simple cells in library
  LIB._s = malloc(sizeof(simple) * G);  // Allocate memory for simple cells in lib

  for (int i = 0; i < G; i++)
  {
    fscanf(fp, "%d %d", &num_inputs, &delta);
    // printf("%d %d\n", num_inputs, delta);
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
  // printf("%d\n", C);
  LIB._num_complex = C;
  LIB._c = malloc(sizeof(complex) * C);

  for (int i = 0; i < C; i++)
  {
    fscanf(fp, "%d %d %d %d", &u, &v, &d, &e);
    // printf("%d %d %d %d\n", u, v, d, e);
    LIB._c[i]._id = ID++;
    LIB._c[i]._u = u;
    LIB._c[i]._v = v;
    LIB._c[i]._d = d;
    LIB._c[i]._e = e;
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
  int I, N;
  fscanf(fp, "%d %d", &I, &N);
  // printf("%d %d\n", I, N);
  int delays[N];

  for (int i = 0; i < N; i++)
  {
    delays[i] = 0;
    // Build the DAG;
  }

  /* Done with reading the input: */
  fclose (fp);

  /* Compute the best delay solution at each node.
   * Traversal is in topological order.
   * 
   * Need to split DAG into new trees when a node has >1 fanout (maybe?)
   */

  /* ... */

  print_lib(LIB);

  /* Print the solution: */
  printf ("%d\n", max_delay);

  return 0;
}
