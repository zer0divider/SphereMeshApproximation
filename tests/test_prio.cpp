#include "DynamicMesh.h"

int main()
{
	std::priority_queue<DynamicMesh::Edge*, std::vector<DynamicMesh::Edge*>, DynamicMesh::CollapseCostLess> prio;
	DynamicMesh::Edge e1(nullptr, nullptr), e2(nullptr, nullptr), e3(nullptr, nullptr);
	e1.collapse_cost = -1;
	e2.collapse_cost = 0.5;
	e3.collapse_cost = 10;

	printf("Pushing %f, %f, %f, into prio queue.\n", e1.collapse_cost, e2.collapse_cost, e3.collapse_cost);
	prio.push(&e3);
	prio.push(&e1);
	prio.push(&e2);

	printf("Pop from prio queue.\n");
	assert(prio.top() == &e3);
	prio.pop();
	assert(prio.top() == &e2);
	prio.pop();
	assert(prio.top() == &e1);
	prio.pop();

	assert(prio.size() == 0);

	printf("All valid.\n");

	return 0;
}
