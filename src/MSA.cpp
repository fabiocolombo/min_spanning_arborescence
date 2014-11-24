#include "MSA.hpp"
#include "SkewHeap.hpp"
#include "DisjointSets.hpp"
#include "floatUtils.hpp"

using namespace std;
using namespace util::floatUtils;

/**Arc structure used by the Minimum Arborescence algorithm*/
template< typename T, typename V > struct BArc{
    const T *arcPtr;
    V reducedCost;
    BArc() : arcPtr(0),reducedCost(V()) {}
    BArc(const T& arc): arcPtr(&arc),reducedCost(arc.getCost()) {}
    BArc(const V& value): arcPtr(0), reducedCost(value) {}
    bool operator<(const BArc<T,V>& arc1){
        return reducedCost < arc1.reducedCost;
    }
    bool operator>(const BArc<T,V>& arc1){
        return reducedCost > arc1.reducedCost;
    }
    BArc& operator+=(const BArc& arc1){
        reducedCost += arc1.reducedCost;
        return *this;
    }
    bool operator==(const BArc<T,V>& arc1)
    {
        return arcPtr == arc1.arcPtr && reducedCost == arc1.reducedCost;
    }
    bool operator!=(const BArc<T,V>& arc1)
    {
        return arcPtr != arc1.arcPtr || reducedCost != arc1.reducedCost;
    }
    int from(){
        return arcPtr->from;
    }
    int to(){
        return arcPtr->to;
    }
};

template< typename T, typename V > ostream& operator<<(ostream& os, const BArc<T,V>& arc){
    return os<<(*arc.arcPtr)<<",RC:"<<arc.reducedCost;
}



/**Node structure used by the Minimum Arborescence algorithm*/
struct FNode{
    FNode *parent;
    list<FNode*> children;
    int from;
    int to;
    Arc a;
};

ostream& operator<<(ostream& os, const FNode& fno){
    return os<<"["<<fno.from<<","<<fno.to<<"]"<<endl;
}


double minimumArborescence(DGraph& g, int r, DGraph& solution){
    typedef BArc< Arc, double > A;
    typedef SkewHeap< A > SHeap;
    //data structures declaration
    DisjointSets<int> scc;
    DisjointSets<int> wcc;
    vector< SHeap > sheaps(g.getN());
    vector< list<FNode*> > sccDefArcs(g.getN());
    A dummy(0);
    vector< A> enter(g.getN(),dummy);
    list<int> roots;
    double optimumCost=0.0;
    vector< vector<FNode*> > fnodes(g.getN());
    list< FNode* > uFNodes;
    for(int i=0;i<g.getN();++i)
        fnodes[i].assign(g.getN(),(FNode*) 0);

#ifdef MSA_OUTPUT
    println("initialise data structures...");
#endif
    //initialise data structures
    for(int i=0;i<g.getN();++i){
        scc.makeSet(i);
        wcc.makeSet(i);
        if(i==r) continue; //no arcs in the root node
        for(ASet::iterator ait=g.getIncut(i).begin();
            ait!=g.getIncut(i).end();++ait)
        {
            A barc(**ait);
            sheaps[i].insert(barc);
        }
        A minArc=sheaps[i].findMin();
        sheaps[i].addConstant(A(-minArc.reducedCost));
        optimumCost+=minArc.reducedCost;
        roots.push_back(i);
        FNode *fnoPtr=new FNode();
        fnoPtr->from=fnoPtr->to=i;
        fnoPtr->parent=0;
        sccDefArcs[i].push_back(fnoPtr);
        fnodes[i][i]=fnoPtr;
    }
#ifdef MSA_OUTPUT
    println("done!");
    println("Starting main loop...");
#endif
    do{
        int k= roots.front();
        roots.pop_front();
        if(sheaps[k].empty()) continue;
        A  a=sheaps[k].findMin();
        sheaps[k].deleteMin();
        int h=scc.findSet(a.from());
        int kw=wcc.findSet(k);
        int hw=wcc.findSet(a.from());
        if(h == k){
            //Arc intern to the already build super node: drop the arc
            roots.push_back(k);
            continue;
        }
        else{
            //Update the forest
            FNode *fnoPtr=new FNode();
            fnoPtr->from=a.from();
            fnoPtr->to=a.to();
            fnoPtr->parent=0;
            fnoPtr->a=(*a.arcPtr);
            uFNodes.push_back(fnoPtr);
            assert(fnodes[fnoPtr->from][fnoPtr->to]==0);
            fnodes[fnoPtr->from][fnoPtr->to]=fnoPtr;
            for(list<FNode*>::iterator fit=sccDefArcs[k].begin();
                fit!=sccDefArcs[k].end();++fit)
            {
                FNode *childPtr= *fit;
                childPtr->parent=fnoPtr;
                fnoPtr->children.push_back(childPtr);
            }

            //check if a build a new wcc
            if(hw != kw){
                wcc.unionSets(hw,kw);
                enter[k]=a;
            }
            else{//we build a new scc and, consequently a new pseudonode
                list<FNode*> defArcs;
                defArcs.push_back(fnodes[a.from()][a.to()]);
                A a1=a;
                int newHeapIndex=k;
                while(true){
                    int heapIndex=scc.findSet(a1.from());
                    int buf=newHeapIndex;
                    a1=enter[heapIndex];
                    if(a1==dummy) break;
                    defArcs.push_back(fnodes[a1.from()][a1.to()]);
                    scc.unionSets(newHeapIndex,heapIndex);
                    newHeapIndex=scc.findSet(newHeapIndex);
                    heapIndex=(buf==newHeapIndex)?heapIndex:buf;
                    assert(!sheaps[newHeapIndex].empty());
                    assert(!sheaps[heapIndex].empty());
                    sheaps[newHeapIndex].unify(sheaps[heapIndex]);
                    enter[newHeapIndex]=enter[k];
                }
                A minArc=dummy;
                do{
                    if(sheaps[newHeapIndex].empty()) break;
                    A tmp=sheaps[newHeapIndex].findMin();
                    if(scc.findSet(tmp.from())==newHeapIndex)
                        sheaps[newHeapIndex].deleteMin();
                    else
                        minArc=tmp;
                }while(minArc==dummy);
                if(minArc!=dummy){
                    sheaps[newHeapIndex].addConstant(A(-minArc.reducedCost));
                    optimumCost+=minArc.reducedCost;
                }
                roots.push_back(newHeapIndex);
                sccDefArcs[newHeapIndex]=defArcs;
                enter[newHeapIndex]=dummy;
            }
        }
    }while(!roots.empty());
#ifdef MSA_OUTPUT
    println("done!");

    println("Post processing...");
#endif
    list<FNode*> N;
    double bCost=0.0;
    for(list< FNode* >::iterator fit=uFNodes.begin();
        fit!=uFNodes.end();++fit)
    {
        FNode* fnoPtr=*fit;
        if(fnoPtr->parent==0){
            N.push_back(fnoPtr);
        }
    }

    while(!N.empty()){
        FNode* fnoPtr= *N.begin();
        //solution.push_back(fnoPtr->a);
        solution.addArc(fnoPtr->a.from,fnoPtr->a.to,fnoPtr->a.cost);
        N.erase(N.begin());
        FNode* noPtr=fnodes[fnoPtr->to][fnoPtr->to];
        while(noPtr!=0){
            for(list<FNode*>::iterator fit=noPtr->children.begin();
                fit!=noPtr->children.end();++fit)
            {
                FNode* cPtr=*fit;
                if(cPtr->parent==0)
                    continue;
                cPtr->parent=0;
                if(cPtr->from!=cPtr->to)
                    N.push_back(cPtr);
            }
            FNode* buf=noPtr->parent;
            noPtr->parent=0;
            noPtr=buf;
        }
    }
    for(int i=0;i<g.getN();++i){
        for(int j=0;j<g.getN();++j){
            if(fnodes[i][j]!=0)
                delete fnodes[i][j];
        }
    }
#ifdef MSA_OUTPUT
    println("done!");
#endif
    for(ASet::iterator ait=solution.getArcs().begin();ait!=solution.getArcs().end();++ait){
        Arc& a=**ait;
        bCost+=a.getCost();
    }
#ifndef NDEBUG
#ifdef MSA_OUTPUT
    println("Checking solution....");
#endif
    if(!eq(bCost,optimumCost)){
        cerr<<"bCost:"<<bCost<<endl;
        cerr<<"optimumCost:"<<optimumCost<<endl;
        cerr<<"NOT VALID COST"<<endl;
        abort();
    }
    if(solution.getArcs().size() != g.getN() -1){
        cerr<<"NOT ENOUGH ARCS"<<endl;
        abort();
    }
    list<int> queue;
    queue.push_back(r);
    vector< int > visited(g.getN(),false);
    visited[r]=true;
    while(!queue.empty()){
        int v=queue.front();
        queue.pop_front();
        for(ASet::iterator ait=solution.getArcs().begin();ait!=solution.getArcs().end();++ait){
            Arc a= **ait;
            if(a.from!=v) continue;
            if(visited[a.to]==true){
                cerr<<"A CYCLE IS PRESENTE"<<endl;
                abort();
            }
            visited[a.to]=true;
            queue.push_back(a.to);
        }
    }
    for(int i=0;i<g.getN();++i){
        if(!visited[i]){
            cerr<<"NOT REACHED NODE:"<<i<<endl;
            abort();
        }
    }
#ifdef MSA_OUTPUT
    println("Done!");
#endif
#endif
    return optimumCost;
}

