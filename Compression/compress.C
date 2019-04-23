/* Copyright (c) 2003 ACM/SIGDA

   Written by Kartik Mohanram  (kmram@rice.edu)
*/

#define Z '0'
#define O '1'
#define X '2'
#define TRACE_COMPRESS 1

#include "testVector.h"
#include <math.h>
#include <map>


// Recursively finds all matches to integer values for a given string
vector<int> all_matches(string cube)
{
    vector<int> ret;

    // No X's in this string
    if (cube.find(X) == std::string::npos)
        ret.push_back(stoi(cube, nullptr, 2));
    else    // There is an X. Replace it with 0 and 1 and call 2 more times.
    {
        string with_zero;
        string with_one;

        size_t index = cube.find(X);    // Find first X
        if (index == 0)                 // First character
        {
            with_zero = cube.substr(0,1) + Z + cube.substr(2, string::npos);
            with_one  = cube.substr(0,1) + O + cube.substr(2, string::npos);
        }
        else if (index == cube.length() - 1)    // Last character
        {
            with_zero = cube.substr(0,index) + Z;
            with_one  = cube.substr(0,index) + O;
        }
        else    // Any other character
        {
            with_zero = cube.substr(0,index) + Z + cube.substr(index + 1, string::npos);
            with_one  = cube.substr(0,index) + O + cube.substr(index + 1, string::npos);
        }
        
        vector<int> first = all_matches(with_zero);
        vector<int> second = all_matches(with_one);

        ret.insert(ret.end(), first.begin(), first.end());
        ret.insert(ret.end(), second.begin(), second.end());
    }
    
    return ret;
}

// Finds indices of highest 2 values in an array
vector<int> find_maxes(int arr[], int size)
{
    int max1 = -1;
    int max2 = -1;
    vector<int> indices (2,-1);

    // Find first max
    for (int i = 0; i < size; i++)
    {
        if (arr[i] > max1)
        {
            max1 = arr[i];
            indices.front() = i;
        }
    }

    // Find second m ax
    for (int i = 0; i < size; i++)
    {
        if (arr[i] > max1 && arr[i] != max1)
        {
            max2 = arr[i];
            indices.back() = i;
        }
    }

    return indices;
}

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
    // Size 2^cubeWidth array, holds frequency of minterms occuring for all possible terms
    // int freqs[ int(pow(2,cubeWidth)) ];
    map<string, int> freqs;
    int vector_len = testVectorSet.front().length();

    // Find most common minterms in all vectors
    vector<string>::iterator iter;
    // For each vector in the set
    for (iter = testVectorSet.begin(); iter < testVectorSet.end(); iter++)
    {
        // buff = string(*iter);
        // For each term in the vector
        for (int i = 0; i < vector_len; i += cubeWidth)
        {
            // vector<int> indices = all_matches(buff.substr(i,cubeWidth));
            // vector<int> indices = all_matches(returnCube(*iter,i,cubeWidth));
            // Increment frequencies of all matches values
            // for (vector<int>::iterator j = indices.begin(); j < indices.end(); j++)
                // freqs[*j]++;
            freqs[ returnCube(*iter, i, cubeWidth) ]++;
        }
    }

    // Get indices of minterms
    // vector<int> minterm_indices = find_maxes(freqs, int(pow(2,cubeWidth)));


    // Try to match as many terms w/ X's to the minterms
    // Matching against each minterm:
    // - If all non-X bits are equal to the minterm's, then they match
    // - - append matching minterm to buffer
    // - If no match against either minterm, keep as is
    // -- append current term to buffer
    // - 
}