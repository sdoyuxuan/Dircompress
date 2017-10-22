#include "Heap.h" 
template<class T>
struct HuffmanTreeNode
{
    HuffmanTreeNode(const T & weight)
    :_weight(weight),_pLeft(NULL),_pRight(NULL),_pParent(NULL)
    {}
    
    T _weight;
    HuffmanTreeNode<T>*_pLeft;
    HuffmanTreeNode<T>*_pRight;
    HuffmanTreeNode<T>*_pParent;
};

template<class T>
class HuffmanTree
{
  protected:
    using Node = HuffmanTreeNode<T>;
  public:
    HuffmanTree()
        :_pRoot(NULL)
    {}
    template<size_t N>
    HuffmanTree(const T(&array)[N],const T & invalild)
    {
        _create(array,invalild);
    }
    HuffmanTree(const HuffmanTree<T>&)=delete;
    ~HuffmanTree()
    {
        _destroy(_pRoot);
    }
    Node*const GetRoot()
    {
        return _pRoot; 
    }
  protected:
    template<size_t N>
    void _create(const T (&array)[N], const T & invalild)
    {
         struct compare
         {
           bool  operator()(const Node *left ,const Node *right)
             {
                return  left->_weight > right->_weight;//min_heap
             }
         };
         Heap<Node*,compare> min_heap;//build heap
         for(auto & i:array)
         {
             if(i!=invalild)
             {
                  min_heap.insert(new Node(i));      
             }
         }
         if(min_heap.size()==0)
         {
             _pRoot=nullptr;// chu li kong wen jian
             return;
         }
         while(min_heap.size()>1)
         {
             Node * pLeft = min_heap.Top();
             min_heap.pop();
             Node * pRight = min_heap.Top();
             min_heap.pop();
             Node * parent = new Node(pLeft->_weight+pRight->_weight);
             parent->_pLeft=pLeft;
             parent->_pRight=pRight;
             pLeft->_pParent=parent;
             pRight->_pParent=parent;
             min_heap.insert(parent);
         }
         _pRoot=min_heap.Top();
    }
    void _destroy(Node*pRoot)
    {
         if(pRoot)
         {
             _destroy(pRoot->_pLeft);
             _destroy(pRoot->_pRight);
             delete pRoot;
             pRoot=nullptr;
         }
    }
  protected:
    Node*_pRoot;
};
