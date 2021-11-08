#include "DynamicMesh.h"

using namespace zer0;

std::string DynamicMesh::Vertex::toString()const
{

	return "(" +
		std::to_string(position.x) + ", " +
		std::to_string(position.y) + ", " +
		std::to_string(position.z) + ")";
}

void DynamicMesh::Face::calculateNormal()
{
	assert(v[0] != nullptr);
	assert(v[1] != nullptr);
	assert(v[2] != nullptr);
	normal = Vector3D::cross(
				(v[1]->position - v[0]->position),
				(v[2]->position - v[0]->position));
	normal.normalize();
}

DynamicMesh::Vertex* DynamicMesh::Face::getOtherVertex(Vertex * not_this_one, Vertex * and_not_this_one)
{
	if(v[0] == not_this_one){
		if(v[1] == and_not_this_one){
			return v[2];
		}
		else{
			return v[1];
		}
	}
	else if(v[1] == not_this_one){
		if(v[0] == and_not_this_one){
			return v[2];
		}
		else{
			return v[0];
		}
	}
	else if(v[2] == not_this_one){
		if(v[1] == and_not_this_one){
			return v[0];
		}
		else{
			return v[1];
		}
	}
	return nullptr;
}

DynamicMesh::DynamicMesh()
{
}

DynamicMesh::~DynamicMesh()
{
}

void DynamicMesh::clear()
{
	_vertexList.clear();
	_edgeList.clear();
	_faceList.clear();
}

void DynamicMesh::debug_print()
{
	// debug print
	int count = 0;
	INFO("### VERTICIES ###");
	for(const Vertex * v = _vertexList.getFirst(); v != nullptr; v = v->getNext())
	{
		INFO("verticies[%d]: %s", count, v->toString().c_str());
		int e_count = 0;
		for(const Edge * e : v->edges){
			INFO("  edges[%d]: %s", e_count, e->toString().c_str());
			e_count++;
		}
		count ++;
		INFO(" ");
	}
	INFO("### FACES ###");
	count = 0;
	for(const Face * f = _faceList.getFirst(); f != nullptr; f = f->getNext())
	{
		INFO("faces[%d]: %s", count, f->toString().c_str());
		count ++;
		INFO(" ");
	}
	INFO("### EDGES ###");
	count = 0;
	for(const Edge * e = _edgeList.getFirst(); e != nullptr; e = e->getNext())
	{
		INFO("edges[%d]: %s", count, e->toString().c_str());
		int f_count = 0;
		for(const Face * f : e->faces){
			INFO("  faces[%d]: %s", f_count, f->toString().c_str());
			f_count++;
		}
		count ++;
		INFO(" ");
	}

}

void DynamicMesh::set(const std::vector<Vector3D> & verticies, const std::vector<unsigned int>& indicies)
{
	// clear all
	clear();

	// create Vertex structs from vertex positions, temporarily store them in an array so that they are indexable
	std::vector<Vertex*> vertex_ptrs;
	for(const Vector3D & v : verticies){
		vertex_ptrs.push_back(new Vertex(v));
	}

	/* create triangular faces from indicies */
	size_t num_indicies = indicies.size();
	size_t num_verticies = vertex_ptrs.size();
	Vertex * v0;
	Vertex * v1;
	for(size_t i = 0; i < num_indicies-2; i += 3){
		/* make sure indicies are in range */
		assert(indicies[i+0] < num_verticies);
		assert(indicies[i+1] < num_verticies);
		assert(indicies[i+2] < num_verticies);

		/* create new face */
		Face* f = _faceList.add(new Face(
			vertex_ptrs[indicies[i+0]],
			vertex_ptrs[indicies[i+1]],
			vertex_ptrs[indicies[i+2]]
		));

		/* check if edges already exist and create new if not */
		for(int j = 0; j < 3; j++){
			v0 = f->v[j];
			v1 = f->v[(j+1)%3];
			if(v0->getEdgeWithOther(v1) == nullptr){
				Edge * e = _edgeList.add(new Edge(v0, v1));
				e->faces.insert(f);
				v0->edges.insert(e);
				v1->edges.insert(e);
			}
		}
	}

	for(Vertex * v : vertex_ptrs){
		_vertexList.add(v);
	}

	INFO(
		"Dynamic Mesh:\n"
		"  -> #Verticies: %ld\n"
		"  -> #Edges: %ld\n"
		"  -> #Faces: %ld\n",
		_vertexList.getSize(),
		_edgeList.getSize(),
		_faceList.getSize()
		);
	
}

void DynamicMesh::upload(zer0::Mesh & m)
{

}

void DynamicMesh::edgeCollapse(Edge * e, const Vector3D& new_position)
{
#define STR(V) (V->toString().c_str())
	/*****************************************************************************************
	 * Basic Idea:
	 * We are going to collapse the v[1] of the edge into v[0],
	 *  remove v[1] and set v[0] to the new position, so that v[0]
	 *  becomes essentially the new joined vertex.
	 * Therefore the collapsing edge needs to be removed from the edge list of v[0] and
	 *  all edges of v[1] need to be merged into the v[0] edge list.
	 * In addition we have to remove faces that contain this edge, and update the face lists of corresponding edges.
	 *****************************************************************************************/

	assert(e->v[0] != nullptr);
	assert(e->v[1] != nullptr);

	// remove edge from vertex edge list
	INFO("collapsing edge %s", STR(e));
	auto r = e->v[0]->edges.erase(e);
	assert(r == 1);// <-- if this assertion is not fullfilled, something must be wrong with initial setting of edges/verticies

	
	// merge second vertex edge set into first vertex edge set
	for(Edge * v1e: e->v[1]->edges){
		if(v1e != e){ // skip the collapsing edge (we still need it)
			Vertex * other_v = v1e->getOtherVertex(e->v[1]);
			Edge * other_e = other_v->getEdgeWithOther(e->v[0]);
			if(other_e == nullptr){// new edge, not connected to v[0] before
				v1e->setThisVertex(e->v[1], e->v[0]);
				INFO("(new edge) Insert edge to v0 : %s", STR(v1e));
				e->v[0]->edges.insert(v1e);
			}
			else{// this edge already exists in v[0] -> remove, so we don't get the same connection twice
				INFO("(edge already exists) Deleting edge : %s", STR(v1e));
				// remove from other
				other_v->edges.erase(v1e);

				// delete edge
				_edgeList.remove(v1e);	
			}
		}
	}
	// DO NOT access edges in v[1].edges from now on, as it might contain dangling pointers !

	// remove all faces that contain the collapsing edge
	for(Face * f : e->faces){
		// remove face from face list in other edges
		Vertex * other_v = f->getOtherVertex(e->v[0], e->v[1]);
		INFO("vertex that is not v[0] nor v[1] : %s", STR(other_v));

		assert(other_v != nullptr);
		Edge *v0e = other_v->getEdgeWithOther(e->v[0]);
		assert(v0e != nullptr);
		auto r = v0e->faces.erase(f);
		assert(r == 1);

		// delete face
		_faceList.remove(f);
	}

	// delete second vertex
	_vertexList.remove(e->v[1]);

	// delete collapsed edge
	_edgeList.remove(e);
}

void DynamicMesh::getEdgeMesh(zer0::Mesh & m, const Edge * e)const
{
	assert(e != nullptr);
	Vector3D v_data[2];
	v_data[0] = e->v[0]->position;
	v_data[1] = e->v[1]->position;
	INFO("Creating line: %f  ->  %f", v_data[0].z, v_data[1].z);
	m.set3D((float*)v_data, 2, Mesh::ONLY_POSITION, GL_LINES);
}

void DynamicMesh::getFaceMesh(zer0::Mesh & m, const Face * f)const
{
}

void DynamicMesh::getVertexMesh(zer0::Mesh & m, const Face * f)const
{
}
