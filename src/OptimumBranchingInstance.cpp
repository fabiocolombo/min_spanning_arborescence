#include "OptimumBranchingInstance.hpp"
#include "IO.hpp"
using namespace std;
using namespace util::IO;

std::ostream& operator<<(std::ostream& os, const Arc& a){
    return os<<"("<<a.from<<","<<a.to<<":"<<a.cost<<")";
}

istream& operator>>(istream& is, DGraph& g){
    is>>g.n>>g.m;
    g.outcut.resize(g.n);
    g.incut.resize(g.n);
    for(int i=0;i<g.m;++i){
        Arc *a=new Arc();
        is>>a->from>>a->to>>a->cost;
        a->from--;
        a->to--;
        g.arcs.push_back(a);
        g.outcut[a->from].push_back(a);
        g.incut[a->to].push_back(a);
    }
    return is;
}

ostream& operator<<(std::ostream& os, const DGraph& g){
    os<<g.n<<" "<<g.m<<endl;
    for(list<Arc*>::const_iterator ait=g.arcs.begin();ait!=g.arcs.end();++ait){
        Arc& a=**ait;
        os<<a.from+1<<" "<<a.to+1<<" "<<a.cost<<endl;
    }
    return os;
}

DGraph::~DGraph(){
    for(list<Arc*>::iterator ait=arcs.begin(); ait!=arcs.end(); ++ait){
        delete *ait;
    }
}
