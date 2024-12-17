#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED
#include <queue>
#include <utility>
#include "Node.h"
#include "TypedFile.h"

using namespace std;

template <class T, const unsigned int min_degree>
class Btree : public TypedFile<T, min_degree>
{
public:
    Btree(const string name, const string type, const unsigned int ver);
    virtual ~Btree();

    void printTreeB();
    void printByLevel();

    pair<Node<T, min_degree>, int> bTreeSearch(const Node<T, min_degree>& x, const T& k);
    Node<T, min_degree> diskReadNo(const long long int i);

    void bTreeSplitChild(Node<T, min_degree>& x,const int i, long long int indexX);
    void bTreeInsertNonfull(Node<T, min_degree> x, T k, long long int indexX);
    void bTreeInsert(T k);

    void bTreeDelete(T k);
    void bTreeRemove(Node<T, min_degree> x, T k, long long indexX);
    T getPredecessor(Node<T, min_degree> y);
    T getSucessor(Node<T, min_degree> z);
    void mergeNodes(Node<T, min_degree>& x, int indexX, int i, Node<T, min_degree>& y, int& indexY, Node<T, min_degree>& z, int indexZ);
    bool borrowFromLeft(Node<T, min_degree>& x, long long int indexX, long long int i, Node<T, min_degree>& c, long long int indexC, Node<T, min_degree>& childrenLeft, long long int indexChildrenLeft);
    bool borrowFromRight(Node<T, min_degree>& x, long long int indexX, long long int i, Node<T, min_degree>& c, long long int indexC, Node<T, min_degree>& childrenRight, long long int indexChildrenRight);
private:
    Node<T, min_degree> root;    //a raiz da B-�rvore est� sempre na mem�ria principal
};

#endif // BTREE_H_INCLUDED