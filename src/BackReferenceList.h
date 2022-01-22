/* Author: Cornelius Marx
 */
#ifndef BACK_REFERENCE_LIST_H
#define BACK_REFERENCE_LIST_H


/**
 * A doubly linked list where each element is the user data itself (through inheritance).
 * This allows for removal of the element from the list, when only the data is known.
 * The list owns the items, which means that 
 */
template <typename T>
class BackReferenceList
{
public:

	class Item
	{
	public:
		virtual ~Item(){}
		T* get(){return (T)this;}	
		const T* get()const{return (T*)this;}

		T* getNext(){return (T*)_next;}
		const T* getNext()const{return (T*)_next;}

		T* getPrev(){return (T*)_prev;}
		const T* getPrev()const{return (T*)_prev;}

		static T* __ITEM_INHERITANCE_GUARD__;
	friend class BackReferenceList;
	private:
		Item * _next; // pointer to next element in list
		Item * _prev; // pointer to last element in list

	};

	/**
	 * constructor, intialize empty list
	 */
	BackReferenceList(): _first(nullptr), _last(nullptr), _size(0){
		/* this will generate a compile error if T does not inherit from Item<T> */
		(void)(T::__ITEM_INHERITANCE_GUARD__ == (T*)nullptr);
	}

	/**
	 * destructor, clear list
	 */
	~BackReferenceList(){clear();}	
	
	/**
	 * append an item at the end of the list
	 */
	T* add(T * item){

		Item * i = (Item*)item;
		if(_first == NULL){
			_first = i;
		}
		else{
			_last->_next = i;
		}
		i->_prev = _last;
		i->_next = nullptr;
		_last = i;

		_size++;
		return item;
	}

	/**
	 * append a new item created with the default constructor T()
	 */
	T* add(){
		return add(new T());
	}

	/**
	 * remove an item from the list
	 */
	void remove(T * item){
		Item * e = (Item*)item;
		if(e->_prev != NULL){
			e->_prev->_next = e->_next;
		}
		else{
			_first = e->_next;
		}
		if(e->_next != NULL){
			e->_next->_prev = e->_prev;
		}
		else{
			_last = e->_prev;
		}
		delete e;

		_size--;
	}

	/**
	 * remove an item from the list but do not delete
	 */
	void extract(T * item){
		Item * e = (Item*)item;
		if(e->_prev != NULL){
			e->_prev->_next = e->_next;
		}
		else{
			_first = e->_next;
		}
		if(e->_next != NULL){
			e->_next->_prev = e->_prev;
		}
		else{
			_last = e->_prev;
		}

		_size--;
	}

	/**
	 * delete all items
	 */
	void clear(){
		Item * i = _first;
		while(i != nullptr){
			Item * n = i->_next;
			delete i;	
			i = n;
		}
		_first = nullptr;
		_last = nullptr;
		_size = 0;
	}

	T* getFirst(){
		return (T*)_first;
	}

	const T* getFirst()const{
		return (T*)_first;
	}

	T* getLast(){
		return (T*)_last;
	}

	const T* getLast()const{
		return (T*)_last;
	}

	size_t getSize()const{
		return _size;
	}

private:
	Item * _first; // first item in list
	Item * _last;  // last item in list
	size_t _size;  // num items
};


#endif
