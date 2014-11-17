#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstdlib>
#include <limits>

using namespace std;

struct Arc;

struct AbstractNode{};

struct SuperNode : public AbstractNode{
    list<AbstractNode> nodes;
};

struct Node : public AbstractNode{
    int id;
    list<Arc*> outcut,incut;
};

struct Arc{
    int id;
    Node *from;
    Node *to;
    double cost;
};

struct DGraph{
    vector<Node> nodes;
    vector<Arc> arcs;
};

//parse a DGRAPH
istream& operator>>(istream& is, DGraph& g){
    int n,m;
    is>>n>>m;
    g.nodes.resize(n);
    for(int i=0;i<n;++i){
        g.nodes[i].id=i;
    }
    g.arcs.resize(m);
    for(int i=0;i<m;++i){
        int from,to;
        double cost;
        is>>from>>to>>cost;
        Arc a;
        a.from=&g.nodes[from];
        a.to=&g.nodes[to];
        a.cost=cost;
        a.id=i;
        g.arcs[i]=a;
        g.arcs[i].from->outcut.push_back(&g.arcs[i]);
        g.arcs[i].to->incut.push_back(&g.arcs[i]);
    }
    return is;
}

ostream& operator<<(ostream& os, DGraph& g){
    os<<g.nodes.size()<<" "<<g.arcs.size()<<endl;
    for(vector<Arc>::iterator ait=g.arcs.begin();ait!=g.arcs.end();++ait){
        Arc& a=*ait;
        os<<a.from->id<<" "<<a.to->id<<" "<<a.cost<<endl;
    }
    return os;
}

//compute the partition matroid: for each node choose the
//entering arc having the max cost
//Complexity: O(m)
void computePart(DGraph& g, vector<Arc*>& part){
    for(vector<Node>::iterator nit=g.nodes.begin();
        nit!=g.nodes.end();++nit){
        Node& no= *nit;
        Arc* bestArcPtr=0;
        double maxCost= -numeric_limits<double>::max();
        for(list<Arc*>::iterator ait=no.incut.begin();
            ait!=no.incut.end();++ait)
        {
            Arc& arc= **ait;
            if(arc.cost > maxCost){
                bestArcPtr=&arc;
                maxCost=arc.cost;
            }
        }
        part[no.id]=bestArcPtr;
    }
}

//Return the first cycle in the given partition
//cycle contains the arcs id
//COMPLEXITY O(n)
bool findCycle(DGraph& g, vector<Arc*>& part, list<int>& cycle){
    const int UNSCANNED=-1;
    const int ROOT=-2;
    vector<int> parent(g.nodes.size(),UNSCANNED);
    int startNode=-1;
    int lastArc=-1;
    for(size_t i=0;i<g.nodes.size() && startNode==-1 ;++i){
        Node& no=g.nodes[i];
        if(parent[no.id]!=UNSCANNED) continue;
        parent[no.id]=ROOT;
        Node *currentNodePtr=&no;
        while(part[currentNodePtr->id]!=0){
            Arc& arc= *part[currentNodePtr->id];
            Node *reachedNodePtr=arc.from;
            if(parent[reachedNodePtr->id]!=-1){
                lastArc=arc.id;
                startNode=reachedNodePtr->id;
                break;
            }
            parent[reachedNodePtr->id]=arc.id;
            currentNodePtr=reachedNodePtr;
        }
    }
    if(startNode==-1) return false; //no cycle
    cycle.push_back(lastArc);
    int currentID=g.arcs[lastArc].to->id;
    while(currentID!=startNode){
        Arc& a=g.arcs[parent[currentID]];
        currentID=a.to->id;
        cycle.push_back(a.id);
    }
    return true;
}

//compress the given cycle
void compress(DGraph& g, list<int>& cycle){
    vector<bool> cNode(g.nodes.size(),false);

    //compute delta
    double delta=numeric_limits<double>::max();
    for(list<int>::iterator it=cycle.begin();
        it!=cycle.end();++it)
    {
        Arc& a=g.arcs[*it];
        cNode[a.to->id]=true;
        if(a.cost < delta)
            delta=a.cost;
    }


    for(list<int>::iterator it=cycle.begin();
        it!=cycle.end();++it)
    {
        Arc& a=g.arcs[*it];
        int p=a.to->id;
        for(list<Arc*>::iterator ait=g.nodes[p].incut.begin();
            ait!=g.nodes[p].incut.end();++ait)
        {
            Arc& a1=**ait;
            if(cNode[a1.from->id]==false){ //not in the cycle
                cout<<"UPDATE "<<a1.from->id<<" "<<a1.to->id<<endl;
                cout<<"NEW COST "<<(a1.cost - a.cost + delta)<<endl;
            }
        }
    }





}

int main(int argc, char **argv){
    if(argc!=2){
        cerr<<"Error, usage: "<<argv[0]<<" input_file"<<endl;
        exit(EXIT_FAILURE);
    }
    ifstream is(argv[1]);
    if(!is.good()){
        cerr<<"Unable to read the file:"<<argv[1]<<endl;
        exit(EXIT_FAILURE);
    }
    DGraph g;
    is>>g;
    cout<<g<<endl;

    vector<Arc*> part(g.arcs.size());
    computePart(g,part);
    list<int> cycle;
    findCycle(g,part,cycle);
    for(list<int>::iterator nit=cycle.begin();
        nit!=cycle.end();++nit)
    {
        cout<<(g.arcs[*nit].from->id)<<" "<<(g.arcs[*nit].to->id)<<endl;
    }
    compress(g,cycle);

// PRINT PART
//    for(size_t i=0;i<g.nodes.size();++i){
//        if(sol[i]==0)
//            cout<<"Node"<<i<<": NONE"<<endl;
//        else
//            cout<<"Node"<<i<<":"<<(sol[i]->from->id)<<endl;
//    }

    exit(EXIT_SUCCESS);
}
