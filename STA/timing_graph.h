/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

// A simple graph package

#ifndef TIMING_H
#define TIMING_H

#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <queue>
#include <stack>

#include<fstream>
#include<stdexcept>

#include "library.h"

using namespace::std;

inline double MAX(double a, double b) { return (a>b)?a:b; }
inline double MIN(double a, double b) { return (a>b)?b:a; }

const double min_rt=0;
const double max_rt=+99999;

enum EdgeType { NOT=0, 
                NAND2, NAND3, NAND4, NAND5, 
                NOR2, NOR3, NOR4, NOR5, 
                AND2, AND3, AND4, AND5, 
                OR2, OR3, OR4, OR5, 
                BUF,
                NUM_OF_CELL_TYPES };

enum PinType { PI, PO };

class TimingInfo {
 private:
  double     _at;     // arrival time
  double     _rt;     // required time
  double     _slack;  // slack
 public:
  TimingInfo():_at(min_rt),_rt(max_rt),_slack(0.0) {}
  ~TimingInfo() {}

  double& AT()    { return _at; }
  double& RT()    { return _rt; }
  double& SLACK() { return _slack; }
};

class Vertex;

/* Edge in the graph. Each edge also contains two timinginfo, one for rising 
   edge, one for falling edge 
    
   Each edge maintains a record the number of targets (the vertex it points 
   to) and sources. It also maintain a list of those target and source
   vertices .
    
   To facilitate graph traversal, each edge also has a special field of 
   integer, _is_ready, which can be used for bookkeeping.
  */
class Edge {
 friend class Vertex;

 protected:
  string  _name;
  
  vector<Vertex*>   _targets;
  vector<Vertex*>   _sources;

  TimingInfo   _R;
  TimingInfo   _F;

  int _is_ready;

  // internal use only
  unsigned _cur;

  // private methods
  void AddTarget(Vertex* v) { _targets.push_back(v); }
  void AddSource(Vertex* v) { _sources.push_back(v); }

 public:
  Edge(const char* name);
  virtual ~Edge() {}

  // Query
  const char* Name() { return _name.c_str(); }
  int NumTargets() const { return _targets.size(); }
  int NumSources() const { return _sources.size(); }
  int &IsReady() { return _is_ready; }
  virtual int IsPin() const { return 0; }

  // Iterators
  Vertex* FirstTarget();
  Vertex* NextTarget();
  Vertex* FirstSource();
  Vertex* NextSource();

  // Access to timing info
  TimingInfo& Rising()  { return _R; }
  TimingInfo& Falling() { return _F; }

  // Debug
  void Dump(FILE *f);
};

/* Vertex in the graph 
   Each vertex maintains a list of "IN" edges and "OUT" edges. Note that the 
   vertex doesn't have any timing information. However, it does have a field 
   _cell, which is the index of the corresponding cell characterization in 
   the library. 
 */
class Vertex {
 protected:
  string    _name;          // instance name
  EdgeType  _type;          // type of the instance
  int       _is_inverting;  // derived from the type

  vector<Edge*>   _in_edges;
  vector<Edge*>   _out_edges;
  int             _cell;      // record the cell characterization entry 

  unsigned        _cur;       // internal use only

 public:
  Vertex(const char* cell_name, const char* instance_name, int cell);
  ~Vertex() {};

  // Modification, note the edges are also modified as well
  void AddInEdge(Edge* e);
  void AddOutEdge(Edge* e);

  // Query
  const char *Name() { return _name.c_str(); }
  EdgeType Type() const { return _type; }
  
  /* Whether a vertex is inverting or not. Any gate start with N will be
     inverting, otherwise it is non-inverting */
  int IsInverting() const { return _is_inverting; }

  int NumInEdges()  const { return _in_edges.size(); }
  int NumOutEdges() const { return _out_edges.size(); }
  int TheCell()     const { return _cell; }
  char *CellType();

  /* Special method to check how many IN or OUT edges have Ready set*/
  int NumInEdgesReady();
  int NumOutEdgesReady();

  // Iterators
  Edge* FirstInEdge();
  Edge* NextInEdge();
  Edge* FirstOutEdge();
  Edge* NextOutEdge();

  // Debug
  void Dump(FILE *f);
};

/* Believe it or not, pins are treated as edges. So it inherits all the 
   properties of edges, plus its own.
 */
class Pin : public Edge {
 protected:
  string   _name;
  PinType  _type;
  
 public:
  Pin(const char* name, PinType t):Edge(name),_type(t) {}
  ~Pin() {};

  /* Modification */
  void AddTarget(Vertex *v);
  void AddSource(Vertex *v);

  /* Add AT (arrivial time) and RT (required time) at each pin */
  void AddAT(double tr, double tf);
  void AddRT(double tr, double tf);

  // Query
  PinType Type()  const { return _type; }
  int     IsPin() const { return 1; }

  // Iterator - use Edge iterators

  // Debug
  void Dump(FILE *f);
};

/* Timing Grpah:
   Each timing graph is a collection of vertices and edges, which are 
   properly interconnected
 */
class TimingGraph {
 private:
  map<string, Edge*>   _edge_hash;
  map<string, Vertex*> _vertex_hash;
  map<string, Pin*>    _pin_hash;

  // private use only
  map<string, Pin*>::iterator    _p_cur;
  map<string, Vertex*>::iterator _v_cur;
  map<string, Edge*>::iterator   _e_cur;

 public:
  TimingGraph();
  ~TimingGraph();

  /* The difference between LocatePin and CreatePin is that LocatePin will 
     return NULL if a particular pin is not found, while CreatePin will 
     create a new one if it is not found. */
  Pin* LocatePin(const char* name);
  Pin* CreatePin(const char* name, PinType t);
  /* Ditto */
  Edge* LocateEdge(const char* name);
  Edge* CreateEdge(const char* name);
  /* Ditto */
  Vertex* LocateVertex(const char* name);
  Vertex* CreateVertex(const char* name, const char *instance, int cell);

  /* Easier way to get all */
  Edge* LocateEdgeOrPin(const char* name);

  /* Setup procedures */
  void SetupPrimaryInputs();    // tag all PI pins as ready
  void SetupPrimaryOutputs();   // tag all PO pins as ready

  /* Clears the _is_ready flag for each edge */
  void Reset();

  // Iterators
  Pin* FirstPin(PinType pt); 
  Pin* NextPin(PinType pt);

  Edge* FirstEdge();
  Edge* NextEdge();

  Vertex* FirstVertex();
  Vertex* NextVertex();

  /* Loading a graph from a text file  */
  void LoadGraph(ifstream& f, Library *L);
  // void LoadGraph(FILE *f, Library *L);

  /* Print for debugging */
  void Dump(FILE *f, const char* name="NoName");
};

typedef queue<Vertex*> VertexQ;
typedef stack<Edge*>   EdgeS;

#endif
