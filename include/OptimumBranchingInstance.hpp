#ifndef OPTIMUMBRANCHINGINSTANCE_HPP_INCLUDED
#define OPTIMUMBRANCHINGINSTANCE_HPP_INCLUDED
#include <iostream>
#include <list>
#include <vector>

struct Arc{
    int from;
    int to;
    double cost;
    Arc():from(-1),to(-1),cost(0){}
    Arc(int from,int to, double cost):from(from),to(to),cost(cost){}
    double getCost() const {return cost;}
};

std::ostream& operator<<(std::ostream& os, const Arc& a);

struct DGraph{
    int m,n;
    std::list< Arc* > arcs;
    std::vector< std::list<Arc*> > outcut;
    std::vector< std::list<Arc*> > incut;
    ~DGraph();
};

std::istream& operator>>(std::istream& is, DGraph& g);
std::ostream& operator<<(std::ostream& os, const DGraph& g);



#endif // OPTIMUMBRANCHINGINSTANCE_HPP_INCLUDED
