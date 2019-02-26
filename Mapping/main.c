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

/* ... */

/* Data structure declarations: */

/* ... */

int main (int argc, char *argv[])
{
  int max_delay = 0;
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

  /* Done with reading the input: */
  fclose (fp);

  /* Compute the best delay solution at each node.
   * Traversal is in topological order.
   * 
   * Need to split DAG into new trees when a node has >1 fanout
   */

  /* ... */

  /* Print the solution: */
  printf ("%d\n", max_delay);

  return 0;
}
