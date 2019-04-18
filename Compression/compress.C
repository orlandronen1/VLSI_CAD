/* Copyright (c) 2003 ACM/SIGDA

   Written by Kartik Mohanram  (kmram@rice.edu)
*/

#define Z '0'
#define O '1'
#define X '2'

#include "testVector.h"
#include <math.h>

/* You need to provide the solution to the testVector::compress routine. All
 * updates to the test vector set are to be done on the
 * testVector::testVectorSet member, so that the printCompressedTestSet method
 * produces the appropriate output.
 *
 * Also, remember to update members m1 and m2 with the first and second most
 * frequently occuring minterms for the final output 
 *
 * Do not make any changes to main.C. You are allowed to add members/methods to
 * the body of the testVector class, in addition to any stand-alone methods. My
 * solution, with debug statements, runs to 75 lines: as always, this means
 * that a simple solution is better than a complex one!  */ 

void testVector::compress(int cubeWidth) {
    // Size 2^cubeWidth array, holds frequency of minterms occuring for all possible 16 terms
    int freqs[pow(2,cubeWidth)];

    std::string buff;   // Buffer for modified test vector
    std::string vec;    // Holds the current test vector

    // When finding a match, or possible match, increment that minterm's frequency
    // Find two highest values, assign their indices (in binary) as minterms
    // For each term in the vector
    // for (int i = 0; i < testVectorSet.front().length(); i += cubeWidth)
    // {

    // }

    // Try to match as many terms w/ X's to the minterms
    // Matching against each minterm:
    // - If all non-X bits are equal to the minterm's, then they match
    // - - append matching minterm to buffer
    // - If no match against either minterm, keep as is
    // -- append current term to buffer
    // - 
}
