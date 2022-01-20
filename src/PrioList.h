#ifndef PRIO_LIST_H
#define PRIO_LIST_H

#include <list>

template <class T, class Compare>
class PrioList : public std::list<T>
{
public:
	PrioList(){}
	~PrioList(){}

	// insert according to priority
	typename std::list<T>::iterator insert(const T & element){
		Compare cmp;
		typename std::list<T>::iterator it = this->begin();
		while(it != this->end()){
			// compare both elements
			if(cmp(element, *it)){
				// insert here, done
				return this->std::list<T>::insert(it, element);
			}
			it++;
		}

		return this->std::list<T>::insert(it, element);
	}
	
	/**
	 * insert at the end of the list, this breaks the priority, use sort() to fix
	 * 
	 */
	typename std::list<T>::iterator insert_end(const T & element){
		return this->std::list<T>::insert(this->end(), element);
	}

	/**
	 * sort the list so that priority is in order
	 */
	void sort(){
		Compare cmp;
		this->std::list<T>::sort(cmp);
	}

};

#endif
