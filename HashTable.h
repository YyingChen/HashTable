#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>
#include<string>
using namespace std;

static size_t GetNextPrime(size_t value)//获取一个比value大的素数
//全局函数尽量要使用静态的，放在不同的cpp会冲突
{
	//使用素数表对齐做哈希表的容量，降低哈希冲突
	const int _PrimeSize= 28;
	static const unsigned long _PrimeList[_PrimeSize] =
	{
		53ul,        97ul,        193ul,       389ul,      769ul,
		1543ul,      3079ul,      6151ul,      12289ul,    24593ul,
		49157ul,     98317ul,     196613ul,    393241ul,   786433ul,
		1572869ul,   3145739ul,   6291469ul,   12582917ul, 25165843ul,
		50331653ul,  100663319ul, 201326611ul, 402653189ul,805306457ul,
		1610612741ul,3221225473ul,4294967291ul
	};

	for (size_t i=0;i<_PrimeSize;++i)
	{
		if (_PrimeList[i]>value)
		{
			return _PrimeList[i];
		}
	}

	return _PrimeList[_PrimeSize-1];
}
template <class K>
struct __HashFunc
{
	size_t operator()(const K& key)
	{
		return key;
	}
};

template <>
struct __HashFunc<string>//特化,优先调用特化版本
{
	static size_t BKDRHash(const char*str)
	{
		unsigned int seed= 131;// 31 131 1313 13131 131313
		unsigned int hash= 0;
		while(*str)
		{
			hash=hash*seed+ (*str++);
		}
		return(hash& 0x7FFFFFFF);
	}
	size_t operator()(const string& key)
	{
		return BKDRHash(key.c_str());//字符串转整形

	}
};
namespace OPEN
{
	enum State
{
	EXIST,
	DELETE,
	EMPTY,
};


template<class K,class V> 
struct HashNode
{
	pair<K,V> _kv;
	State _state;//标记一个位置的状态

	HashNode()
		:_state(EMPTY)
	{}
};


//struct __HashFuncString//仿函数解决字符串哈希算法
//{
//	static size_t BKDRHash(const char*str)
//	{
//		unsigned int seed= 131;// 31 131 1313 13131 131313
//		unsigned int hash= 0;
//		while(*str)
//		{
//			hash=hash*seed+ (*str++);
//		}
//		return(hash& 0x7FFFFFFF);
//	}
//	size_t operator()(const string& key)
//	{
//		return BKDRHash(key.c_str());
//	
//	}
//};


template<class K,class V,class HashFunc=__HashFunc<K>>
class HashTable
{
public:
	HashTable()
		:_n(0)
	{}
	bool Insert(pair<K,V>& kv)
	{
		_CheckCapacity();
		size_t i=1;
        size_t index=_HashFunc(kv.first);//如果k是string/struct就不能用%，仿函数解决
		                                 //将不能取模的转化为取模
		//如何判断位置是否有无数据,删了之后又是怎么知道该位置的数据被删除了---状态标志位
		while (_tables[index]._state==EXIST)
		{
			if (_tables[index]._kv.first==kv.first)
			{
				return false;
			}
			//二次探测
			index+=i*i;
			index=index%_tables.size();
			++i;
			//++index;//线性探测
			//if (index==_tables.size())
			//{
			//	index=0;
			//}
		}
		_tables[index]._kv=kv;
		_tables[index]._state=EXIST;
		++_n;
		return true;
	} 

	HashNode<K, V>* Find(const K&key)
	{
		size_t index=_HashFunc(key);
		while (_tables[index]._state!=EMPTY)//提出了覆载因子，不是满了才增容，所以不会死循环
		{
			if (_tables[index]._kv.first==key)
			{
				if (_tables[index]._state==EXIST)
				{
					return &_tables[index];
				}
			}
			++index;
			if (index==_tables.size())
			{
				index=0;
			}
		}
		return NULL;
	}
	size_t _HashFunc(const K& key)
	{
		HashFunc hash;
        return hash(key)% _tables.size();
	}
	bool Remove(const K& key)
	{
		HashNode<K, V>* node=Find(key);
		if (node)
		{
			node->_state=DELETE;
			return true;
		}
		return false;
	}
	size_t Size()
	{
		return _n;
	}
	size_t Capacity()
	{
		return _tables.size();//真实用的空间
	}
protected:
	
	void _CheckCapacity()
	{
		if(_tables.empty())
		{
			_tables.resize(GetNextPrime(0));//reserve只开空间不变size
			return;
		}
		if (_n*10/_tables.size()==7)//α载荷因子计算到0.7就开始增容
		{
			size_t newSize=_tables.size()*2;
			HashTable<K,V,HashFunc> newHT;//新建一个哈希表
			newHT._tables.resize(newSize);
            for (size_t i=0;i<_tables.size();++i)
            {
				if (_tables[i]._state==EXIST)
				{
					newHT.Insert(_tables[i]._kv);//解决哈希表扩容后数据的位置问题
				}
            }
			_tables.swap(newHT._tables);
		}
	}
protected:
	//pair<K,V>* _tables;//k是数据，v是出现的次数
	//size_t _size;//有可能会扩容
	//size_t _capacity;
	vector<HashNode<K, V>> _tables;//不需要初始化，调此类型的缺省构造函数。
	                          //在初始化列表的初始化，比函数体内的初始化更高效
	size_t _n;//跟vector的size进行区分，表示vector里面真正存了多少个数

};


void TestHashTable()
{
	HashTable<int,int> ht1;
	/*ht1.Insert(make_pair(71,0));
	ht1.Insert(make_pair(72,0));
	ht1.Insert(make_pair(72,0));
	ht1.Insert(make_pair(1122,0));*/
	for (int i=0;i<54;++i)
	{
		ht1.Insert(make_pair(i,i));
	}


	//HashTable<string,string,__HashFuncString>dict;
	HashTable<string,string>dict;
	dict.Insert(make_pair(string("left"),string("左边")));
	dict.Insert(make_pair(string("1111"),string("上边")));
	dict.Insert(make_pair(string("1122"),string("右边")));
	
     HashNode<string,string>*ret =dict.Find("1111");

	 cout<<ret->_kv.second<<endl;
}

}

namespace BUCKET
{
	template <class K,class V>
	struct HashNode
	{
		pair<K,V> _kv;
		HashNode<K,V>* _next;

		HashNode(const pair<K,V>& kv)
			:_kv(kv)
			,_next(NULL)
		{}

	};

	template <class K,class V,class HashFunc>
	class HashTable;

	template <class K,class V,class HashFunc=__HashFunc<K>>
	struct HashTableIterator
	{
       typedef HashNode<K,V> Node;
       
	   typedef HashTableIterator<K,V,HashFunc> Self;

	   Node* _node;

	   HashTable<K,V,HashFunc>* _ht;
	  
	   
		HashFunc hf;
		size_t index;
		size_t i;

		HashTableIterator(Node* node,HashTable<K,V,HashFunc>* _ht)
			:_node(node)
			,_ht(_ht)
			{}
	   pair<K,V>& operator*()
	   {
		   return _node->_kv;
	   }

	   pair<K,V>* operator->()
	   {
		   return &(operator*());
	   }

	   Self& operator++()
	   {
		   //1、如果当前桶没走完，继续走
          if (_node->_next!=NULL)
          {
			  _node=_node->_next;
          }
		  //2、当前桶走完了，找下一次不为空的痛
		  else
		  {
			 
             index = hf(_node->_kv.first)%_ht->_tables.size();
			  _node=NULL;
              for (i=index+1;i<_ht->_tables.size();++i)
              {
                  if (_ht->_tables[i])
                  {
					  _node=_ht->_tables[i];//找到下一个不为空的数据_node改变，如果没找到_node=NULL
					  break;
                  }
              }
		  }
		  return *this; 
	   }
	  bool operator!=(const Self& s)const
	  {
           return _node!=s._node;
	  }
	};

	template <class K,class V,class HashFunc=__HashFunc<K>>
	class HashTable
	{
		typedef HashNode<K,V> Node;
		friend struct HashTableIterator<K,V,HashFunc>;
	public:
		typedef HashTableIterator<K,V,HashFunc> Iterator;
	public:
		HashTable()
			:_n(0)
		{}
		~HashTable()
		{
			for (size_t i=0;i< _tables.size();++i)
			{
				Node*cur=_tables[i];
				while (cur)
				{
					Node* next=cur->_next;
					delete cur;
					cur=next;
				}
				_tables[i]=NULL;
			}
		}
		Iterator Begin()
		{
			for (size_t i=0;i<_tables.size();++i)
			{
				if (_tables[i])
				{
					return Iterator(_tables[i],this);
				}
			}
			return Iterator(NULL,this);//this指向hashtable 
		}
		Iterator End()
		{
		  return Iterator(NULL,this);
		}
		pair<Iterator,bool> Insert(const pair<K,V>& kv)
		{
			_CheckCapacity();
            size_t index=_HashFunc(kv.first,_tables.size());
			Node* cur=_tables[index];
			while(cur)
			{
                if (cur->_kv.first==kv.first)
                {
					return make_pair(Iterator(cur,this),false);
                }
				cur=cur->_next;
			}
			//头插
			Node* tmp=new Node(kv);
			tmp->_next=_tables[index];
			_tables[index]=tmp;

			++_n;
			return make_pair(Iterator(tmp,this),true);

		}
		Node* Find(const K& key)
		{
			size_t index=_HashFunc(key,_tables.size());
			Node* cur=_tables[i];
			while (cur)
			{
				if (cur->_kv.first==key)
				{
					return cur;
				}
				cur=cur->_next;

			}
			return NULL;
		}
		bool Remove(const K&key)
		{
			size_t index=_HashFunc(key,_tables.size());
			Node*prev=NULL;
			Node*cur=_tables[index];
			while (cur)
			{
				if (cur->_kv.first==key)
				{
                     if (prev==NULL)//说明cur是第一个节点
                     {
						 //头删
						 _tables[index]=cur->_next;
                     }
					 else
					 {
						 prev->_next=cur->_next;
					 }
					 delete cur;
					 return true;
				}
				prev=cur;
				cur=cur->_next;
			}
			return false;
        }
		void Resize(size_t size)//如果你提前知道你有多少数据，先把表resize到那么大 避免增容
			                    //提高插入数据的效率
		{
			_tables.resize(GetNextPrime(size));
		}

		V& operator[](const K&key)
		{
			pair<Iterator,bool> ret=Insert(make_pair(key,V()));

			return ret.first->second;//ret.first是迭代器
		}
	protected:
		size_t _HashFunc(const K&key,size_t size)
		{
			HashFunc hf;
			return hf(key)%_tables.size();
		}
		void _CheckCapacity()
		{
			if (_tables.size()==_n)
			{
				size_t newSize=GetNextPrime(_tables.size());

			/*	HashTable<K,V> newHT;
				newHT._tables.resize(newSize);*/
				vector<Node*> newTables;
				newTables.resize(newSize);
				for (size_t i=0;i<_tables.size();++i)
				{
                    Node* cur=_tables[i];
					while (cur)
					{
						//头插
						Node* next=cur->_next;
						size_t index=_HashFunc(cur->_kv.first,newSize);
						cur->_next=newTables[index];
						newTables[index]=cur;
						cur=next;
					}
					_tables[i]=NULL;
				}
               _tables.swap(newTables);
			}
		}

	protected:
		vector<Node*> _tables;
		size_t _n;

		pair<Node*,Node*> _headNode;

	};
	void TestHashTable()
	{
		HashTable<int,int> ht;
		for (size_t i=0;i<100;++i)
		{
			ht.Insert(make_pair(rand()%1000,i));
		}
		HashTable<string,string> dict;
		dict.Insert(make_pair("left","左边"));
		dict.Insert(make_pair("right","右边"));
		dict.Insert(make_pair("right1","右边"));
		dict.Insert(make_pair("right2","右边"));
		dict.Insert(make_pair("right3","右边"));
		dict.Insert(make_pair("right4","右边"));

		//dict.Insert(make_pair("right","剩余"));

		dict["left"]="剩余";

		HashTable<string,string>::Iterator it=dict.Begin();
		while (it!=dict.End())
		{
			cout<<it->first<<":"<<it->second<<" "<<endl;
			++it;
		}
		cout<<endl;
		
	}
}