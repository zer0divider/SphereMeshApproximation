#ifndef PRIO_LIST_H
#define PRIO_LIST_H

#include <queue>

template <class T, class Compare>
class PrioList : public std::priority_queue<T, std::vector<T>, Compare>
{
public:
	PrioList(){}
	~PrioList(){}

};

#endif
