// $Id: circuit.h,v 1.1.1.1 2004/07/06 20:40:39 cvsdevel Exp $
//    Frank Liu    draft     11/01/2003
/**************************************************************************
 *
 * circuit class: container of an on-chip power grid
 *    
 *   A power grid is represended by a undirected graph
 *   A node represents a circuit node and may have any of the two fields:
 *       LOAD - the current load
 *       DCAP - the decoupling capacitance
 *   An edge represents a wire segment connecting the circuit nodes. It has a 
 *       resistance and an inductance. The resistance and inductance are in 
 *       series. Some wire segments may have zero inductance. But all the wire 
 *       segments must have non-zero resistance
 *
 *   Query methods are provided to access the node and wire properties, such as 
 *   loading current, amount of decoupling caps, node supply voltages, wire 
 *   resistance and wire inductance.
 *   
 *   Traversal the circuit is done via GTL methods. Please see GTL 
 *   documentation for details. Some of them are listed below:
 *      adj_edges_begin();
 *      adj_edges_end();
 *      adj_nodes_begin();
 *      adj_nodes_end();
 *      in_edges_begin();
 *      in_edges_end();
 *      out_edges_begin();
 *      out_edges_end();
 *      inout_edges_begin();
 *      inout_edges_end();
 *      all_edges();
 *      all_nodes();
 *
 **************************************************************************/

#ifndef _CIRCUIT_H
#define _CIRCUIT_H

#include <vector>
#include <map>
#include <GTL/graph.h>

using namespace std;

// command types
enum DType { WIRE=0, DCAP, LOAD, SUPP, SOLV, UNKNOWN };

class WireProperties {
 public:
  WireProperties():_res(0.0),_ind(0.0) {}
  WireProperties(double r, double l):_res(r),_ind(l) {} 
  ~WireProperties() {};
  double &Res() { return _res; };
  double &Ind() { return _ind; };

 private:
  double    _res;
  double    _ind;

};

// class eqstr:public binary_function<char *, char *, bool> {
class eqstr {
  public:
    bool operator() (const char* s1, const char* s2) const {
      // cout << "s1 = " << s1 << " s2 = " << s2 << endl;
      return (strcmp(s1, s2) < 0 ); 
    }
};

class Circuit : public graph {
 public:
  Circuit() : graph() {
    _loads.init(*this);
    _wires.init(*this);
    _dcaps.init(*this);
    _supps.init(*this);
  };

  ~Circuit();
  
  // returns the total number of nodes in the ckt
  int NumNodes() const  { return number_of_nodes(); };
  // returns the total number of edges in the ckt
  int NumWires() const  { return number_of_edges(); };

  // methods to add or query nodes
  inline int NodeExists(const char *name) { 
    return ( _nodes.find(name) == _nodes.end() ? -1 : 0 );
  };
  // locate the node, exit if the node does not exist
  node FindNode(const char *);
  // locate a node if it already exists, otherwise create one
  node FindOrCreateNode(const char*);      

  // methods to add elements in the circuit
  // load the circuit from a file, see doc for the file format
  int LoadCircuit(FILE *fp);
  void InsertDCAP(char *n_name, double val);  // insert a decoupling cap
  void InsertLOAD(char *n_name, double val);  // insert a load (i source)
  void InsertSUPP(char *n_name, double val);  // insert a supply (v source)
  // insert a wire segment
  void InsertWIRE(char *f_name, char *t_name, double r_val, double l_val);
  void InsertSOLV(char *n_name);              // tag the node to be solved

  // methods to query the node
  inline double NodeLOAD(node n) { return _loads[n]; };
  inline double NodeDCAP(node n) { return _dcaps[n]; };
  inline double NodeSUPP(node n) { return _supps[n]; };

  // methods to query the wire
  inline double WireRES(edge e)  { return _wires[e].Res(); };
  inline double WireIND(edge e)  { return _wires[e].Ind(); };

  // method to solve the voltage
  void   GetAllVoltages(int howmany=5000);
  double GetVoltage(const char *name, int howmany=5000);

 private:

  typedef edge_map<WireProperties>                               WireList;
  typedef node_map<double>                                       NodeList;
  typedef map<const char*, node, eqstr>  NodeMap;
  typedef vector<const char*>                                    NameVec;

  NodeMap        _nodes;             // node map
  WireList       _wires;             // wires and their properties
  NodeList       _loads;             // loads
  NodeList       _dcaps;             // decaps
  NodeList       _supps;             // supplies
  NameVec        _what;              // names of nodes to calculate

};



#endif

/* end */
