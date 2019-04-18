/* Copyright (c) 2003 ACM/SIGDA

   Written by Kartik Mohanram  (kmram@rice.edu)
*/

#include <fstream>
#include <algorithm>
using namespace std;

#include "testVector.h"

testVector::testVector(char * input_file) {

  ifstream strm;
  strm.open(input_file, ios::in);
  if (!strm) {
    cerr << "Unable to open '" << input_file << "'" << endl;
    exit(EXIT_FAILURE);
  }

  strm >> numVectors;
  strm >> numScanCells;
  // cout << "Test vector has " << numVectors << " vectors and ";
  // cout << numScanCells << " scan cells" << endl;

  testVectorSet.resize(numVectors);

  char * token = new char[numScanCells + 1];

  int vectorNumber = 0;
  while (!strm.eof()) {
    strm >> token; if(strm.eof()) { break; }
    // cout << "Token is " << token << endl;
    testVectorSet[vectorNumber] = token;
    ++vectorNumber;
  }

}

void testVector::printCompressedTestSet() { 
  cout << "Minterm1: " << m1 << endl;
  cout << "Minterm2: " << m2 << endl;
  for(int i = 0; i < numVectors; ++i) {
    cout << testVectorSet[i] << endl;
  }
}   

string testVector::binaryConvert(int n, int cubeWidth) {
  string tmp;
  if(n == 0) {
    for(int i = 0; i < cubeWidth; ++i) { tmp += "0"; }
    return tmp;
  }
  while(n) {
    if(n % 2) {
      tmp += "1";
    } else {
      tmp += "0";
    }
    n /= 2;
  }
  while(tmp.size() < (unsigned int)(cubeWidth)) {
    tmp += "0";
  }
  reverse(tmp.begin(), tmp.end());
  return tmp;
}

string testVector::returnCube(string& vector, int position, int size) {
  string tmp;
  for(int i = 0; i < size; ++i) {
    tmp += vector[position + i];
  }
  return tmp;
}
