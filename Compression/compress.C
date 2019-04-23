/* Copyright (c) 2003 ACM/SIGDA

   Written by Kartik Mohanram  (kmram@rice.edu)
*/

#define Z '0'
#define O '1'
#define X '2'
#define TRACE_COMPRESS 1
#define TRACE_MATCHING 1

#include "testVector.h"
#include <math.h>
#include <map>


// Recursively finds all matches to integer values for a given string
void all_matches(string cube, std::map<string, int> &m);

// Finds keys of highest 2 values in a array
vector<string> find_maxes(std::map<string, int> &m);

// Checks if a term matches a string
bool check_match(string cube, string term);

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
        for (int i = 0; i < vector_len; i += cubeWidth) // For each term in the vector
        {
            #if TRACE_COMPRESS
            cout << "TERM: " << returnCube(*iter, i, cubeWidth) << "\n";
            #endif
            all_matches(returnCube(*iter, i, cubeWidth), freqs);
        }
    }

    // Get minterms
    vector<string> minterms = find_maxes(freqs);
    m1 = minterms.front();
    m2 = minterms.back();


    // Try to match as many terms w/ X's to the minterms
    // Matching against each minterm:
    // - If all non-X bits are equal to the minterm's, then they match
    // - - append matching minterm to buffer
    // - If no match against either minterm, keep as is
    // -- append current term to buffer
    // - 
}

void all_matches(string cube, std::map<string, int> &m)
{
    // No X's in this string
    if (cube.find(X) == std::string::npos)
    {
        m[cube]++;
        #if TRACE_MATCHING
        cout << "FREQ OF " << cube << " IS NOW " << m[cube] << "\n";
        #endif
    }
    else    // There is an X. Replace it with 0 and 1 and call 2 more times.
    {
        string with_zero;
        string with_one;
        size_t index = cube.find(X);    // Find first X

        if (index == 0)                 // First character
        {
            with_zero = Z + cube.substr(1, string::npos);
            with_one  = O + cube.substr(1, string::npos);
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
        
        all_matches(with_zero, m);
        all_matches(with_one, m);
    }

}

vector<string> find_maxes(std::map<string, int> &m)
{
    int max1 = -1;
    int max2 = -1;
    vector<string> keys (2, "");

    // Find first max
    for (auto i = m.begin(); i != m.end(); i++)
        if (i->second > max1)
        {
            max1 = i->second;
            keys.front() = i->first;
        }

    // Find second max
    for (auto i = m.begin(); i != m.end(); i++)
        if (i->second > max2 && i->first != keys.front())
        {
            max2 = i->second;
            keys.back() = i->first;
        }

    return keys;
}

bool check_match(string cube, string term)
{
    
}