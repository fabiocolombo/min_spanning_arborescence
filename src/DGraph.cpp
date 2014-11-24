#include "DGraph.hpp"
#include <cassert>
using namespace std;

std::ostream& operator<<(std::ostream& os, const Arc& a){
    return os<<"("<<a.from<<","<<a.to<<":"<<a.cost<<")";
}

istream& operator>>(istream& is, DGraph& g){
    int n,m;
    is>>n>>m;
    g.resize(n);    
    for(int i=0;i<m;++i){
        int from,to;
        double cost;
        Arc *a=new Arc();
        is>>from>>to>>cost;
        g.addArc(from-1,to-1,cost);
    }
    return is;
}

ostream& operator<<(std::ostream& os, const DGraph& g){
    os<<g.getN()<<" "<<g.getM()<<endl;
    for(ASet::const_iterator ait=g.getArcs().begin();ait!=g.getArcs().end();++ait){
        Arc& a=**ait;
        os<<a.from+1<<" "<<a.to+1<<" "<<a.cost<<endl;
    }
    return os;
}

DGraph::DGraph(){  }

DGraph::DGraph(int n):outcut(n),incut(n)
{}

void DGraph::resize(int n){
    outcut.resize(n);
    incut.resize(n);
}

DGraph& DGraph::addArc(int from, int to, double cost){
    Arc *newArc=new Arc(from,to,cost);
    arcs.insert(newArc);
    outcut[from].insert(newArc);
    incut[to].insert(newArc);
    return *this;
}

bool DGraph::contains(int from, int to) const{
    Arc a(from,to,0.0);
    return arcs.find(&a)!=arcs.end();
}

DGraph& DGraph::updateArc(int from, int to, double newCost){
    Arc a(from,to,newCost);
    ASet::iterator it=arcs.find(&a);
    assert(it!=arcs.end());
    Arc& oa=(**it);
    oa.cost=newCost;
    return *this;
}

const ASet& DGraph::getOutcut(int node) const{
    return outcut[node];
}

const ASet& DGraph::getIncut(int node) const{
    return incut[node];
}

const ASet& DGraph::getArcs() const{
    return arcs;
}

int DGraph::getM() const{
    return arcs.size();
}

int DGraph::getN() const{
    return outcut.size();
}

DGraph::~DGraph(){
    for(ASet::iterator ait=arcs.begin(); ait!=arcs.end(); ++ait){
        delete *ait;
    }
}
