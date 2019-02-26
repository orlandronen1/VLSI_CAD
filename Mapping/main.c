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

  /* ... */

  /* Read in cell info */

  /* ... */

  /* Read in the DAG */

  /* ... */

  /* Done with reading the input: */
  fclose (fp);

  /* Compute the best delay solution at each node.
   * Traversal is in topological order.
   */

  /* ... */

  /* Print the solution: */
  printf ("%d\n", max_delay);

  return 0;
}
