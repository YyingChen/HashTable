#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>
#include<string>
using namespace std;

static size_t GetNextPrime(size_t value)//��ȡһ����value�������
//ȫ�ֺ�������Ҫʹ�þ�̬�ģ����ڲ�ͬ��cpp���ͻ
{
	//ʹ���������������ϣ������������͹�ϣ��ͻ
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
struct __HashFunc<string>//�ػ�,���ȵ����ػ��汾
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
		return BKDRHash(key.c_str());//�ַ���ת����

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
	State _state;//���һ��λ�õ�״̬

	HashNode()
		:_state(EMPTY)
	{}
};


//struct __HashFuncString//�º�������ַ�����ϣ�㷨
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
        size_t index=_HashFunc(kv.first);//���k��string/struct�Ͳ�����%���º������
		                                 //������ȡģ��ת��Ϊȡģ
		//����ж�λ���Ƿ���������,ɾ��֮��������ô֪����λ�õ����ݱ�ɾ����---״̬��־λ
		while (_tables[index]._state==EXIST)
		{
			if (_tables[index]._kv.first==kv.first)
			{
				return false;
			}
			//����̽��
			index+=i*i;
			index=index%_tables.size();
			++i;
			//++index;//����̽��
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
		while (_tables[index]._state!=EMPTY)//����˸������ӣ��������˲����ݣ����Բ�����ѭ��
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
		return _tables.size();//��ʵ�õĿռ�
	}
protected:
	
	void _CheckCapacity()
	{
		if(_tables.empty())
		{
			_tables.resize(GetNextPrime(0));//reserveֻ���ռ䲻��size
			return;
		}
		if (_n*10/_tables.size()==7)//���غ����Ӽ��㵽0.7�Ϳ�ʼ����
		{
			size_t newSize=_tables.size()*2;
			HashTable<K,V,HashFunc> newHT;//�½�һ����ϣ��
			newHT._tables.resize(newSize);
            for (size_t i=0;i<_tables.size();++i)
            {
				if (_tables[i]._state==EXIST)
				{
					newHT.Insert(_tables[i]._kv);//�����ϣ�����ݺ����ݵ�λ������
				}
            }
			_tables.swap(newHT._tables);
		}
	}
protected:
	//pair<K,V>* _tables;//k�����ݣ�v�ǳ��ֵĴ���
	//size_t _size;//�п��ܻ�����
	//size_t _capacity;
	vector<HashNode<K, V>> _tables;//����Ҫ��ʼ�����������͵�ȱʡ���캯����
	                          //�ڳ�ʼ���б�ĳ�ʼ�����Ⱥ������ڵĳ�ʼ������Ч
	size_t _n;//��vector��size�������֣���ʾvector�����������˶��ٸ���

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
	dict.Insert(make_pair(string("left"),string("���")));
	dict.Insert(make_pair(string("1111"),string("�ϱ�")));
	dict.Insert(make_pair(string("1122"),string("�ұ�")));
	
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
		   //1�������ǰͰû���꣬������
          if (_node->_next!=NULL)
          {
			  _node=_node->_next;
          }
		  //2����ǰͰ�����ˣ�����һ�β�Ϊ�յ�ʹ
		  else
		  {
			 
             index = hf(_node->_kv.first)%_ht->_tables.size();
			  _node=NULL;
              for (i=index+1;i<_ht->_tables.size();++i)
              {
                  if (_ht->_tables[i])
                  {
					  _node=_ht->_tables[i];//�ҵ���һ����Ϊ�յ�����_node�ı䣬���û�ҵ�_node=NULL
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
			return Iterator(NULL,this);//thisָ��hashtable 
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
			//ͷ��
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
                     if (prev==NULL)//˵��cur�ǵ�һ���ڵ�
                     {
						 //ͷɾ
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
		void Resize(size_t size)//�������ǰ֪�����ж������ݣ��Ȱѱ�resize����ô�� ��������
			                    //��߲������ݵ�Ч��
		{
			_tables.resize(GetNextPrime(size));
		}

		V& operator[](const K&key)
		{
			pair<Iterator,bool> ret=Insert(make_pair(key,V()));

			return ret.first->second;//ret.first�ǵ�����
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
						//ͷ��
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
		dict.Insert(make_pair("left","���"));
		dict.Insert(make_pair("right","�ұ�"));
		dict.Insert(make_pair("right1","�ұ�"));
		dict.Insert(make_pair("right2","�ұ�"));
		dict.Insert(make_pair("right3","�ұ�"));
		dict.Insert(make_pair("right4","�ұ�"));

		//dict.Insert(make_pair("right","ʣ��"));

		dict["left"]="ʣ��";

		HashTable<string,string>::Iterator it=dict.Begin();
		while (it!=dict.End())
		{
			cout<<it->first<<":"<<it->second<<" "<<endl;
			++it;
		}
		cout<<endl;
		
	}
}