/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

// Methods related to the graph

#include <stdlib.h>
#include "timing_graph.h"
#include "tokenizer.h"
#include "library.h"


inline int STRINGEQ(const char *a, const char* b) 
{
  return (a[0]==b[0])&&(!strcmp(a,b)); 
}


EdgeType Edge_Type (const char* cellname) 
{
  if        (STRINGEQ(cellname, "NOT")) { 
    return (NOT);
  } else if (STRINGEQ(cellname, "NAND2")) { 
    return (NAND2);
  } else if (STRINGEQ(cellname, "NAND3")) { 
    return (NAND3);
  } else if (STRINGEQ(cellname, "NAND4")) { 
    return (NAND4);
  } else if (STRINGEQ(cellname, "NAND5")) { 
    return (NAND5);
  } else if (STRINGEQ(cellname, "NOR2")) { 
    return (NOR2);
  } else if (STRINGEQ(cellname, "NOR3")) { 
    return (NOR3);
  } else if (STRINGEQ(cellname, "NOR4")) { 
    return (NOR4);
  } else if (STRINGEQ(cellname, "NOR5")) { 
    return (NOR5);
  } else if (STRINGEQ(cellname, "AND2")) { 
    return (AND2);
  } else if (STRINGEQ(cellname, "AND3")) { 
    return (AND3);
  } else if (STRINGEQ(cellname, "AND4")) { 
    return (AND4);
  } else if (STRINGEQ(cellname, "AND5")) { 
    return (AND5);
  } else if (STRINGEQ(cellname, "OR2")) { 
    return (OR2);
  } else if (STRINGEQ(cellname, "OR3")) { 
    return (OR3);
  } else if (STRINGEQ(cellname, "OR4")) { 
    return (OR4);
  } else if (STRINGEQ(cellname, "OR5")) { 
    return (OR5);
  } else if (STRINGEQ(cellname, "BUF")) { 
    return (BUF);
  } else {
    return NUM_OF_CELL_TYPES;
  }
}

const char* TypeName(EdgeType e) 
{
  switch (e) {
  case NOT:   return "NOT";
  case NAND2: return "NAND2";
  case NAND3: return "NAND3";
  case NAND4: return "NAND4";
  case AND2:  return "AND2";
  case AND3:  return "AND3";
  case AND4:  return "AND4";
  case NOR2:  return "NOR2";
  case NOR3:  return "NOR3";
  case NOR4:  return "NOR4";
  case OR2:   return "OR2";
  case OR3:   return "OR3";
  case OR4:   return "OR4";
  case BUF:   return "BUF";
  default:    return "ERROR!";
  }
}

inline int CheckInverting(const char* cellname) 
{
  return ( cellname[0] == 'N');
}

Edge::Edge(const char *name) 
{
  _name = string(name);
  _is_ready = 0;
  _targets.clear();
  _sources.clear();
  _cur = 0;
}

Vertex* Edge::FirstTarget() 
{
  _cur=0;
  return (_targets.size() == 0 ? NULL : _targets[_cur++]); 
}

Vertex* Edge::NextTarget() 
{
  return (_cur >= _targets.size() ? NULL : _targets[_cur++]); 
}

Vertex* Edge::FirstSource() 
{
  _cur=0; 
  return (_sources.size() == 0 ? NULL : _sources[_cur++]); 
}

Vertex* Edge::NextSource() 
{ 
  return (_cur >= _sources.size() ? NULL : _sources[_cur++]); 
}

void Edge::Dump(FILE *f) 
{
  Vertex *v;
  fprintf(f,"\tE(%s), IN(",_name.c_str());
  for (v=FirstSource(); v!=NULL; v=NextSource()) 
    fprintf(f,"%s ", v->Name());
  fprintf(f,"), OUT(");
  for (v=FirstTarget(); v!=NULL; v=NextTarget()) 
    fprintf(f,"%s ", v->Name());
  fprintf(f,") R(AT=%g,RT=%g,S=%g)", _R.AT(), _R.RT(), _R.SLACK());
  fprintf(f,"  F(AT=%g,RT=%g,S=%g)", _F.AT(), _F.RT(), _F.SLACK());
  fprintf(f,"\n");
}


Vertex::Vertex(const char* cell_name, const char* instance_name, int cell) 
{
  _type = Edge_Type(cell_name);
  _is_inverting = CheckInverting(cell_name);
  _name = string(instance_name);
  _cell = cell;
  _cur = 0;

  _in_edges.clear();
  _out_edges.clear();
}

int Vertex::NumInEdgesReady() 
{
  int tmp=0;
  
  Edge *e;
  for (e=FirstInEdge(); e; e=NextInEdge()){
    tmp += e->IsReady();
  }
  return tmp;
}

int Vertex::NumOutEdgesReady() 
{
  int tmp=0;
  Edge *e;
  for (e=FirstOutEdge(); e; e=NextOutEdge()){
    tmp += e->IsReady();
  }
  return tmp;
}

void Vertex::AddInEdge(Edge* e) 
{ 
  _in_edges.push_back(e); 
  e->AddTarget(this);
}

void Vertex::AddOutEdge(Edge* e) 
{ 
  _out_edges.push_back(e); 
  e->AddSource(this);
}

Edge* Vertex::FirstInEdge() 
{
  _cur = 0;
  return _in_edges.size() == 0 ? NULL : _in_edges[_cur++]; 
}

Edge* Vertex::NextInEdge() 
{
  return _cur >= _in_edges.size() ? NULL : _in_edges[_cur++]; 
}

Edge* Vertex::FirstOutEdge() 
{
  _cur = 0;
  return _out_edges.size() == 0 ? NULL : _out_edges[_cur++]; 
}

Edge* Vertex::NextOutEdge() 
{ 
  return _cur >= _out_edges.size() ? NULL : _out_edges[_cur++];
}

void Vertex::Dump(FILE *f) 
{
  Edge *e;
  fprintf(f,"\tV(%s, %s, %x), OUT(",TypeName(_type), _name.c_str(),
	  (unsigned int)_cell);
  for (e=FirstOutEdge(); e!=NULL; e=NextOutEdge()) 
    fprintf(f,"%s ", e->Name());
  fprintf(f,"), IN(");
  for (e=FirstInEdge(); e!=NULL; e=NextInEdge()) 
    fprintf(f,"%s ", e->Name());
  fprintf(f,")\n");
}

void Pin::AddTarget(Vertex *v) 
{ 
  if (_type == PO) return; 
  Edge::AddTarget(v); 
}

void Pin::AddSource(Vertex *v) 
{ 
  if (_type == PI) return; 
  Edge::AddSource(v); 
}

void Pin::AddAT(double tr, double tf) 
{
  if ( Type() != PI ) return;
  Rising().AT() = tr;
  Falling().AT() = tf;
}

void Pin::AddRT(double tr, double tf) 
{
  if ( Type() != PO ) return;
  Rising().RT() = tr;
  Falling().RT() = tf;
}

void Pin::Dump(FILE *f) 
{
  Vertex *v;
  if ( _type == PI ) {
    fprintf(f,"\tPI(%s), (", Name());
    for( v=FirstTarget(); v!=0;v=NextTarget()) fprintf(f,"%s ",v->Name());
    fprintf(f,")");
  } else {
    fprintf(f,"\tPO(%s), (", Name());
    for( v=FirstSource(); v!=0;v=NextSource()) fprintf(f,"%s ",v->Name());
    fprintf(f,")");
  }
  fprintf(f,") R(AT=%g,RT=%g,S=%g)", _R.AT(), _R.RT(), _R.SLACK());
  fprintf(f,"  F(AT=%g,RT=%g,S=%g)", _F.AT(), _F.RT(), _F.SLACK());

  fprintf(f,"\n");
}

Pin* TimingGraph::LocatePin(const char* name) 
{
  string n = string(name);
  return  ( (_pin_hash.find(n) == _pin_hash.end() ) ? NULL : _pin_hash[n] ); 
}

Pin* TimingGraph::CreatePin(const char* name, PinType t) 
{
  string n = string(name);
  if  (_pin_hash.find(n) != _pin_hash.end() ) return _pin_hash[n];
  Pin* p=new Pin(name,t);
  _pin_hash[n] = p;
  return p;
}

Edge* TimingGraph::LocateEdge(const char* name) 
{
  string n = string(name);
  return  ( (_edge_hash.find(n) == _edge_hash.end() ) ? NULL : _edge_hash[n] );
}

Edge* TimingGraph::CreateEdge(const char* name) 
{
  string n = string(name);
  if  (_edge_hash.find(n) != _edge_hash.end() ) return _edge_hash[n];
  Edge* e=new Edge(name);
  _edge_hash[n] = e;
  return e;
}


Vertex* TimingGraph::LocateVertex(const char* name) 
{
  string n = string(name);
  return  ( (_vertex_hash.find(n) == _vertex_hash.end() ) ? NULL 
                                                          : _vertex_hash[n] ) ; 
}

Vertex* TimingGraph::CreateVertex(const char* cellname, const char* instance,
				  int cell) 
{
  string n = string(instance);
  if  (_vertex_hash.find(n) != _vertex_hash.end() ) return _vertex_hash[n];
  Vertex* v=new Vertex(cellname, instance, cell);
  _vertex_hash[n] = v;
  return v;
}

TimingGraph::TimingGraph() 
{
  _edge_hash.clear(); 
  _vertex_hash.clear(); 
  _pin_hash.clear();
  _p_cur = _pin_hash.end();
  _v_cur = _vertex_hash.end();;
  _e_cur = _edge_hash.end();
}

TimingGraph::~TimingGraph() 
{
  map<string,Edge*>::iterator ei;
  Edge* e;
  for (ei=_edge_hash.begin(); ei!=_edge_hash.end(); ei++) {
    e = (*ei).second;
    delete e;
  }

  map<string,Vertex*>::iterator vi;
  Vertex *v;
  for (vi=_vertex_hash.begin(); vi!=_vertex_hash.end(); vi++) {
    v = (*vi).second;
    delete v;
  }

  map<string,Pin*>::iterator pi;
  Pin *p;
  for (pi=_pin_hash.begin(); pi!=_pin_hash.end(); pi++) {
    p = (*pi).second;
    delete p;
  }

  _edge_hash.clear();
  _pin_hash.clear();
  _vertex_hash.clear();
}

void TimingGraph::SetupPrimaryInputs() 
{
  Pin *p;
  for (p=FirstPin(PI); p; p=NextPin(PI))  p->IsReady() = 1;
}

void TimingGraph::SetupPrimaryOutputs() 
{
  Pin *p;
  for (p=FirstPin(PO); p; p=NextPin(PO))  p->IsReady() = 1;
}

void TimingGraph::Reset() 
{
  Edge *e;
  for ( e=FirstEdge(); e; e=NextEdge() ) e->IsReady() = 0;
}

Pin* TimingGraph::FirstPin(PinType pt) 
{
  Pin *p = NULL;
  _p_cur = _pin_hash.begin();
  for (; _p_cur!=_pin_hash.end(); _p_cur++){
    p = (*_p_cur).second;
    if ( p->Type() == pt ) break;
    else p = NULL;
  }
  return p;
}

Pin* TimingGraph::NextPin(PinType pt) 
{
  Pin *p = NULL;
  _p_cur++;
  for ( ; _p_cur!=_pin_hash.end(); ++_p_cur){
    p = (*_p_cur).second;
    if ( p->Type() == pt ) break;
    else p = NULL;
  }
  return p;
}

Edge* TimingGraph::FirstEdge() 
{
  Edge *e= NULL;
  _e_cur = _edge_hash.begin();
  if ( _e_cur != _edge_hash.end() ) {
    e = (*_e_cur).second;
  }
  return e;
}

Edge* TimingGraph::NextEdge() 
{
  Edge *e= NULL;
  if ( _e_cur == _edge_hash.end() ) return e;
  if ( ++_e_cur != _edge_hash.end() ) e = (*_e_cur).second;
  return e;
}

Vertex* TimingGraph::FirstVertex() 
{
  Vertex *v= NULL;
  _v_cur = _vertex_hash.begin();
  if ( _v_cur != _vertex_hash.end() ) {
    v = (*_v_cur).second;
  }
  return v;
}

Vertex* TimingGraph::NextVertex() 
{
  Vertex *v= NULL;
  if ( _v_cur == _vertex_hash.end() ) return v;
  if ( _v_cur != _vertex_hash.end() ) v = (*_v_cur).second;
    ++_v_cur;
  return v;
}

void TimingGraph::Dump(FILE *f, const char* name)
{
  fprintf(f,"====== %s ==========\n",name);
  fprintf(f,"Pins\n");
  map<string,Pin*>::iterator pi;
  Pin *p;
  for (pi=_pin_hash.begin(); pi!=_pin_hash.end(); pi++) {
    p = (*pi).second;
    p->Dump(f);
  }

  fprintf(f,"Edges\n");
  map<string,Edge*>::iterator ei;
  Edge* e;
  for (ei=_edge_hash.begin(); ei!=_edge_hash.end(); ei++) {
    e = (*ei).second;
    e->Dump(f);
  }

  fprintf(f,"Vertices \n");
  map<string,Vertex*>::iterator vi;
  Vertex *v;
  for (vi=_vertex_hash.begin(); vi!=_vertex_hash.end(); vi++) {
    v = (*vi).second;
    v->Dump(f);
  }
  
}

Edge* TimingGraph::LocateEdgeOrPin(const char* name) 
{
  Pin *p = LocatePin(name);
  if ( p ) return (Edge*)p;
  Edge *e = CreateEdge(name);
  return e;
}


void TimingGraph::LoadGraph(ifstream& f, Library *L) 
{
  // ssize_t  read;
  // size_t len;
  char     *line = new char[1000]; // char *line=NULL;
  char **tokens;
  int  sz, k;
  if ( !f ) return;
  char *spec, *name;
  string sn, stype, ss;
  string::size_type pos;
  double t1, t2;

  Pin* p;
  Vertex *v;
  Edge *e;

  Tokenizer TK;
  // while ( (read = getline(&line, &len,f)) != -1 ) {
  while ( !f.eof() ) {
    f.getline(line, 1000, '\n');
    sz = TK.Parse(line, &tokens);
    if ( sz > 0 ) {
      k = 0;
      if ( strcmp(tokens[0],"*")==0 || strcmp(tokens[0],"#")==0 || 
	   strcmp(tokens[0],"%")==0 ) continue;
      spec = tokens[k++];
      if ( strcmp(spec, "PI")==0 || strcmp(spec,"pi")==0 ) {
	name = tokens[k++];
	t1 = atof(tokens[k++]);
	t2 = atof(tokens[k++]);
	p = CreatePin(name, PI);
	p->AddAT(t1,t2);
      } else if ( strcmp(spec, "PO")==0 || strcmp(spec,"po")==0 ){
	name = tokens[k++];
	t1 = atof(tokens[k++]);
	t2 = atof(tokens[k++]);
	p = CreatePin(name, PO);
	p->AddRT(t1,t2);
      } else {  // gate specification
	name = tokens[k++];  // break name into Cell+Strength
	sn = string(name);
	pos = sn.find("_", 0);
	stype = sn.substr(0,pos);
	ss = sn.substr(pos+1,1);
	const char* ns = ss.c_str();
	const char* nn = stype.c_str();
	char* gatename = tokens[k++];  // name of the gate
	int l=L->LocateCell(nn, ns[0]);
	if ( l == -1) {
	  fprintf(stderr, 
		  "Unable to locate cell lib %s, strength %c, quiting..\n", 
		  nn, ns[0]);
	  exit(EXIT_FAILURE);
	}
	v = CreateVertex(nn, gatename, l);
	e = LocateEdgeOrPin(spec);
	v->AddOutEdge(e);
	for (int i=3;i<sz;i++) {     // output pins
	  e = LocateEdgeOrPin(tokens[k++]);
	  v->AddInEdge(e);
	}
	
      }
    }

  }
  if (line) free(line);

  // Make sure to set up
  SetupPrimaryInputs();
}
// End
