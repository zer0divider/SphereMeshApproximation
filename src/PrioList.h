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
		// we start from the end, because for the specific case of SQEM metric,
		// when an edge is collapsed the new edges will likely have a very high cost,
		// therefore shortening the search time for the correct position in the list
		Compare cmp;
		/*
		if(this->empty() || !cmp(element, this->back())){
			return this->std::list<T>::insert(this->end(), element);
		}
		typename std::list<T>::iterator it = this->end();
		it--;

		
		while(it != this->end()){
			// compare both elements
			if(!cmp(element, *it)){
				// insert after current
				it++;
				return this->std::list<T>::insert(it, element);
			}
			it--;
		}

		// smallest element, put at front
		return this->std::list<T>::insert(this->begin(), element);
		*/
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
