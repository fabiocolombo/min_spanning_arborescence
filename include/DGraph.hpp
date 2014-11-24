#ifndef DGRAPH_HPP_INCLUDED
#define DGRAPH_HPP_INCLUDED
#include <iostream>
#include <set>
#include <vector>

/**Describe an arc in a weighted directed graph*/
struct Arc{
    int from;
    int to;
    double cost;
    Arc():from(-1),to(-1),cost(0){}
    Arc(int from,int to, double cost):from(from),to(to),cost(cost){}
    double getCost() const {return cost;}
};

inline bool operator<(const Arc& a1, const Arc& a2){
    return a1.from < a2.from || (a1.from==a2.from && a1.to<a2.to);
}

struct ArcPtrCmp{
    bool operator()(const Arc* const a1Ptr,const Arc* const a2Ptr) const{
        return *a1Ptr < *a2Ptr;
    }
};

typedef std::set<Arc*,ArcPtrCmp> ASet;

/**Print an Arc using the format: (from,to):cost*/
std::ostream& operator<<(std::ostream& os, const Arc& a);

/**Describe a weighted directed graph using adjacency lists*/
struct DGraph{
private:
    ASet arcs;
    std::vector< ASet > outcut;
    std::vector< ASet > incut;
public:
    DGraph();
    DGraph(int n);
    void resize(int n);
    DGraph& addArc(int from,int to,double cost);
    DGraph& updateArc(int from,int to, double newCost);
    const ASet& getOutcut(int node) const;
    const ASet& getIncut(int node) const;
    const ASet& getArcs() const;
    bool contains(int from, int to) const;
    int getM() const;
    int getN() const;
    ~DGraph();
};

/**Read a DGraph with the standard format used by benchmarks
   Steiner tree instances*/
std::istream& operator>>(std::istream& is, DGraph& g);
/**Write a DGraph with the standard format used by benchmarks
   Steiner tree instances*/
std::ostream& operator<<(std::ostream& os, const DGraph& g);



#endif // DGRAPH_HPP_INCLUDED