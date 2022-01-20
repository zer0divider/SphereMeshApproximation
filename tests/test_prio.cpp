#include "DynamicMesh.h"
#include "PrioList.h"
#include <queue>

int main()
{
	DynamicMesh::Edge e1(nullptr, nullptr), e2(nullptr, nullptr), e3(nullptr, nullptr), e4(nullptr, nullptr);
	e1.collapse_cost = -1;
	e2.collapse_cost = 0.5;
	e3.collapse_cost = 10;
	e4.collapse_cost = -2;
	/*
	printf("### Testing std::priority_queue ###\n");
	std::priority_queue<DynamicMesh::Edge*, std::vector<DynamicMesh::Edge*>, DynamicMesh::CollapseCostCompare> prio;

	printf("Pushing %f, %f, %f, into prio queue.\n", e1.collapse_cost, e2.collapse_cost, e3.collapse_cost);
	prio.push(&e3);
	prio.push(&e1);
	prio.push(&e2);

	printf("Pop from prio queue.\n");
	assert(prio.top() == &e1);
	prio.pop();
	assert(prio.top() == &e2);
	prio.pop();
	assert(prio.top() == &e3);
	prio.pop();

	assert(prio.size() == 0);

	printf("All valid.\n\n");
	*/

	// testing own prio queue
	printf("### Testing own PrioList ###\n");
	PrioList<DynamicMesh::Edge*, DynamicMesh::CollapseCostCompare> my_prio;
	my_prio.insert(&e3);
	my_prio.insert(&e1);
	my_prio.insert(&e2);
	my_prio.insert(&e4);
	printf("Pop from prio queue.\n");
	assert(my_prio.front() == &e4);
	my_prio.pop_front();
	assert(my_prio.front() == &e1);
	my_prio.pop_front();
	assert(my_prio.front() == &e2);
	my_prio.pop_front();
	assert(my_prio.front() == &e3);
	my_prio.pop_front();

	DynamicMesh::Edge e_big(nullptr, nullptr);
	e_big.collapse_cost = 12;
	my_prio.insert(&e_big);
	assert(my_prio.back() == &e_big);

	printf("All valid.\n\n");

	printf("### Testing own PrioList with sort ###\n");
	my_prio.clear();
	my_prio.insert_end(&e2);
	my_prio.insert_end(&e1);
	my_prio.insert_end(&e3);

	printf("Sorting.\n");
	my_prio.sort();
	printf("Checking sorting.\n");
	assert(my_prio.front() == &e1);
	my_prio.pop_front();
	assert(my_prio.front() == &e2);
	my_prio.pop_front();
	assert(my_prio.front() == &e3);
	my_prio.pop_front();

	printf("All valid.\n\n");

	return 0;
}
