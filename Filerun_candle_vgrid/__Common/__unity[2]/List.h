#pragma once

template<class T> class List;


template <class T>
class cnode
{
public:
	 T data;
     cnode<T>* prev;
	 cnode<T>* next;

	 friend class List<T>;
};


template <class T>
class List
{
protected:
	cnode<T>* head;
	cnode<T>* tail;
	int   m_nCount;

public:
	List(void);
	~List(void);

	bool InsertBefore(T& data, cnode<T>* b);
	bool InsertAfter(T& data, cnode<T>* b);
	bool AddTail(T& data);
	int GetCount(void) const;

	bool RemoveAt (cnode<T>* t, bool bData = false); 
	bool RemoveAt (int index, bool bData = false); 
	bool RemoveAll(bool bData = false);

	bool GetNext(cnode<T>** pBase, T& data)const; // pBase will be ++, return 1 : ok, o : tail;
    bool GetPrev(cnode<T>** pBase, T& data)const; // pBase will be --, return 1 : ok, o : head;

    cnode<T>* Find(T& data) const;  // Find with value data

	cnode<T>* GetHead(void) const;
	cnode<T>* GetTail(void) const;
	cnode<T>* GetAt(int index) const;   
	T& GetAt2 (int index) const;  // return node->data;

	const List& operator=( const List & rhs );
};


template <class T>
List<T>::List(void)
: m_nCount(0)
{
	head = new cnode<T>();
	tail = new cnode<T>();

	head->next = tail;
	head->prev = head;

	tail->next = tail;
	tail->prev = head;
}

template <class T>
List<T>::~List(void)
{
	RemoveAll();
	delete (cnode<T>*)head;
	delete (cnode<T>*)tail;
}

template <class T>
bool List<T>::InsertBefore(T& data, cnode<T>* b)
{
	if (b == head) return false;

	cnode<T>*  p = new cnode<T>();
    p->data = data;

	cnode<T>*  a = b->prev;

	a->next = p;
	b->prev = p;

	p->prev = a;
	p->next = b;
    
	m_nCount++;
	return true;
}

template <class T>
bool List<T>::InsertAfter(T& data, cnode<T>* b)
{
    if (b == tail) return false;

	cnode<T>*  p = new cnode<T>();
    p->data = data;

	cnode<T>* c = b->next;

	p->next = c;
	p->prev = b;

	b->next = p;
	c->prev = p;

	m_nCount++;

	return true;
}

template <class T>
bool List<T>::AddTail(T& data)
{
	cnode<T>* b = tail;
    return InsertBefore(data, b);
}

template <class T>
int List<T>::GetCount(void) const
{
	return m_nCount;
}

template <class T>
bool List<T>::RemoveAt(cnode<T>* b, bool bData/* = false*/)
{
	if(b == head || b == tail) return false;

	cnode<T>* a = b->prev;
	cnode<T>* c = b->next;

	a->next = c;
	c->prev = a;

	if(b)
	{ 
		if(bData) { delete b->data; b->data = 0; }
		delete b; b = 0;
	};

	m_nCount --;
	return true;
}

template <class T>
bool List<T>::RemoveAt (int index, bool bData/* = false*/)
{
   if(index >= m_nCount || index < 0) return false;
   
   cnode<T>* s = head->next;

   for(int nIndex = 0; s != tail && nIndex < index; nIndex++) s = s->next;

   return RemoveAt(s, bData);
}

template <class T>
bool List<T>::RemoveAll(bool bData/* = false*/)
{
	cnode<T>* p = head->next;
	cnode<T>* s;

	while(p != tail)
	{
	   s = p;
	   p = s->next;

		if(s) 
		{ 
			if(bData) { delete s->data; s->data = 0; }
			delete s; s = 0;
		};
	}

	m_nCount = 0;
	head->next = tail;
	tail->prev = head;
	return true;
}

template <class T>
bool List<T>::GetNext(cnode<T>** b, T& data) const 
{
	*b = (*b)->next;
	data = (*b)->data;

	if(*b == tail) return false;
	else return true;
}

template <class T>
bool List<T>::GetPrev(cnode<T>** b, T& data) const // pBase will be --, return node->data;
{
	*b = (*b)->prev;
	data = (*b)->data;

	if(*b == head) return false;
	return true;
}

template <class T>
cnode<T>* List<T>::Find(T& data) const
{
   cnode<T>* s = head->next;

   while(s != tail)
   {
	   if(s->data == data) return s;
	   s = s->next;
   }
   return (cnode<T>*)(0); // return 0
}

template <class T>
cnode<T>* List<T>::GetHead(void) const
{
    return head;
}

template <class T>
cnode<T>* List<T>::GetTail(void) const
{
   return tail;
}

template <class T>
cnode<T>* List<T>::GetAt(int index) const
{
   if(index >= m_nCount || index < 0) return (cnode<T>*)0;
   
   int itr = index;
   cnode<T>* s = head->next;

   for(; s!=tail && itr > 0; itr--) s = s->next;

   return s;
}

template <class T>
T& List<T>::GetAt2 (int index) const
{
	const static T t; 

	cnode<T>* s;
	s = GetAt(index);

	if(s != 0) return s->data;
	else return s->data;
}

template <class T>
const List<T>& List<T>::operator=( const List<T> & rhs )
{
    if( this != &rhs )
    {
        RemoveAll();
	    cnode<T>* s = rhs.GetHead();
		T data;

		while(rhs.GetNext(&s,data)) AddTail(data);
    }
    return *this;
}
