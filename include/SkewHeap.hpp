//Implementation of Skew Heap
//As described by
//Sleator, Daniel Dominic, and Robert Endre Tarjan. "Self-adjusting heaps." SIAM Journal on Computing 15.1 (1986): 52-69.
#include <iostream>
#include <list>
#include <cassert>
#include "IO.hpp"

template<class T> class SkewHeap;
template <class T> std::ostream& operator<<(std::ostream& os, const SkewHeap<T>& sh);

template<class T> class SkewHeap{

    friend std::ostream& operator<< <> (std::ostream& os, const SkewHeap<T>& sh);

    struct Node{
        Node* left;
        Node* right;
        T value;
        T constant;
    };

    T zero;
    Node *root;
    Node* unify(Node *no1Ptr,Node *no2Ptr);
    void destroy(Node *no);

    public:
    SkewHeap();
    T findMin();
    void deleteMin();
    void addConstant(const T& value);
    void insert(const T& value);
    //make sh empty
    void unify(SkewHeap& sh);
    bool empty();
    ~SkewHeap();
};

template<class T> bool SkewHeap<T>::empty(){
    return root==0;
}

template<class T> SkewHeap<T>::SkewHeap():root(0),zero(T())
{}

template <class T> typename SkewHeap<T>::Node* SkewHeap<T>::unify(typename SkewHeap<T>::Node *no1Ptr,
                                                                  typename SkewHeap<T>::Node *no2Ptr)
{
    if(no1Ptr==0) return no2Ptr;
    if(no2Ptr==0) return no1Ptr;

    if(no1Ptr->constant != zero){
        if(no1Ptr->right!=0)
            no1Ptr->right->constant+= no1Ptr->constant;
        if(no1Ptr->left!=0){
            no1Ptr->left->constant+= no1Ptr->constant;
        }
        no1Ptr->value += no1Ptr->constant;
        no1Ptr->constant= zero;
    }

    if(no2Ptr->constant != zero){
        if(no2Ptr->right!=0)
            no2Ptr->right->constant += no2Ptr->constant;
        if(no2Ptr->left!=0){
            no2Ptr->left->constant += no2Ptr->constant;
        }
        no2Ptr->value += no2Ptr->constant;
        no2Ptr->constant= zero;
    }

    Node *buf;
    if(no1Ptr->value > no2Ptr->value){
        buf=no1Ptr;
        no1Ptr=no2Ptr;
        no2Ptr=buf;
    }
    buf=no1Ptr->right;
    no1Ptr->right=no1Ptr->left;
    no1Ptr->left=unify(buf,no2Ptr);
    return no1Ptr;
}


template<class T> T SkewHeap<T>::findMin(){
    assert(root!=0);
    T ris=root->value;
    ris+=root->constant;
    return ris;
}

template<class T> void SkewHeap<T>::deleteMin(){
    Node *noPtr=root;
    if(noPtr->left!=0)
        noPtr->left->constant+=noPtr->constant;
    if(noPtr->right!=0)
        noPtr->right->constant+=noPtr->constant;
    root=unify(noPtr->left,noPtr->right);
    delete noPtr;
}

template<class T> void SkewHeap<T>::addConstant(const T& value){
    if(root!=0)
        root->constant=root->constant += value;
}

template<class T> void SkewHeap<T>::unify(SkewHeap& sh){
    root=unify(root,sh.root);
    sh.root=0;
}

template<class T> void SkewHeap<T>::insert(const T& value){
    Node *noPtr=new Node();
    noPtr->left=noPtr->right=0;
    noPtr->value=value;
    root=unify(root,noPtr);
}

template<class T> void SkewHeap<T>::destroy(typename SkewHeap<T>::Node *noPtr){
    typename SkewHeap<T>::Node *rightPtr=noPtr->right;
    typename SkewHeap<T>::Node *leftPtr=noPtr->left;
    delete noPtr;
    if(rightPtr!=0) destroy(rightPtr);
    if(leftPtr!=0) destroy(leftPtr);
}

template<class T> SkewHeap<T>::~SkewHeap(){
    if(root!=0)
        destroy(root);
}

template <class T> std::ostream& operator<<(std::ostream& os, const SkewHeap<T>& sh){
    std::list<T> data;
    std::list< typename SkewHeap<T>::Node* > queue;
    if(sh.root!=0)
        queue.push_back(sh.root);
    while(!queue.empty()){
        typename SkewHeap<T>::Node *noPtr= queue.front();
        queue.pop_front();
        if(noPtr->right!=0) queue.push_back(noPtr->right);
        if(noPtr->left!=0) queue.push_back(noPtr->left);
        T ris=noPtr->value;
        ris+=noPtr->constant;
        data.push_back(ris);
    }
    os<<"sh=";
    for(typename std::list<T>::iterator it=data.begin();
        it!=data.end();++it)
    {
        T a=*it;
        os<<(a)<<" ";
    }
    return os;
}



