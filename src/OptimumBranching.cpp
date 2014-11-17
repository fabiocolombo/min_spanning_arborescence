#include <fstream>
#include <set>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include "OptimumBranchingInstance.hpp"
#include "DisjointSets.hpp"
#include "SkewHeap.hpp"
#include "IO.hpp"
#include "iloGrowableMatrix.hpp"
#include "ilcplex/ilocplex.h"
#include "ilcplex/cplex.h"
#include "floatUtils.hpp"

using namespace std;
using namespace util::IO;
using namespace util::iloGrowableMatrix;
using namespace util::floatUtils;
using namespace boost::random;

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

//bool operator<(const Arc& a1, const Arc& a2){
//    return a1.cost < a2.cost;
//}
//
//bool operator>(const Arc& a1, const Arc& a2){
//    return a1.cost > a2.cost;
//}
//
//bool operator==(const Arc& a1, const Arc& a2){
//    return a1.from==a2.from && a1.to==a2.to && a1.cost==a2.cost;
//}
//
//bool operator!=(const Arc& a1, const Arc& a2){
//    return !(a1==a2);
//}
//
//Arc operator+(const Arc& a1, const Arc& a2){
//    Arc ris=a1;
//    ris.cost+=a2.cost;
//    return ris;
//}
//
//Arc operator+=(Arc& a1, const Arc& a){
//    a1.cost+=a.cost;
//    return a1;
//}

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

double optimumBranchingMIP(DGraph& g, list<Arc>& solution){
    IloEnv env;
    IloModel model(env);
    IloBiIndexedMap<IloBoolVar> x(env,0,1,"x");
    IloTriIndexedMap<IloBoolVar> f(env,0,1,"f");
    IloExpr expr(env);
    int r=0;
    for(list<Arc*>::iterator ait=g.arcs.begin();
        ait!=g.arcs.end();++ait)
    {
        Arc a=**ait;
        expr+=a.cost*x(a.from,a.to);
    }
    IloAdd(model,IloMinimize(env,expr));
    for(int v=0;v<g.n;++v){
        if(v==r) continue;
        for(list<Arc*>::iterator ait=g.arcs.begin();
            ait!=g.arcs.end();++ait)
        {
            Arc a=**ait;
            expr=IloExpr(env);
            IloAdd(model,f(a.from,a.to,v) <= x(a.from,a.to) );
        }
    }

    for(list<Arc*>::iterator ait=g.arcs.begin();
        ait!=g.arcs.end();++ait)
    {
        Arc a=**ait;
        expr=IloExpr(env);
        for(int v=0;v<g.n;++v){
            expr+=f(a.from,a.to,v);
        }
        IloAdd(model,x(a.from,a.to)<=expr);
    }

    for(int v=0;v<g.n;++v){
        if(v==r) continue;
        for(int w=0;w<g.n;++w){
            expr=IloExpr(env);
            for(list<Arc*>::iterator ait=g.outcut[w].begin();
                ait!=g.outcut[w].end();++ait)
            {
                Arc a=**ait;
                expr+=f(w,a.to,v);
            }
            for(list<Arc*>::iterator ait=g.incut[w].begin();
                ait!=g.incut[w].end();++ait)
            {
                Arc a=**ait;
                expr-=f(a.from,w,v);
            }
            if(w==v) IloAdd(model,expr==-1);
            else if(w==r) IloAdd(model,expr==1);
            else IloAdd(model,expr==0);
        }
    }

    for(int v=0;v<g.n;++v){
        expr=IloExpr(env);
        for(list<Arc*>::iterator ait=g.incut[v].begin();
            ait!=g.incut[v].end();++ait)
        {
            Arc& a= **ait;
            expr+=x(a.from,a.to);
        }
        if(v==r) IloAdd(model,expr==0);
        else IloAdd(model,expr<=1);
    }

//    for(int v=0;v<g.n;++v){
//        expr=IloExpr(env);
//        for(list<Arc*>::iterator ait=g.incut[v].begin();
//            ait!=g.incut[v].end();++ait)
//        {
//            Arc& a= **ait;
//            expr+=f(a.from,a.to,v);
//        }
//        if(v==r) IloAdd(model,expr==0);
//        else IloAdd(model,expr<=1);
//    }



//    println(model);

    IloCplex solver(env);
    solver.setOut(env.getNullStream());
    solver.setError(env.getNullStream());
    solver.extract(model);
    solver.solve();
//    f.printStatePositive(solver);
//    x.printStatePositive(solver);

    for(list<Arc*>::iterator ait=g.arcs.begin();
        ait!=g.arcs.end();++ait)
    {
        Arc a= **ait;
        if(gr(solver.getValue(x(a.from,a.to)),0.0,1e-4)){
            solution.push_back(a);
        }
    }
    double obj=solver.getObjValue();
    env.end();
    return obj;
}


#define MSA_OUTPUT

double minimumArborescence(DGraph& g, int r, list<Arc>& solution){
    typedef BArc< Arc, double > A;
    typedef SkewHeap< A > SHeap;
    //data structures declaration
    DisjointSets<int> scc;
    DisjointSets<int> wcc;
    vector< SHeap > sheaps(g.n);
    vector< list<FNode*> > sccDefArcs(g.n);
    A dummy(0);
    vector< A> enter(g.n,dummy);
    list<int> roots;
    double optimumCost=0.0;
    vector< vector<FNode*> > fnodes(g.n);
    list< FNode* > uFNodes;
    for(int i=0;i<g.n;++i)
        fnodes[i].assign(g.n,(FNode*) 0);

#ifdef MSA_OUTPUT
    println("initialise data structures...");
#endif
    //initialise data structures
    for(int i=0;i<g.n;++i){
        scc.makeSet(i);
        wcc.makeSet(i);
        if(i==r) continue; //no arcs in the root node
        for(list<Arc*>::iterator ait=g.incut[i].begin();
            ait!=g.incut[i].end();++ait)
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
    cout<<"Initial optimumCost:"<<optimumCost<<endl;
#ifdef MSA_OUTPUT
    println("done!");
    println("Starting main loop...");
#endif
    do{
        int k= roots.front();
        roots.pop_front();
        if(sheaps[k].empty()) continue;
        A  a=sheaps[k].findMin();
//        println(a);
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
//                    cout<<"JOIN"<<endl;
//                    println(sheaps[newHeapIndex]);
//                    println(sheaps[heapIndex]);
                    sheaps[newHeapIndex].unify(sheaps[heapIndex]);
                    enter[newHeapIndex]=enter[k];
                }
//                cout<<"FINAL HEAP"<<endl;
//                cout<<sheaps[newHeapIndex]<<endl;
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
//                cout<<"AFTER DECREASE HEAP"<<endl;
//                cout<<sheaps[newHeapIndex]<<endl;
//                cout<<"new optimum cost:"<<optimumCost<<endl;
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
        solution.push_back(fnoPtr->a);
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
    for(int i=0;i<g.n;++i){
        for(int j=0;j<g.n;++j){
            if(fnodes[i][j]!=0)
                delete fnodes[i][j];
        }
    }
#ifdef MSA_OUTPUT
    println("done!");
#endif
    for(list<Arc>::iterator ait=solution.begin();ait!=solution.end();++ait){
        Arc& a=*ait;
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
    if(solution.size() != g.n -1){
        cerr<<"NOT ENOUGH ARCS"<<endl;
        abort();
    }
    list<int> queue;
    queue.push_back(r);
    vector< int > visited(g.n,false);
    visited[r]=true;
    while(!queue.empty()){
        int v=queue.front();
        queue.pop_front();
        for(list<Arc>::iterator ait=solution.begin();ait!=solution.end();++ait){
            Arc a= *ait;
            if(a.from!=v) continue;
            if(visited[a.to]==true){
                cerr<<"A CYCLE IS PRESENTE"<<endl;
                abort();
            }
            visited[a.to]=true;
            queue.push_back(a.to);
        }
    }
    for(int i=0;i<g.n;++i){
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

//double optimumBranching(DGraph& g, list<Arc>& solution){
//#ifdef OPTIMUM_BRANCHING_DEBUG
//    cout<<"INIT GRAPH"<<endl;
//    cout<<g<<endl;
//#endif
//    Arc dummyArc;
//    dummyArc.from=dummyArc.to=-1;
//    set<int> roots;
//    DisjointSets<int> scc;
//    DisjointSets<int> wcc;
//    vector< SkewHeap<Arc> > sheaps(g.n);
//    vector< list<Arc> > sccDefArcs(g.n);
//    vector< Arc > enter(g.n,dummyArc);
//    vector< int > min(g.n);
//    vector< FNode* > lambda(g.n);
//    map< pair<int,int>, FNode* > fnodes;
//    for(int i=0;i<g.n;++i){
//        lambda[i]=new FNode();
//        lambda[i]->parent=0;
//        lambda[i]->from=lambda[i]->to=i;
//        fnodes[ make_pair(i,i) ]=lambda[i];
//    }
//    for(list<Arc*>::iterator ait=g.arcs.begin();
//        ait!=g.arcs.end();++ait)
//    {
//        Arc& a= **ait;
//        FNode *fno=new FNode();
//        fno->from=a.from;
//        fno->to=a.to;
//        fno->parent=0;
//        fnodes[make_pair(fno->from,fno->to)]=fno;
//    }
////    set<FNode *> N;
//    for(int i=0;i<g.n;++i){
//        for(list<Arc*>::iterator ait=g.incut[i].begin();
//            ait!=g.incut[i].end();++ait)
//        {
//            sheaps[i].insert(**ait);
//        }
//        scc.makeSet(i);
//        wcc.makeSet(i);
//        enter[i]=dummyArc;
//        roots.insert(i);
//        min[i]=i;
//    }
//    list<Arc> H;
//    set<int> rset;
//    while(!roots.empty()){
//#ifdef OPTIMUM_BRANCHING_DEBUG
//        println("ENTERARCS");
//        printlnSTLCollection(enter);
//        println("ROOTS");
//        printlnSTLCollection(roots);
//#endif
//        int k= (*roots.begin());
//        k=scc.findSet(k);
//        roots.erase(roots.begin());
//        if(sheaps[k].empty()){
//            rset.insert(k);
//            continue;
//        }
//
//        Arc a=sheaps[k].findMin(); //(i,j)
//#ifdef OPTIMUM_BRANCHING_DEBUG
//        cout<<"consider the SCC "<<k<<endl;
//        println(scc);
//        cout<<"consider the entering arc:"<<a<<endl;
//#endif
//        sheaps[scc.findSet(a.to)].deleteMin();
//        if(a.realCost <= 0){
//            rset.insert(k);
//            continue;
//        }
//        else if(scc.findSet(a.from) == scc.findSet(k)){
//            roots.insert(k);
//            fnodes.erase(make_pair(a.from,a.to));
//#ifdef OPTIMUM_BRANCHING_DEBUG
//            cout<<"ERASE FROM FOREST:"<<a<<endl;
//            cout<<"DROP IT!"<<endl;
//#endif
//        }
//        else{
//            H.push_back(a);
//#ifdef OPTIMUM_BRANCHING_DEBUG
//            cout<<"insert the arc:"<<a<<endl;
//#endif
//            //UPDATE FOREST
//            FNode* parent=fnodes[make_pair(a.from,a.to)];
//            if(sccDefArcs[k].empty()){
//                FNode* child=fnodes[make_pair(k,k)];
//                assert(child->parent==0);
//                child->parent=parent;
//                assert(child->parent!=0);
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                cout<<"ADD FOREST ARC:("<<(*parent)<<","<<(*child)<<")"<<endl;
//#endif
//            }
//            else{
//                for(list<Arc>::iterator ait=sccDefArcs[k].begin();
//                    ait!=sccDefArcs[k].end();++ait)
//                {
//                    Arc& a1= *ait;
//                    FNode* child=fnodes[make_pair(a1.from,a1.to)];
//                    assert(child->parent==0);
//                    child->parent=parent;
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                    cout<<"ADD FOREST ARC:("<<(*parent)<<","<<(*child)<<")"<<endl;
//#endif
//                }
//            }
//
//            if( wcc.findSet(a.from) != wcc.findSet(a.to) ){
//                wcc.unionSets(a.from,a.to);
//                enter[k]=a;
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                cout<<"wcc union:"<<endl;
//                cout<<wcc<<endl;
//#endif
//            }
//            else{
//                double val=10000000;
//                Arc a1=a; //(x,y)
//                int vertex=-1;
//                list<Arc> defArcs;
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                cout<<"LOOPS ARCS"<<endl;
//#endif
//                while( a1 != dummyArc){
//                    defArcs.push_back(a1);
//                    if(a1.cost < val){
//                        val=a1.cost;
//                        vertex=min[scc.findSet(a1.to)];
//                    }
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                    cout<<"from SCC:"<<scc.findSet(a1.from)<<endl;
//#endif
//                    a1=enter[scc.findSet(a1.from)];
//                }
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                println("DEFARCS");
//                printlnSTLCollection(defArcs);
//                cout<<"UPDATE SCC "<<k<<" += "<<valArc(val - a.cost)<<endl;
//                println("BEFORE");
//                println(sheaps[k]);
//#endif
//                sheaps[k].addConstant(valArc(val-a.cost));
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                println("AFTER");
//                println(sheaps[k]);
//                cout<<"UPDATE ARCS"<<endl;
//#endif
//                a1=enter[scc.findSet(a.from)];
//
//                while(a1!=dummyArc){
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                    println(a1);
//                    cout<<"UPDATE SCC "<<(scc.findSet(a1.to))<<" += "<<valArc(val - a1.cost)<<endl;
//                    println("BEFORE");
//                    println(sheaps[scc.findSet(a1.to)]);
//#endif
//                    sheaps[scc.findSet(a1.to)].addConstant(valArc(val - a1.cost));
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                    println("AFTER");
//                    println(sheaps[scc.findSet(a1.to)]);
//#endif
//
//                    int heapIndex1=scc.findSet(a1.to);
//                    int heapIndex2=scc.findSet(k);
//
//                    scc.unionSets(heapIndex1,heapIndex2);
//                    int newHeapIndex=scc.findSet(heapIndex1);
//                    if(newHeapIndex==heapIndex1)
//                        sheaps[newHeapIndex].unify(sheaps[heapIndex2]);
//                    else
//                        sheaps[newHeapIndex].unify(sheaps[heapIndex1]);
//                    enter[newHeapIndex]=enter[k];
//
//                    a1=enter[scc.findSet(a1.from)];
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                    cout<<"new Arc"<<endl;
//                    println(a1);
//                    cout<<"newHeapIndex:"<<newHeapIndex<<endl;
//#endif
//                }
//                int finalNewHeapIndex=scc.findSet(k);
//                min[finalNewHeapIndex]=vertex;
//                roots.insert(finalNewHeapIndex);
//                sccDefArcs[finalNewHeapIndex]=defArcs;
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                cout<<"FINAL SCC HEAP:"<<endl;
//                println(sheaps[finalNewHeapIndex]);
//#endif
//
//            }
//        }
//    }
//    for(int k=0;k<g.n;++k){
//        while(!sheaps[k].empty()){
//            Arc a=sheaps[k].findMin();
//            sheaps[k].deleteMin();
//            fnodes.erase(make_pair(a.from,a.to));
//        }
//    }
//#ifdef OPTIMUM_BRANCHING_DEBUG
//    println("RSET");
//    printlnSTLCollection(rset);
//    cout<<"FINAL GRAPH"<<endl;
//    cout<<g<<endl;
//    cout<<"H arcs"<<endl;
//    printlnSTLCollection(H);
//#endif
//
//    set<int> R;
//    set<FNode*> B;
//
//    for(set<int>::iterator rit=rset.begin();rit!=rset.end();++rit){
//        R.insert(min[*rit]);
//    }
//
//#ifdef OPTIMUM_BRANCHING_DEBUG
//    println("R");
//    printlnSTLCollection(R);
//#endif
//    set<FNode*> N;
//    for(map< pair<int,int>, FNode* >::iterator fit=fnodes.begin();
//        fit!=fnodes.end();++fit)
//    {
//        FNode *fnode=fit->second;
//        if(fnode->parent==0){
//            N.insert(fnode);
//        }
//    }
//
//    while(!N.empty()){
//#ifdef OPTIMUM_BRANCHING_DEBUG
//        println("N");
//        printlnSTLPtrCollection(N);
//#endif
//        int v;
//        if(!R.empty()){
//            v= *R.begin();
//            R.erase(R.begin());
//        }
//        else{
//            FNode* fno= *N.begin();
//            B.insert(fno);
//            N.erase(N.begin());
//            v=fno->to;
//        }
//        FNode *fno=lambda[v];
//        while(fno->parent!=0){
//#ifdef OPTIMUM_BRANCHING_DEBUG
//            cout<<"ERASE FROM FOREST:"<<(*fno)<<endl;
//#endif
//            fnodes.erase(make_pair(fno->from,fno->to));
//#ifdef OPTIMUM_BRANCHING_DEBUG
//            cout<<"FOREST"<<endl;
//            printlnSTLCollection(fnodes);
//#endif
//            fno=fno->parent;
//            for(map< pair<int,int>, FNode* >::iterator fit=fnodes.begin();
//                fit!=fnodes.end();++fit)
//            {
//                FNode *fno1=fit->second;
//                if(fno1->from != fno1->to && fno1->parent==fno){
//                    N.insert(fno1);
//#ifdef OPTIMUM_BRANCHING_DEBUG
//                    cout<<"INSERT "<<(*fno1)<<" in N"<<endl;
//#endif
//                    fno1->parent=0;
//                }
//            }
//        }
//#ifdef OPTIMUM_BRANCHING_DEBUG
//        cout<<"ERASE FROM FOREST:"<<(*fno)<<endl;
//#endif
//        fnodes.erase(make_pair(fno->from,fno->to));
//#ifdef OPTIMUM_BRANCHING_DEBUG
//        cout<<"FOREST"<<endl;
//        printlnSTLCollection(fnodes);
//#endif
//        N.erase(fno);
//    }
//#ifdef OPTIMUM_BRANCHING_DEBUG
//    printlnSTLPtrCollection(B);
//#endif
//    double obj=0.0;
//    for(set<FNode*>::iterator fit=B.begin();fit!=B.end();++fit){
//        FNode& fno= **fit;
//        for(list<Arc*>::iterator ait=g.arcs.begin();ait!=g.arcs.end();++ait){
//            Arc& a1= **ait;
//            if(a1.from == fno.from && a1.to == fno.to){
//                solution.push_back(a1);
//                obj+=a1.realCost;
//                break;
//            }
//        }
//
//    }
//    return obj;
//}
//
void randomDGraph(DGraph& g,int n){
    static boost::mt19937 rng;
    boost::uniform_int<> ureal(0,10);
    g.n=n;
    g.outcut.resize(g.n);
    g.incut.resize(g.n);

    int r=0;
    for(int v=0;v<g.n;++v){
        if(v==r) continue;
        for(int w=0;w<g.n;++w){
            if(w==v) continue;
            Arc *aPtr=new Arc();
            aPtr->from=w;
            aPtr->to=v;
            aPtr->cost=ureal(rng);
            g.arcs.push_back(aPtr);
            g.outcut[w].push_back(aPtr);
            g.incut[v].push_back(aPtr);
        }
    }
    g.m=g.arcs.size();
}



//#define TEST

int main(int argc, char** argv){
#ifndef TEST
    fstream is(argv[1]);
    if(!is.good())
        abort();
    DGraph ins;
    is>>ins;
    list<Arc> solutionMIP;
//    double objMIP=optimumBranchingMIP(ins,solutionMIP);
//    cout<<"OBJMIP="<<objMIP<<endl;
    list<Arc> solution;
//    double obj=optimumBranching(ins,solution);
//    solution.clear();
    double objMSA=minimumArborescence(ins,0,solution);
//    cout<<"OBJ="<<obj<<endl;
//    printlnSTLCollection(solutionMIP);
//    printlnSTLCollection(solution);

    cout<<"OBJMSA="<<objMSA<<endl;
#endif
#ifdef RANDOM_GEN
    for(int i=0;i<500;++i){
        DGraph ins;
        cout<<"test #"<<i<<endl;
        string name=string("randomIns/testDG") + boost::lexical_cast<string>(i);
        ifstream is(name.c_str());
        is>>ins;
        list<Arc> solution;
        double obj=optimumBranching(ins,solution);
        ofstream os(name.c_str());
        os<<ins<<","<<obj<<endl;
        os.close();
    }

#endif
#ifdef TEST
    for(int i=0;i<500;++i){
        DGraph ins;
        cout<<"test #"<<i<<endl;
        randomDGraph(ins,10);
        list<Arc> solutionMIP;
        list<Arc> solution;
//        println(ins);
        string name=string("testDG");
        ofstream os(name.c_str());
        os<<ins<<endl;
        os.close();
        double obj=minimumArborescence(ins,0,solution);

//        double objMIP=optimumBranchingMIP(ins,solutionMIP);
    double objMIP=obj;
        printlnSTLCollection(solutionMIP);
        cout<<"obj="<<obj<<",";
        cout<<"objMIP="<<objMIP<<endl;
        println("SOLUTION MIP");
        printlnSTLCollection(solutionMIP);
        println("SOLUTION");
        printlnSTLCollection(solution);
        if(!eq(obj,objMIP)){
            ofstream os("testDG");
            os<<ins<<endl;
            os.close();
            cout<<ins<<endl;
            abort();
        }
    }
#endif

    return 0;
}



