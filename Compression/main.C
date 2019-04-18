/* Copyright (c) 2003 ACM/SIGDA

   Written by Kartik Mohanram  (kmram@rice.edu)
*/

#include <iostream>
#include <cstdlib>

#include "testVector.h"

int 
main(int argc, char **argv) 
{
  char *testVectorFile; 

  if(argc != 2) {
    cerr << "usage: testCompress filename" << endl;
    return EXIT_FAILURE;
  }

  testVectorFile = argv[1];

  /* Read the test vector file and setup data structures */

  testVector tv(testVectorFile);

  /* Identify the two most frequently occuring minterms and update the
   * incompletely specified test vector set, before performing compression. Note
   * that you do not need to perform compression, this follows automatically
   * after the test vector set is updated. */

  int cubeWidth = 4;
  tv.compress(cubeWidth);

  /* Print the updated test set, prior to compression */

  tv.printCompressedTestSet();

  return EXIT_SUCCESS;
}
