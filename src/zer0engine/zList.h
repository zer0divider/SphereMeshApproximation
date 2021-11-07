#ifndef ZER0_LIST_H
#define ZER0_LIST_H

namespace zer0{
	/* generic double linked list */
	template <class T>
	class List
	{
	public:
		struct Element{
		public:
			Element(T* data, Element * next, Element * prev):
				_data(data), _next(next), _prev(prev){}

			~Element(){}

			/* get current data pointer
			 * @return current data pointer
			 */
			T* getData()const{return _data;}

			/* set new data pointer
			 * @return old data pointer
			 */
			T* setData(T* new_data){
				T* old_data = _data;
				_data = new_data;
				return old_data;
			}
			
			Element* getNext(){return _next;}
			Element* getPrev(){return _prev;}
		private:
			Element * _next;
			Element * _prev;
			T* _data;
			friend class List;
		};

		struct Iterator{
		public:
			Iterator(Element * first):_current(first){}
			T* operator *()const{assert(_current != NULL); return _current->getData();}
			T* operator ->()const{assert(_current != NULL); return _current->getData();}
			void next(){assert(_current != NULL); _current = _current->getNext();}
			void prev(){assert(_current != NULL); _current = _current->getNext();}
			operator bool()const{return _current != NULL;}
			void operator +=(int a){for(int i = 0; i< a; i++){if(_current == NULL)break; next();}}
			void operator -=(int a){for(int i = 0; i< a; i++){if(_current == NULL)break; prev();}}
			Iterator operator +(int a)const{Iterator it = *this; it += a; return it;}
			Iterator operator -(int a)const{Iterator it = *this; it -= a; return it;}
			T* setData(T* new_data){assert(_current != NULL); return _current->setData(new_data);}
		private:
			Element * _current;
			friend class List;
		};

		/* Constructor
		 * @free_objects if true, on destruction, delete is called on every object in the list
		 */
		List(bool free_objects = true): _freeObjects(free_objects), _first(NULL), _last(NULL), _size(0){}

		/* Destructor 
		 * clears all the data in the list, calls delete on each data element
		 */
		~List(){_freeObjects ? destroyAll() : removeAll(NULL);}

		/* size
		 * @return the number of elements currently in the list
		 */
		int size()const{return _size;}

		/* insert new element carrying given data pointer at the end of the list
		 * @data data to put into list
		 */
		void append(T * data){insertBack(data);}
		void insertBack(T* data);

		/* insert element before the first element
		 * @data data to put into list
		 */
		void insertFront(T * data);

		/* remove the first element from the list without destroying the data
		 * @return data carried by first element
		 */
		T* removeFirst();

		/* remove the last element from the list without destroying the data
		 * @return data carried by last element
		 */
		T* removeLast();

		/* remove all elements from the list without destroying the data
		 * the data held by the list is written into the array (if != NULL) starting with the first object at index 0
		 * @data array pointing to at least size*sizeof(T*) Bytes of allocated memory or NULL
		 */
		void removeAll(T** data = NULL);

		/* removes the first element from the list and calls delete on the object carried by that element
		 */
		void destroyFirst(){delete removeFirst();}

		/* removes the last element from the list and calls destroy on the object carried by that element
		 */
		void destroyLast(){delete removeLast();}

		/* remove all elements from the list, calls destroy on every object
		 */
		void destroyAll();

		/* remove element at given iterator without destroying the data, the iterator is altered so it references the element after the removed one
		 * @return data held by removed element
		 */
		T* remove(Iterator & it);

		/* remove element at given iterator and delete the data, the iterator is altered so it references the element after the removed one
		 */
		void destroy(Iterator & it){delete remove(it);}

		/* getting data in first element 
		 * @return data in first element or NULL if list is empty
		 */
		T* getFirst()const{return _first == NULL ? NULL : _first->_data;}

		/* getting data in last element
		 * @return data in last element or NULL if list is empty
		 */
		T* getLast()const{return _last == NULL ? NULL : _first->_data;}

		/* get all the data in the list
		 * the data held by the list is written into the array (if != NULL) starting with the first object at index 0
		 */
		void getAll(T** data)const;

		/* get an iterator referencing the first element 
		 * @return iterator referencing the first element 
		 */
		Iterator begin()const{Iterator it(_first); return it;}

		/* get an iterator referencing the last element 
		 * @return iterator referencing the last element 
		 */
		Iterator end()const{Iterator it(_last); return it;}

		/* call a function on each object in the list
		 * @procedure member function of the class this list was instanciated with (template)
		 */
		void forEach(void (T::*function)())const;

		/* call a function on each object in the list
		 * @procedure member function of the class this list was instanciated with (template)
		 * @...A variable list of arguments that are passed with the function call to each object
		 */
		template <class ...A>
		void forEach(void (T::*function)(A... args), A... args)const;

		/* insert an element carrying data after the given iterator it
		 * @data data to insert
		 * @it iterator after which to insert the element
		 */
		void insert(T * data, const Iterator & it);

	private:
		Element * _first;
		Element * _last;
		int _size;
		const bool _freeObjects;
	};

	template <class T>
	void List<T>::removeAll(T**data)
	{
		Element * e = _first;
		if(data == NULL){
			while(e != NULL){
				Element * e_next = e->_next;	
				delete e;
				e = e_next;
			}
		}
		else{
			int i = 0;
			while(e != NULL){
				Element * e_next = e->_next;	
				data[i++] = e->_data;
				delete e;
				e = e_next;
			}
		}
		_first = NULL;
		_last = NULL;
		_size = 0;
	}

	template <class T>
	void List<T>::destroyAll()
	{
		Element * e = _first;
		while(e != NULL){
			Element * e_next = e->_next;	
			delete e->_data;
			delete e;
			e = e_next;
		}
		_first = NULL;
		_last = NULL;
		_size = 0;
	}

	template <class T>
	void List<T>::insertBack(T * data)
	{
		Element * e = new Element(data, NULL, _last);
		if(_first == NULL){
			_first = e;
		}
		else{
			_last->_next = e;
		}
		_last = e;

		_size++;
	}

	template <class T>
	void List<T>::insertFront(T * data)
	{
		Element * e = new Element(data, _first, NULL);
		if(_last == NULL){
			_last = e;
		}
		else{
			_first->_prev = e;
		}
		_first = e;

		_size++;
	}

	template <class T>
	T* List<T>::removeFirst()
	{
		if(_first == NULL)
			return NULL;

		T* ret = _first->_data;
		if(_first->_next != NULL){
			_first->_next->_prev = NULL;
		}
		Element * e = _first;
		_first = _first->_next;
		delete e;

		_size--;
		if(_size <= 1){
			_last = _first;
		}
		return ret;
	}

	template <class T>
	T* List<T>::removeLast()
	{
		if(_last == NULL)
			return NULL;

		T* ret = _last->_data;
		if(_last->_prev != NULL){
			_last->_prev->_next = NULL;
		}
		Element * e = _last;
		_last = _last->_prev;
		delete e;

		_size--;
		if(_size <= 1){
			_first = _last;
		}
		return ret;
	}

	template <class T>
	T* List<T>::remove(Iterator & it)
	{
		assert(it->_current != NULL);
		Element * e = it._element;
		T * data = e->_data;
		it.next();
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
		return data;
	}

	template<class T>
	void List<T>::insert(T * data, const Iterator & it)
	{
		assert(it._current != NULL);
		Element * e = new Element(data, it._current->_next, it._current);
		if(it._current->_next != NULL){
			it._current->_next->_prev = e;
		}
		else{
			_last = e;
		}
		it._current->_next = e;
		_size++;
	}
	
	template <class T>
	template <class ...A>
	void List<T>::forEach(void (T::*function)(A... args), A... args)const
	{
		Element * e = _first;
		while(e != NULL){
			((*e->_data).*function)(args...);
			e = e->_next;
		}
	}

	template <class T>
	void List<T>::forEach(void (T::*function)())const
	{
		Element * e = _first;
		while(e != NULL){
			((*e->_data).*function)();
			e = e->_next;
		}
	}
}

#endif
