#ifndef HEAP_H
#define HEAP_H
#include<iostream>
#include<vector>
#include<assert.h>
#include<iterator>
#include<string>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<algorithm>
#include<stack> 
using namespace std;

template<class T>
struct Less{
    bool operator()(const T & left , const T & right)
    {
        return left < right;
    }
};

template<class T>
struct Greater {
     bool operator()(const T & left , const T & right)
     {
         return left > right;
     }
};

template<class T , class compare=Less<T> >
struct Heap
{
   public: 
        Heap(const T array [] , size_t length)
            :_heap(std::begin(array),std::end(array+length))
        {
            int node = (length-1-1)>>1;
            for(int i = node; i >= 0 ; --i)
            {
                _adjustdown(node);
            }
        }
        Heap()
            :_heap()
        {}
        void insert( T data)
        {
            _heap.push_back(data);
            if(_heap.size()>1)
            {
                _adjustup();
            }
        }
        void pop()
        {
            if(!empty())
            {
                if(size()>1)
                {
                    std::swap(_heap.front(),_heap.back());
                    _heap.pop_back();
                    _adjustdown(0); 
                }
                else 
                {
                    _heap.pop_back();    
                }
            }
        }
        const T& Top()
        {
            return _heap[0];
        }
        bool empty()
        {
            return _heap.empty();
        }
        size_t size()
        {
            return _heap.size();
        }
   private:
        void _adjustup()
        {
            int child  =_heap.size()-1;
            int parent = (child - 1)>>1;
            compare com;
            while(parent >=0)
            {
                 if(com(_heap[parent],_heap[child]))
                 {
                     std::swap(_heap[parent],_heap[child]);
                 }
                 child=parent;
                 parent=(child-1)>>1;
            }
        }
        void _adjustdown(int root)
        {
            assert(root>=0);
            int parent=root;
            int size = _heap.size();
            int child = 2*parent+1;
            while(child < size)
            {
                compare com;
                if(child+1<size&&com(_heap[child],_heap[child+1]))
                {
                    child=child+1;
                }
                if(com(_heap[parent],_heap[child]))
                {
                    std::swap(_heap[parent],_heap[child]);
                }
                else 
                {
                   break;
                }
                parent=child;
                child=2*parent+1;
            }
        }
        vector<T> _heap;
};
#endif
