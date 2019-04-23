/* Copyright (c) 2003 ACM/SIGDA

   Written by Kartik Mohanram  (kmram@rice.edu)
*/

#define Z '0'
#define O '1'
#define X '2'
#define TRACE 1

#include "testVector.h"
#include <math.h>
#include <map>


// Recursively finds all matches to integer values for a given string
void all_matches(string cube, std::map<string, int> &m);

// Finds keys of highest 2 values in a array
string find_max(std::map<string, int> &m);

// Checks if a term matches a string
bool check_match(string cube, string term);


void testVector::compress(int cubeWidth) {
    map<string, int> freqs;
    int vector_len = testVectorSet.front().length();
    vector<string>::iterator iter;

    // Find frequencies
    for (iter = testVectorSet.begin(); iter < testVectorSet.end(); iter++)
    {
        for (int i = 0; i < vector_len; i += cubeWidth) // For each term in the vector
        {
            string cube = returnCube(*iter, i, cubeWidth);
            if (cube.find(X) != string::npos)
            {
                #if TRACE
                cout << "TERM: " << cube << "\n";
                #endif
                all_matches(cube, freqs);
            }
        }
    }
    m1 = find_max(freqs);
    // Replace matching terms with minterm 1
    for (iter = testVectorSet.begin(); iter < testVectorSet.end(); iter++)
    {
        string buf;
        for (int i = 0; i < vector_len; i+= cubeWidth)
        {
            string cube = returnCube(*iter, i, cubeWidth);
            if (check_match(cube, m1))  // Matches minterm
                buf += m1;
            else    // Doesn't match either minterm
                buf += cube;
        }
        *iter = buf;
    }

    #if TRACE
    cout << "*\n* FINISHED REPLACING WITH FIRST MINTERM\n*\n";
    #endif

    // Do the same process as above for second minterm, after replacements with first
    freqs.clear();
    for (iter = testVectorSet.begin(); iter < testVectorSet.end(); iter++)
    {
        for (int i = 0; i < vector_len; i += cubeWidth) // For each term in the vector
        {
            string cube = returnCube(*iter, i, cubeWidth);
            if (cube.find(X) != string::npos)
            {
                #if TRACE
                cout << "TERM: " << cube << "\n";
                #endif
                all_matches(cube, freqs);
            }
        }
    }
    m2 = find_max(freqs);
    for (iter = testVectorSet.begin(); iter < testVectorSet.end(); iter++)
    {
        string buf;
        for (int i = 0; i < vector_len; i+= cubeWidth)
        {
            string cube = returnCube(*iter, i, cubeWidth);
            if (check_match(cube, m2))  // Matches minterm
                buf += m2;
            else    // Doesn't match either minterm
                buf += cube;
        }
        *iter = buf;
    }
}


void all_matches(string cube, std::map<string, int> &m)
{
    // No X's in this string
    if (cube.find(X) == std::string::npos)
    {
        m[cube]++;
        #if TRACE
        cout << "-FREQ OF " << cube << " IS NOW " << m[cube] << "\n";
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

string find_max(std::map<string, int> &m)
{
    int max_val = -1;
    string max_key;

    // Find first max
    for (auto i = m.begin(); i != m.end(); i++)
        if (i->second > max_val || (i->second == max_val && stoi(i->first,nullptr,2) < stoi(max_key,nullptr,2)) )
        {
            max_val = i->second;
            max_key = i->first;
        }
    return max_key;
}

bool check_match(string cube, string term)
{
    for (uint i = 0; i < cube.length(); i++)
    {
        if (cube[i] == X)
            continue;
        if (cube[i] != term[i])
            return false;
    }
    return true;
}