//
// Klasy, funkcje, metody do implementacji dynamicznych struktur danych
// (listy, drzewa, itp)
//


#ifndef __DATASTRUCT_H__
#define __DATASTRUCT_H__

#include <stdio.h>
#include <iostream>
#include <string>
#include "blocking.h"
#include "ccerror.h"

#ifndef _REENTRANT
#define _REENTRANT
#endif

// Klasy:
//	TNode
//	TList
//	TIterator
//	Token
//	DynamicBuffer
//	Safe_TList
//	ListRoom
//	TSortedArray
//	TSortedArrayIterator
//	TIndexItem
//	TIndex
//	TIndexIterator

// Funkcje:
//	copy_string

// copy_string

char * copy_string(string &s);

// TNode, TList, TIterator - szablony list jednokierunkowych

template <class T>
class TNode {
	T *info;
	TNode<T> *nextNode;
public:
	TNode(T*);
	~TNode();
	TNode *next() const;
	void setNext(TNode *);
	void setInfo(T *);
	T *showInfo() const;
	void destroy();
};


template <class T>
class TList {
	TNode<T> *head;
	TNode<T> *tail;  
	int length;
public:
	TList();
	virtual ~TList();
	void destroy();
	TNode<T> *showHead() const;
	T *showHeadInfo() const;
	TNode<T> *getHead();
	T *getHeadInfo();
	int rmHead();
	TNode<T> *showTail() const;
	T *showTailInfo() const;
	TNode<T> *getTail();
	T *getTailInfo();
	int rmTail();
	TNode<T> *showElt(int ) const;
	T *showEltInfo(int ) const;
	TNode<T> *getElt(int );
	T *getEltInfo(int );
	int rmElt(int );
	TNode<T> *showElt(T * ) const;
	T *showEltInfo(T * ) const;
	TNode<T> *getElt(T * );
	T *getEltInfo(T * );
	int rmElt(T * );
	int getLength() const;
	void add(T *a);// add at the end       
	void insert(T *a); // insert as the head
	bool del(T *a); // deletes a from the list
};

template <class T>
class TIterator 
{
	TNode<T> *actual;
	TList<T> *list;
public:
	TIterator(TList<T>* l);
	~TIterator();
	T* next();
	int endReached() const;
	void goToStart();
};

// Token
// Klasa ulatwiajaca parsowanie stringu na tokeny

class Token {
	char *token;
public:
	Token();
	Token(Token *);
	Token(const char *);
	~Token();
	int operator==(const Token *);
	int operator||(const Token *);
	char *string();
	char *print();
	int acceptsAll();
	int isHelpToken();
};

// DynamicBuffer

class DynamicBuffer {
	char *string;
	int length;
	int line;
public:
	DynamicBuffer();
	~DynamicBuffer();
	DynamicBuffer& operator<<(char *);
	char *toString() const;
	char *nextLine();
	void startLines();
	int numberOfLines();
};

// TSortedArray
template <class T>
class TSortedArrayIterator;

template <class T>
class TSortedArray {
	friend class TSortedArrayIterator<T>;
	T** array;

	int allocated;
	int size;
	void realloc_array(int tosize);
public:
	TSortedArray();
	~TSortedArray();	
	TSortedArray(const TSortedArray<T> &arr) { *this = arr; };

	TSortedArray<T>& operator = (const TSortedArray<T> &arr) {
		allocated = arr.allocated;
		size = arr.size;
		array = (T**) calloc(allocated, sizeof(T*));
		for (int i = 0; i < size; i++) {
			array[i] = new T;
			*(array[i]) = *arr.get(i);
		}
		return *this;
	};

	T* get(int i) const;
	int find(T t) const;
	int find_closest_on_left(T t) const;
	void insert(T t);
	void del(int i);
	void del(T t);
	void print(ostream &o) const {
		int i;
		if (size == 0) return;
 		for (i = 0; i < size; i++) { o << "{" << i << "= " << (T) *get(i) << "} "; }
	}
	friend ostream& operator << (ostream &o, const TSortedArray<T> &arr) { arr.print(o); return o; };
};

// TSortedArrayIterator

template <class T>
class TSortedArrayIterator {
	TSortedArray<T> *array;
	int actual;	
public:
	TSortedArrayIterator() : array(NULL) { };
	TSortedArrayIterator(TSortedArray<T> &_array) : array(&_array), actual(0) { };

	T *next() {
			if (!array || !array->size || IsEnd()) return NULL; 
			return array->get(actual++);
		  };
	bool IsBegin() { if (array && array->size) return (actual == 1); else return false; };
	bool IsEnd() { if (array && array->size) return (actual >= array->size); else return true; };
};

// TIndexItem

template <class T>
class TIndexItem {
public:
	string	s;
	T	item;

	TIndexItem(string _s, T _item) : s(_s), item(_item) { };
	TIndexItem() : s(""), item(T()) { };

	bool operator < (TIndexItem<T> &i) { return s < i.s; };
	bool operator <= (TIndexItem<T> &i) { return s <= i.s; };
	bool operator > (TIndexItem<T> &i) { return s > i.s; };
	bool operator >= (TIndexItem<T> &i) { return s >= i.s; };
	bool operator == (TIndexItem<T> &i) { return s == i.s; };
	bool operator != (TIndexItem<T> &i) { return s != i.s; };
	friend ostream& operator << (ostream &o, const TIndexItem<T> &item) 
		{ o << "s: " << item.s << " i: " << item.item; return o; };

};

// TIndex

template <class T>
class TIndexIterator;

template <class T>
class TIndexIterator_Names;

template <class T>
class TIndex {
	friend class TIndexIterator<T>;
	friend class TIndexIterator_Names<T>;
	TSortedArray< TIndexItem<T> > array;
public:
	void	insert(string name, T item) { 
			try {
				array.insert(TIndexItem<T>(name, item)); 
			}
			catch (exc_TSortedArray_AlreadyExists) {
				throw exc_TIndex_AlreadyExists("TIndex<T>::insert");
			}
		};
	int	find(string name) { return array.find(TIndexItem<T>(name, T() )); };
	void	del(string name) { 
			try {
				array.del(TIndexItem<T>(name, T() ));
			}
			catch (exc_TSortedArray_NotFound) {
				throw exc_TIndex_NotFound("TIndex<T>::del");
			}
		}
	T*	get(string name) { 
			int i = find(name);
  			if (i == -1) return NULL; else return &array.get(i)->item;
		};
	friend ostream& operator << (ostream &o, const TIndex<T> &index) { o << index.array; return o; };
};	

// TIndexIterator 
 
template <class T>
class TIndexIterator {
	TSortedArrayIterator< TIndexItem<T> > *iterator;
public:
	TIndexIterator() : iterator(NULL) { };
	~TIndexIterator() { if (iterator) delete iterator; };
	TIndexIterator(const TIndexIterator<T> &ii) { *this = ii; }; 
	TIndexIterator<T>& operator = (const TIndexIterator<T> &ii) { 
		iterator = new TSortedArrayIterator< TIndexItem<T> >();
		*iterator = *ii.iterator;
		return *this;
	};
	TIndexIterator(TIndex<T> &index)  { 
		iterator = new TSortedArrayIterator< TIndexItem<T> > (index.array);
	};

	T *next() {
			if (!iterator) return NULL;
			TIndexItem<T> *item = iterator->next();
			if (item) return &item->item; else return NULL;
		  };
	bool IsBegin() { if (!iterator) return 1; return iterator->IsBegin(); };
	bool IsEnd() { if (!iterator) return 1; return iterator->IsEnd(); };

};

// TIndexIterator_Names 
 
template <class T>
class TIndexIterator_Names {
	TSortedArrayIterator< TIndexItem<T> > *iterator;
public:
	TIndexIterator_Names() : iterator(NULL) { };
	~TIndexIterator_Names() { if (iterator) delete iterator; };
	TIndexIterator_Names(const TIndexIterator_Names<T> &ii) { *this = ii; }; 
	TIndexIterator_Names<T>& operator = (const TIndexIterator_Names<T> &ii) { 
		iterator = new TSortedArrayIterator< TIndexItem<T> >();
		*iterator = *ii.iterator;
		return *this;
	};
	TIndexIterator_Names(TIndex<T> &index)  { 
		iterator = new TSortedArrayIterator< TIndexItem<T> > (index.array);
	};

	string next() {
			if (!iterator) return "";
			TIndexItem<T> *item = iterator->next();
			if (item) return item->s; else return "";
		  };
	bool IsBegin() { if (!iterator) return 1; return iterator->IsBegin(); };
	bool IsEnd() { if (!iterator) return 1; return iterator->IsEnd(); };

};


// Safe_TList

template <class T>
class Safe_TList : public TList<T>, public Room {
public:
	Safe_TList();
	virtual ~Safe_TList();
};

template <class T>
Safe_TList<T>::Safe_TList() : TList<T>(), Room() {
};

template <class T>
Safe_TList<T>::~Safe_TList() {
};

//////////////////////////////////////
//	Implementacje
//////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

//
//	TNode
//

template<class T> TNode<T>::TNode(T *a) : info(a), nextNode(NULL) {
};

template<class T>
TNode<T>::~TNode() {
//	info = NULL;
//	nextNode = NULL;
};

template<class T>
TNode<T> *TNode<T>::next() const {
	return nextNode;
};

template<class T>
void TNode<T>::destroy() {

	delete info;
//	delete this;

};

template<class T>
T *TNode<T>::showInfo() const {
	return info;
};

template<class T>
void TNode<T>::setInfo(T *i) {
	info = i;
};

template<class T>
void TNode<T>::setNext(TNode *n) {
	nextNode = n;
};


//
//	TList
//

template<class T>
TList<T>::TList() {
	head = tail = NULL;
	length = 0;
};

template<class T>
TList<T>::~TList() {
	//skasuj wszystko oprocz obiektow przechowywanych w liscie
	TNode<T> *pom;
	while(head != NULL) {
		pom = head;
		head = head->next();
		delete pom;
	}
};

template<class T>
void TList<T>::destroy() {
	//skasuj wszystko razem z obiektami ktore przechowuje
	TNode<T> *pom;
//	while(rmHead());


	while(head != NULL) {
		pom = head;
		head = head->next();
		pom->destroy();
	}


// o k*... czyj to patent ?
//	delete this;
// c*** Ci w dupe, sprawdzilem, nie mooj (V.)
};

template<class T>
TNode<T> *TList<T>::showHead() const {
	return head;
};

template<class T>
T *TList<T>::showHeadInfo() const {
	if( head != NULL )
		return head->showInfo();
	else
		return NULL;
};

template<class T>
TNode<T> *TList<T>::getHead() {
	if( head != NULL ) {
		TNode<T> *pom = head;
		head = head->next();
		pom->setNext(NULL);
		length--;
		return pom;
	} else
		return NULL;
};

template<class T>
T *TList<T>::getHeadInfo() {
	if( head != NULL ) {
		TNode<T> *pom = head;
		head = head->next();
		T *ret = pom->showInfo();
		delete pom;
		length--;
		return ret;
	} else
		return NULL;
};

template<class T>
int TList<T>::rmHead() {
	T *t = getHeadInfo();
	if( t == NULL )
		return 0;
	else {
		delete t;
		return 1;
	}
};

template<class T>
TNode<T> *TList<T>::showTail() const {
	return tail;
};

template<class T>
T *TList<T>::showTailInfo() const {
	if( tail != NULL )
		return tail->showInfo();
	else
		return NULL;
};

template<class T>
TNode<T> *TList<T>::getTail() {
	if( tail != NULL ) {
		TNode<T> *pom1 = head;
		length--;
		if( tail == head ) {
			tail = head = NULL;
			return pom1;
		} else {
			TNode<T> *pom2;	
			while (pom1 != tail) {
				pom2 = pom1;
				pom1 = pom1->next();
			}
			pom2->setNext(NULL);
			tail = pom2;
			return pom1;
		} 
	} else
		return NULL;
};

template<class T>
T *TList<T>::getTailInfo() {
	if( tail != NULL ) {
		TNode<T> *pom1 = head;
		T *t_pom;
		length--;
		if( tail == head ) {
			tail = head = NULL;
			t_pom = pom1->showInfo();
			delete pom1;
			return t_pom;
		} else {
			TNode<T> *pom2;	
			while (pom1 != tail) {
				pom2 = pom1;
				pom1 = pom1->next();
			}
			pom2->setNext(NULL);
			tail = pom2;
			t_pom = pom1->showInfo();
			delete pom1;
			return t_pom;
		}
	} else
		return NULL;
};

template<class T>
int TList<T>::rmTail() {
	T *t = getTailInfo();
	if( t == NULL )
		return 0;
	else {
		delete t;
		return 1;
	}
};


template<class T>
TNode<T> *TList<T>::showElt(int n) const {
	if( n < 0 || n >= length)
		return NULL;
	else {
		TNode<T> *pom;
		pom = head;
		for(int i = 0; i < n; i++)
			pom = pom->next();
		return pom;
	}
};

template<class T>
T *TList<T>::showEltInfo(int n) const {
	if( n < 0 || n >= length)
		return NULL;
	else {
		TNode<T> *pom;
		pom = head;
		for(int i = 0; i < n; i++)
			pom = pom->next();
		return pom->showInfo();
	}	
};

template<class T>
TNode<T> *TList<T>::getElt(int n) {
	if( n < 0 || n >= length)
		return NULL;
	else {
		TNode<T> *pom1 = head;
		length--;
		if( tail == head ) {
			tail = head = NULL;
			return pom1;
		} else {
			TNode<T> *pom2;
			for(int i = 0; i < n; i++) {
				pom2 = pom1;
				pom1 = pom1->next();
			}
			if( pom1 == head ) {
				head = head->next();
				pom1->setNext(NULL);
				return pom1;
			}
			if( pom1 == tail ) {
				tail = pom2;
				tail->setNext(NULL);
				return pom1;
			}
			pom2->setNext(pom1->next());
			pom1->setNext(NULL);
			return pom1;
		}	
	}
};

template<class T>
T *TList<T>::getEltInfo(int n) {
	if( n < 0 || n >= length)
		return NULL;
	else {
		TNode<T> *pom1 = head;
		T *t_pom;
		length--;
		if( tail == head ) {
			tail = head = NULL;
			t_pom = pom1->showInfo();
			delete pom1;
			return t_pom;
		} else {
			TNode<T> *pom2;
			for(int i = 0; i < n; i++) {
				pom2 = pom1;
				pom1 = pom1->next();
			}
			if( pom1 == head ) {
				head = head->next();
				pom1->setNext(NULL);
				t_pom = pom1->showInfo();
				delete pom1;
				return t_pom;
			}
			if( pom1 == tail ) {
				tail = pom2;
				tail->setNext(NULL);
				t_pom = pom1->showInfo();
				delete pom1;
				return t_pom;
			}
			pom2->setNext(pom1->next());
			pom1->setNext(NULL);
			t_pom = pom1->showInfo();
			delete pom1;
			return t_pom;
		}	
	}
};

template<class T>
int TList<T>::rmElt(int n) {
	T *t = getEltInfo(n);
	if( t == NULL )
		return 0;
	else {
		delete t;
		return 1;
	}
};


template<class T>
TNode<T> *TList<T>::showElt(T *t) const {
	TNode<T> *pom;
	pom = head;
	while(pom != NULL && !( *(pom->showInfo) == *t))
		pom = pom->next();
	return pom;
};

template<class T>
T *TList<T>::showEltInfo(T *t) const {
	TNode<T> *pom;
	pom = head;
	while(pom != NULL && !( *(pom->showInfo) == *t))
		pom = pom->next();
	if( pom != NULL )
		return pom->showInfo();
	else
		return NULL;
};

template<class T>
TNode<T> *TList<T>::getElt(T *t) {
	TNode<T> *pom1, *pom2;
	pom1 = head;
	while(pom1 != NULL && pom1->showInfo() != t) {
		pom2 = pom1;
		pom1 = pom1->next();
	}
	if( pom1 != NULL ) {
		length--;
		if( pom1 == head && pom1 == tail ) {
			head = tail = NULL;
			return pom1;
		}
		if( pom1 == head ) {
			head = pom1->next();
			pom1->setNext(NULL);
			return pom1;
		}
		if( pom1 == tail ) {
			pom2->setNext(NULL);
			tail = pom2;
			return pom1;
		}
		pom2->setNext(pom1->next());
		pom1->setNext(NULL);
		return pom1;
	} else
		return NULL;
};

template<class T>
T *TList<T>::getEltInfo(T *t) {
	TNode<T> *pom1, *pom2;
	pom1 = head;
	while(pom1 != NULL && pom1->showInfo() != t) {
		pom2 = pom1;
		pom1 = pom1->next();
	}
	if( pom1 != NULL ) {
		T *t_pom;
		length--;
		if( pom1 == head && pom1 == tail ) {

			head = tail = NULL;
			t_pom = pom1->showInfo();
			delete pom1;
			return t_pom;
		}
		if( pom1 == head ) {
			head = pom1->next();
			pom1->setNext(NULL);
			t_pom = pom1->showInfo();
			delete pom1;
			return t_pom;
		}
		if( pom1 == tail ) {
			pom2->setNext(NULL);
			tail = pom2;
			t_pom = pom1->showInfo();
			delete pom1;
			return t_pom;
		}
		pom2->setNext(pom1->next());
		pom1->setNext(NULL);
		t_pom = pom1->showInfo();
		delete pom1;
		return t_pom;
	} else
		return NULL;
};

template<class T>
int TList<T>::rmElt(T *t) {
	T *z = getEltInfo(t);
	
	if( z == NULL )
		return 0;
	else {
		delete z;
		return 1;
	}
};




template<class T>
int TList<T>::getLength() const {
	return length;
};

// puts the T-class element to the end of the list
template <class T>
void TList<T>::add(T *a) {       
	TNode<T>* dow = new TNode<T>(a);
	if (tail == NULL)
		head = tail = dow;
	else {
		tail->setNext(dow); 
		tail = dow;
	} 
	length++;
};


// put the element to the head of the list
template <class T>
void TList<T>::insert(T *a) {
	TNode<T>* dow = new TNode<T>(a);
	if (tail == NULL)
		head = tail = dow;
	else {
		dow->setNext(head);
		head = dow;                     
	}
	length++;
}           
// a bit complicated, but it works!
template <class T>
bool TList<T>::del(T * element){

int l = 0;
TNode<T>* n, *tmpN;
TNode<T>* prev = head;

  for(n= head;n != NULL;n = n->next()){
        if( n->showInfo() == element ){
                tmpN = (TNode<T>*) n->next();
                n->setNext(NULL);
                delete n;
                if( n == head ) head = tmpN;
                        else
                prev->setNext(tmpN);
                                                                                                                  
                if( tmpN == NULL ) // ostatni element!
                        tail = prev;
                return true;
                }
        l++;
        prev = n;
        }
                        
return false;
}
                        
//
//	TIterator
//

template<class T>
TIterator<T>::TIterator(TList<T> *l) {
	list = l;
	actual = l->showHead();
};

template<class T>
TIterator<T>::~TIterator() {

};

template <class T>
T* TIterator<T>::next() {
	if (actual != NULL) {
		T* pom = actual->showInfo();
		actual = actual->next();
		return pom;
	} else
		return NULL;
};

template <class T>
int TIterator<T>::endReached() const {
	if (actual != NULL) 
		return 0; 
	else
		return 1;
};

template <class T>
void TIterator<T>::goToStart() {
	actual = list->showHead();
};


////////////////////////////////////////////////////////////////////////////
/// ListRoom

class Room;

template <class T>
class TListRoom : public TList<T>, public Room {
public:
	TListRoom();
	virtual ~TListRoom();
};

template <class T>
TListRoom<T>::TListRoom() : TList<T>(), Room() {
};

template <class T>
TListRoom<T>::~TListRoom() {
};



//////////////////////////////////////////////////////////////////////////////
// TSortedArray & TSortedArrayIterator


template <class T>
void TSortedArray<T>::realloc_array(int tosize)
{

	int tmp = 1;

	while (tmp < tosize) {
		tmp *= 2;
	};

	if (tmp == allocated) return;

	allocated = tmp;
	array = (T**) realloc(array, allocated * sizeof(T*));
	if (!array) throw exc_CannotAlloc("TSortedArray<T>::realloc_array");
}


template <class T>
TSortedArray<T>::TSortedArray()
{
	allocated = 1;
	size = 0;
	array = (T**) calloc(allocated, sizeof(T*));
}

template <class T>
TSortedArray<T>::~TSortedArray()
{
	if (!array) return;
	for (int i = 0; i < size; i++) {
		delete array[i];
	}
	free(array);
}

template <class T>
void TSortedArray<T>::insert(T t)
{
	int i = find_closest_on_left(t);
	if (i > -1 && t == *get(i))
		throw exc_TSortedArray_AlreadyExists("TSortedArray<T>::insert");

	realloc_array(size + 1);
	for (int j = size; j > i+1; j --) {
		array[j] = array[j - 1];
	};
	array[i+1] = new T;
	*(array[i+1]) = t;
	size++;	
}

template <class T>
T* TSortedArray<T>::get(int i) const
{
	if (i < 0 || i >= size) return NULL;
	return array[i];
}

template <class T>
int TSortedArray<T>::find_closest_on_left(T t) const
{
	if (size == 0) return -1;
	int l, r, m;

	l = 0;
	r = size - 1;

	if (t < *get(l)) return -1; // mniejszy od lewego przedzialu
	if (t > *get(r)) return r; // wiekszy od prawego

	do {
		m = (l + r) / 2;
		if (l == m) break;

		if (t > *get(m)) l = m; else r = m;

	} while (l != r);
	
	if (*get(r) <= t) l = r;

	return l;
}

template <class T>
int TSortedArray<T>::find(T t) const
{
	int i = find_closest_on_left(t);
	if (i > -1 && *(array[i]) == t) return i; else return -1;
}

template <class T>
void TSortedArray<T>::del(int i) 
{
	if (!array || !size) throw exc_TSortedArray_Empty("TSortedArray<T>::del");
	if (i < 0 && i >= size) throw exc_TSortedArray_OutOfBounds("TSortedArray<T>::del");

	delete array[i];
	size--;

	if (i == size) return;

	for (int j = i; j < size; j++) {
		array[j] = array[j+1];
	}
	realloc_array(size);
}

template <class T>
void TSortedArray<T>::del(T t) 
{
	int i = find(t);
	if (i >= 0) del(i);
	else throw exc_TSortedArray_NotFound("TSortedArray<T>::del");
}



#endif

