// $Id: walk.C,v 1.1.1.1 2004/07/06 20:40:39 cvsdevel Exp $
//    Frank Liu    draft     11/01/2003

/*********************************************
 * 
 * walk.C - top level driver 
 * 
 *********************************************/
#include <stdlib.h>
#include <stdio.h>
#include "circuit.h"

const int L_limit = 500;

int main(int argc, char *argv[] ) {
  Circuit *C;
  FILE    *fp;
  int     num;
  
  if ( argc != 3 ) { 
    printf("Usage: %s <input netlist> <number of walks per node>\n", argv[0]);
    exit(0);
  }
  
  if ( (fp = fopen(argv[1],"r"))==NULL ) {
    printf("FATAL: Unable to open input netlist file %s\n", argv[1]);
    exit(-1);
  }

  num = atoi(argv[2]);
  if ( num < L_limit ) {
    printf("WARNING: walks/node specified is too small. Use default value of %d.\n", L_limit);
    num = L_limit;
  }

  // load the circuit
  C = new Circuit;
  if (  C->LoadCircuit(fp) != 0 ) {
    exit (-1);
  }

  C->GetAllVoltages(num);

  delete C;
  return (0);
}

// end
