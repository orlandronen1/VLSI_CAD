/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

/*
  The cell library
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>

using namespace::std;

enum CellStrength { A=0,B,C,D,E,F,G,H,I,J,K,L,S_ERROR };  // cell strength
enum SignalType { EDGE_R, EDGE_F };                       // edge type

extern CellStrength TranslateStrength(char);

// Each delay instance is a multi-dimensional table
class DelayInstance {
 private:

  int            _num_slew;
  int            _num_load;

  double        *_input_slew;
  double        *_load_cap;
  double        *_delay;
  double        *_slew;
  double        *_root;

  double _extrapolate(double x1, double x2, double y1, double y2, double x);
  void _locate(double *_indices, int total_length, double value, int &idx1, 
	       int &idx2);

 public:
  DelayInstance():_num_slew(0),_num_load(0),_input_slew(0),
    _load_cap(0),_delay(0),_slew(0),_root(0) 
  {
  }
  
  ~DelayInstance() 
  {
    if (_root) delete [] _root; 
    _root = 0; 
  }

  void Assign(int num_slew, int num_load, double *in_slews, double *loads, 
	      double *out_slew, double *out_delay);
  void Query(double in_slew, double load, double &out_delay, double &out_slew);
  void Query(double load, double &out_delay, double &out_slew);
  void Query(double &out_delay, double &out_slew);
};


/* 
   Simplified Library entry: only differentiate rising and falling edges
 */
class LibraryEntry {
 private:
  string         _name;      // cell name tag
  CellStrength   _cst;

  double          _load_cap; // loading cap
  DelayInstance  *_r_inst;   // rising edge
  DelayInstance  *_f_inst;   // falling edge

 public:
  LibraryEntry(char* name, char cell_strength, double loadcap) 
  {
    _name = name;
    _cst = TranslateStrength(cell_strength);
    _load_cap = loadcap;
    _r_inst = _f_inst = 0;
  }

  string Name() { return _name; }
  CellStrength  Strength() { return _cst; }
  double LoadCap() { return _load_cap; }

  ~LibraryEntry() {
    if ( _r_inst ) { 
      delete _r_inst; 
      _r_inst = 0;
    }
    if ( _f_inst ) { 
      delete _f_inst; 
      _f_inst = 0;
    }
  }

  void AddEdge(SignalType e, int num_slew, int num_load, double *in_slew, 
	       double *loads, double *out_slews, double *out_delays);
  void QueryCell(SignalType e, double in_slew, double load, double &out_delay, 
		 double &out_slew);
};

/*
  Generic Library: 

  To add a cell:

     rc = AddCell(name, cs);   // where cs is the driving strength ( A-L )
     AddEdge(rc, EDGE_R/EDGE_F, num_slew, num_load, in_slew, loads, out_slews, 
             out_delays);
    
  To query: 

     rc = LocateCell(name, cs);
     QueryCell(rc, EDGE_R/EDGE_F, in_slew, load, out_delay, out_slew);

    Bilinear intrapolation is applied when delay and slew are queried
 */
class Library {
 private:
  double         _vdd;         // recording only, no functional purpose
  double         _temp;        // dido
  int            _num_entries;

  LibraryEntry  *_cells[300];  // pre-allocated 300 entries
  LibraryEntry  *_current;

 public:
  Library(double vdd, double temp) 
  {
    _vdd = vdd;
    _temp = temp;
    _num_entries = 0;
    memset(&_cells[0], 0, 300*sizeof(LibraryEntry*));
  }

  ~Library() 
  {
    for (int i=0; i<_num_entries; i++ ) {
      if ( _cells[i] ) delete _cells[i];
    }
  }

  /* Add a cell characterization to the library, returns the index of the cell
     library */
  int AddCell(char* name, double loadcap, char cs='A');

  /* Given the index, record the delay/slew table at different input slews and 
     loading condtions */
  void AddSignalEdge(int which, SignalType e, int num_slew, int num_load, 
		     double *in_slew, double *loads, double *out_slews, 
		     double *out_delays);

  /* Find the cell library, given the name */
  int LocateCell(const char* name, char cs='A');

  /* find the loading cap the cell */
  double FindCellLoadCap(int which);

  /* Given the cell index, query delay and slew */
  void QueryCell(int which, SignalType e, double in_slew, double load,
		 double &out_delay, double &out_slew);

  /* Simplified version, when loading cap and input slew are not included in 
     the library */
  void QueryCell(int which, SignalType e, double &out_delay);

  /* Load the libray content from a tech file */
  void LoadLibrary(ifstream& f);
  // void LoadLibrary(FILE *f);
};

#endif // LIBRARY_H

/* Sample library file

# Leading #, * or % indicating a comment line
#
# Format:
# name strength load_cap num_load num_in_slew load ... slew ...
#   rising_delay... rising_slew... falling_delay ... falling_slew ...
#
# Specification:
#    name          - not much to explain, though capical letters are prefered
#    strength      - letters A to S
#    loading cap   - any real number
#    num_load      - one dimension in the library table
#    num_in_slew   - another dimension in the library table
#    load          - as many fields as indicated by num_load
#    slew          - as many fields  as indicated by num_slew
#    rising_delay  - float array of num_load*num_in_slew, delay of rising 
#                    transition
#    rising_slew   - float array of num_load*num_in_slew, slew of falling 
#                    transition
#    falling_delay - float array of num_load*num_in_slew, delay of rising
#                    transition
#    falling_slew  - float array of num_load*num_in_slew, slew of falling
#                    transition
#
# For the example below, the first line says:
#   cell NAND2, strength A, 0.0 loading cap, only one next stage cap, 
#   only one input 
#   slew (therefore we use numbers 0 0). The rising delay is 2.0, rising slew 
#   is 0, the falling delay is 1.0, and the falling slew is 0.0
#
NAND2 A 0.0 1 1 0.0 0.0   2.0 0.0   1.0 0.0
NAND2 B 0.0 1 1 0.0 0.0   4.0 0.0   2.0 0.0
NAND2 C 0.0 1 1 0.0 0.0   3.0 0.0   1.0 0.0
NAND2 D 0.0 1 1 0.0 0.0   1.0 0.0   1.0 0.0
NAND2 E 0.0 1 1 0.0 0.0   3.0 0.0   2.0 0.0
NAND2 F 0.0 1 1 0.0 0.0   2.0 0.0   2.0 0.0
NAND2 G 0.0 1 1 0.0 0.0   1.0 0.0   3.0 0.0
# 
*/
// End
