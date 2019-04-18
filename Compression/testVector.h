/* Copyright (c) 2003 ACM/SIGDA

   Written by Kartik Mohanram  (kmram@rice.edu)
*/

#ifndef TESTVECTOR_H
#define TESTVECTOR_H

#include <iostream>
#include <vector>
#include <string>
using namespace std;

class testVector;

/* This class encapsulates the test vector file */

class testVector {
  private:
    int numVectors;
    int numScanCells;
    vector<string> testVectorSet;
    string m1;
    string m2;
     
  public:
    /* The method that needs to be completed. See compress.C for further
     * details. The value of cubeWidth is 4 as per the problem description */

    void compress(int cubeWidth);

    /* HELPER routines
     * ---------------
     * The constructor reads the input_file and populates the data structures */

    testVector(char *input_file);

    /* Print the test set after compression */

    void printCompressedTestSet();

    /* Convert integer/minterm n to a string of width cubeWidth */
    string binaryConvert(int n, int cubeWidth);

    /* This can be used to extract size number of bits from position in a test
     * vector */

    string returnCube(string& vector, int position, int size);
};

#endif /* TESTVECTOR_H */
