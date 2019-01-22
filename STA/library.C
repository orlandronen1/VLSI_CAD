/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include <assert.h>
#include <string.h>
#include "library.h"
#include "tokenizer.h"

#define DEBUG 0

/* Linear intrapolation, although the function name says otherwise

     x1 -> y1
     x2 -> y2

     and x1 < x2

     calculate y = f(x), assuming linearity holds
 */
double DelayInstance::_extrapolate(double x1, double x2, double y1, double y2, 
				   double x) 
{
  return ( y1 + (y2-y1)*(x-x1)/(x2-x1) );
}

/*
  Given a double array and a value, find the indices, using dumb linear search 
 */
void DelayInstance::_locate(double *values, int total, double v, int &idx1, 
			    int &idx2) 
{
  if ( v < values[0] ) {               // special case 1
    idx1 = 0;
    idx2 = 1;
  } else if ( v >  values[total-1] ) { // special case 2
    idx1 = total-2;
    idx2 = total-1;
  } else {                             // general case, dumb linear search
    int i;
    for ( i=0; i<total; i++ ) if ( values[i] > v ) break; // a little flimsy
    idx2 = i;
    idx1 = i-1;
  }
  return;
}

/*
  Simple copy: assuming the delay/slew are stored in row based format, i.e.:
     (slew1,load1), (slew1,load2), (slew1,load3), ... 
     (slew2,load1), (slew2,load2),...,
     (slew3,load1), (slew3,load2),...
  */
void DelayInstance::Assign(int num_slew, int num_load, double *in_slews, 
			   double *loads, double *out_slew, double *out_delay) 
{
  if ( _root ) return;  // already allocated, get out

  // allocate
  _num_slew = num_slew;
  _num_load = num_load;

  _root = new double [ num_slew + num_load + 2*num_slew*num_load ];
  double *tmp = _root;
  
  _input_slew = tmp;
  tmp += num_slew;
  
  _load_cap = tmp;
  tmp += num_load;
  
  _delay = tmp;
  tmp += (num_load*num_slew);
  
  _slew = tmp;

  // copy
  memcpy(_input_slew, in_slews, _num_slew*sizeof(double));
  memcpy(_load_cap, loads, _num_load*sizeof(double));

  memcpy(_slew, out_slew, _num_load*_num_slew*sizeof(double));
  memcpy(_delay, out_delay, _num_load*_num_slew*sizeof(double));
}

/* 
   Find the value, using bilinear interpolation 
*/

void DelayInstance::Query(double in_slew, double load, double &out_delay, 
			  double &out_slew)
{
  int slew_idx1, slew_idx2, load_idx1, load_idx2;
  int corner11, corner12, corner21, corner22;
  double tmp_val1, tmp_val2;

  // special cases
  if ( _num_slew == 1 && _num_load == 1 ) {
    return Query(out_delay, out_slew);
  } else if ( _num_load > 1 ) {
    return Query(load, out_delay, out_slew);
  }

  _locate(_input_slew, _num_slew, in_slew, slew_idx1, slew_idx2);
  _locate(_load_cap, _num_load, load, load_idx1, load_idx2);
  
  corner11 = slew_idx1*_num_load + load_idx1;
  corner12 = slew_idx1*_num_load + load_idx2;
  corner21 = slew_idx2*_num_load + load_idx1;
  corner22 = slew_idx2*_num_load + load_idx2;
  
  tmp_val1 = _extrapolate(_input_slew[slew_idx1], _input_slew[slew_idx2],
			  _slew[corner11], _slew[corner21], in_slew);
  tmp_val2 = _extrapolate(_input_slew[slew_idx1], _input_slew[slew_idx2],
			  _slew[corner12], _slew[corner22], in_slew);
  out_slew = _extrapolate(_load_cap[load_idx1], _load_cap[load_idx2],tmp_val1,
			  tmp_val2, load);

  tmp_val1 = _extrapolate(_input_slew[slew_idx1], _input_slew[slew_idx2],
			  _delay[corner11], _delay[corner21], in_slew);
  tmp_val2 = _extrapolate(_input_slew[slew_idx1], _input_slew[slew_idx2],
			  _delay[corner12], _delay[corner22], in_slew);
  out_delay = _extrapolate(_load_cap[load_idx1], _load_cap[load_idx2],tmp_val1,
			   tmp_val2, load);
}

void DelayInstance::Query(double load, double &out_delay, double &out_slew) 
{
  int load_idx1, load_idx2;

  _locate(_load_cap, _num_load, load, load_idx1, load_idx2);
  out_delay = _extrapolate(_load_cap[load_idx1], _load_cap[load_idx2],
			   _delay[load_idx1], _delay[load_idx2], load);
  out_slew = _slew[0];
}

void DelayInstance::Query(double &out_delay, double &out_slew) 
{
  out_delay = _delay[0];
  out_slew = _slew[0];
}


// Translate cell strength
CellStrength TranslateStrength(char s) 
{
  s = (s>='a' && s<='l') ? s-'a'+'A' : s;
  return (s>='A'&&s<='L') ? (CellStrength)(s-'A') : S_ERROR;
}

void LibraryEntry::AddEdge(SignalType e, int num_slew, int num_load, 
			   double *in_slews, double *loads, double *out_slews, 
			   double *out_delays) 
{
  DelayInstance *t;
  
  if ( e == EDGE_R ) {
    if ( _r_inst == 0 ) _r_inst = new DelayInstance();
    t = _r_inst;
  } else {
    if ( _f_inst == 0 ) _f_inst = new DelayInstance();
    t = _f_inst;
  }
  
  t->Assign(num_slew, num_load, in_slews,loads, out_slews, out_delays);
}

void LibraryEntry::QueryCell(SignalType e, double in_slew, double load,
			     double &out_delay, double &out_slew) 
{
  DelayInstance *t;

  if ( e == EDGE_R ) t = _r_inst;
  else t=_f_inst;
  
  if ( t == 0 ) {
    out_delay = -99.99;
    out_slew = -99.99;
  }
  
  t->Query(in_slew, load, out_delay, out_slew);
}

int Library::AddCell(char *name, double loadcap, char cs) 
{
  // better to translate into upper case!
  _cells[_num_entries] = new LibraryEntry(name, cs, loadcap);
  return (_num_entries++);
};

void Library::AddSignalEdge(int which, SignalType e,int num_slew, int num_load,
			    double *in_slew, double *loads, double *out_slews,
			    double *out_delays) 
{
  LibraryEntry *t = _cells[which];

  if ( t == 0 ) return;  // quiet death, can be improved!

  t->AddEdge(e, num_slew, num_load, in_slew, loads, out_slews, out_delays);
}

int Library::LocateCell(const char* name, char cs) 
{
  // better translate into upper case
  int i;
  CellStrength c = TranslateStrength(cs);
  for (i=0; i<_num_entries; i++) {
    if ( _cells[i]->Name() == std::string(name) && 
	 _cells[i]->Strength() == c ) {
      return i;
    }
  }
  return -1;  // not found!
}

double Library::FindCellLoadCap(int which) 
{
  LibraryEntry *t = _cells[which];
  
  if ( t == 0 ) return -99.99;

  return t->LoadCap();
}

void Library::QueryCell(int which, SignalType e,double in_slew, double load, 
			double &out_delay, double &out_slew)
{
  LibraryEntry *t = _cells[which];
  
  if ( t == 0 ) {
    out_delay = -99.99;
    out_slew = -99.99;
    return;
  }

  t->QueryCell(e, in_slew, load, out_delay, out_slew);
}

void Library::QueryCell(int which, SignalType e, double &out_delay) 
{
  LibraryEntry *t = _cells[which];
  double junk;

  if ( t == 0 ) {
    out_delay = -99.99;
    return;
  }

  t->QueryCell(e, 0.0, 0.0, out_delay, junk);
}

void Library::LoadLibrary(ifstream& f) {
// void Library::LoadLibrary(FILE *f) {
  // ssize_t  read;
  // size_t len;
  char  *line = new char[1000]; // char *line=NULL;
  char **tokens;
  int  sz;
  char *name;
  char strength;
  double load, all_loads[32], all_slew[32], r_delays[1024], r_slews[1024], 
         f_delays[1024], f_slews[1024];
  int num_load, num_in_slew;
  int i,k,l;

  // if ( !f ) return;

  Tokenizer TK;

  // while ( (read = getline(&line, &len,f)) != -1 ) {
  while ( !f.eof() ) {
    f.getline(line, 1000, '\n');
    sz = TK.Parse(line, &tokens);
    if ( sz > 0 ) {
      k = 0;
      if ( strcmp(tokens[0],"*")==0 || strcmp(tokens[0],"#")==0 ||
	   strcmp(tokens[0],"%")==0 ) continue;
      name = tokens[k++];
      strength = tokens[k++][0];
      load = atof(tokens[k++]);
      num_load = atoi(tokens[k++]);
      num_in_slew = atoi(tokens[k++]);

      for (i=0; i<num_load; i++) {
	all_loads[i] = atof(tokens[k++]);
      }
      for (i=0; i<num_in_slew; i++) {
	all_slew[i] = atof(tokens[k++]);
      }
      for (i=0; i<num_load*num_in_slew; i++) {
	r_delays[i] = atof(tokens[k++]);
      }
      for (i=0; i<num_load*num_in_slew; i++) {
	r_slews[i] = atof(tokens[k++]);
      }
      for (i=0; i<num_load*num_in_slew; i++) {
	f_delays[i] = atof(tokens[k++]);
      }
      for (i=0; i<num_load*num_in_slew; i++) {
	f_slews[i] = atof(tokens[k++]);
      }

#if DEBUG
      printf("%s %c %g %d %d\t(", name, strength, load, num_load, num_in_slew);
      for (i=0; i<num_load;i++) printf("%g ", all_loads[i]);
      printf(")\t(");
      for (i=0; i<num_in_slew;i++) printf("%g ", all_slew[i]);
      printf(")\n\t");
      for (i=0; i<num_load*num_in_slew;i++) printf("%g ", r_delays[i]);
      printf("\n\t");
      for (i=0; i<num_load*num_in_slew;i++) printf("%g ", r_slews[i]);
      printf("\n\t");
      for (i=0; i<num_load*num_in_slew;i++) printf("%g ", f_delays[i]);
      printf("\n\t");
      for (i=0; i<num_load*num_in_slew;i++) printf("%g ", f_slews[i]);
      printf("\n");
#endif
      l = AddCell(name, load, strength);
      AddSignalEdge(l, EDGE_R, num_in_slew, num_load, all_slew, all_loads,
		    r_slews, r_delays);
      AddSignalEdge(l, EDGE_F, num_in_slew, num_load, all_slew, all_loads, 
		    f_slews, f_delays);
    }
    
  } 
  if (line) free (line);
}
// End
