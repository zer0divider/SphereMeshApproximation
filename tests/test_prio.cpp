#include "DynamicMesh.h"
#include <queue>

int main()
{
	DynamicMesh::Edge e1(nullptr, nullptr), e2(nullptr, nullptr), e3(nullptr, nullptr), e4(nullptr, nullptr);
	e1.collapse_cost = -1;
	e2.collapse_cost = 0.5;
	e3.collapse_cost = 10;
	e4.collapse_cost = -2;

	// testing own prio queue
	printf("### Testing own PrioList ###\n");
	DynamicMesh::CollapseListType my_prio;
	my_prio.push(&e3);
	my_prio.push(&e1);
	my_prio.push(&e2);
	my_prio.push(&e4);
	printf("Pop from prio queue.\n");
	assert(my_prio.top() == &e4);
	my_prio.pop();
	assert(my_prio.top() == &e1);
	my_prio.pop();
	assert(my_prio.top() == &e2);
	my_prio.pop();
	assert(my_prio.top() == &e3);
	my_prio.pop();

	printf("All valid.\n\n");

	return 0;
}
