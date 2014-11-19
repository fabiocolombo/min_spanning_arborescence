#ifndef DISJOINTSETS_HPP_INCLUDED
#define DISJOINTSETS_HPP_INCLUDED
#include <list>
#include <map>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <cassert>
#include <IO.hpp>


template<class T> class DisjointSets;
template<class T> std::ostream& operator<<(std::ostream& os, const DisjointSets<T>& ds);

template<class T> class DisjointSets{
    public:

    struct Node{
        T value;
        Node* p;
        int rank;
    };


    friend std::ostream& operator<< <>(std::ostream& os, const DisjointSets<T>& ds);
//
//    private:
    typedef boost::unordered_map<T,Node*> NodeMap;
    NodeMap nodeMap;

    private:
    void linkSets(Node *no1Ptr, Node *no2Ptr);
    void unionSets(Node *no1Ptr, Node *no2Ptr);
    Node *findNode(Node *noPtr) const;


    public:
    ~DisjointSets();
    void makeSet(const T& value);
    void unionSets(const T& value1, const T& value2);
    const T& findSet(const T& value) const;
};


template<class T> void DisjointSets<T>::makeSet(const T& value){
    Node *noPtr=new Node();
    noPtr->p=noPtr;
    noPtr->rank=0;
    noPtr->value=value;
    nodeMap[value]=noPtr;
}

template<class T> void DisjointSets<T>::unionSets(Node *no1Ptr, Node *no2Ptr){
    linkSets(findNode(no1Ptr),findNode(no2Ptr));
}

template<class T> void DisjointSets<T>::linkSets(Node *no1Ptr, Node *no2Ptr){
    if( no1Ptr->rank > no2Ptr->rank ){
        no2Ptr->p=no1Ptr->p;
    }
    else{
        no1Ptr->p=no2Ptr->p;
        if(no1Ptr->rank == no2Ptr->rank){
            no2Ptr->rank++;
        }
    }
}

template<class T> typename DisjointSets<T>::Node* DisjointSets<T>::findNode(DisjointSets<T>::Node *noPtr) const{
    if( noPtr->p != noPtr ) return findNode(noPtr->p);
    else return noPtr;
}

template<class T> void DisjointSets<T>::unionSets(const T& value1, const T& value2){
    typename NodeMap::iterator it1=nodeMap.find(value1);
    typename NodeMap::iterator it2=nodeMap.find(value2);
    assert(it1!=nodeMap.end());
    assert(it2!=nodeMap.end());
    unionSets(it1->second,it2->second);
}

template<class T> const T& DisjointSets<T>::findSet(const T& value) const{
    typename NodeMap::const_iterator it=nodeMap.find(value);
    assert(it!=nodeMap.end());
    Node *noPtr=findNode(it->second);
    return noPtr->value;
}

template<class T> DisjointSets<T>::~DisjointSets(){
    for(typename NodeMap::iterator it=nodeMap.begin();
        it!=nodeMap.end();++it)
    {
        delete it->second;
    }
}

template<class T> std::ostream& operator<<(std::ostream& os, const DisjointSets<T>& ds){
        std::map< T,std::list<T> > sets;
        for(typename DisjointSets<T>::NodeMap::const_iterator it=ds.nodeMap.begin();
            it!=ds.nodeMap.end();++it)
        {
            typename DisjointSets<T>::Node *noPtr=ds.findNode(it->second);
            sets[noPtr->value].push_back(it->first);
        }
        for(typename std::map< T,std::list<T> >::iterator it=sets.begin();
            it!=sets.end();++it)
        {
            util::IO::printlnSTLCollection(it->second,os);
        }
        return os;
}




#endif // DISJOINTSETS_HPP_INCLUDED
