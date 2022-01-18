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

DynamicMesh::Edge* DynamicMesh::Face::getSharedEdgeWith(Face * f)
{
	int shared[2];
	int s_count = 0;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++){
			if(v[i] == f->v[j]){
				shared[s_count] = i;
				s_count++;
				if(s_count == 2){
					break;
				}
			}
		}
		if(s_count == 2){
			break;
		}
	}
	if(s_count != 2){
		return nullptr;
	}
	else{
		return v[shared[0]]->getEdgeWithOther(v[shared[1]]);
	}
}

void DynamicMesh::Face::removeThisFromEdges()
{
	for(int i = 0; i < 3; i++){
		Edge * e = v[i]->getEdgeWithOther(v[(i+1)%3]);
		assert(e != nullptr);
		e->faces.erase(this);
	}
}

#define _FACE_OTHER_VERTEX \
	if(v[0] == not_this_one){\
		if(v[1] == and_not_this_one){\
			return v[2];\
		}\
		else{\
			return v[1];\
		}\
	}\
	else if(v[1] == not_this_one){\
		if(v[0] == and_not_this_one){\
			return v[2];\
		}\
		else{\
			return v[0];\
		}\
	}\
	else if(v[2] == not_this_one){\
		if(v[1] == and_not_this_one){\
			return v[0];\
		}\
		else{\
			return v[1];\
		}\
	}\
	return nullptr;

DynamicMesh::Vertex* DynamicMesh::Face::getOtherVertex(const Vertex * not_this_one, const Vertex * and_not_this_one)
{
	_FACE_OTHER_VERTEX;
}

const DynamicMesh::Vertex* DynamicMesh::Face::getOtherVertex(const Vertex * not_this_one, const Vertex * and_not_this_one)const
{
	_FACE_OTHER_VERTEX;
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
			Edge * e = v0->getEdgeWithOther(v1);
			if(e == nullptr){
				e = _edgeList.add(new Edge(v0, v1));
				v0->edges.insert(e);
				v1->edges.insert(e);
			}
			e->faces.insert(f);
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
	// assign a unique id to every Vertex and store their positions in an array
	const size_t num_verts = _vertexList.getSize();
	Vector3D * verts = new Vector3D[num_verts];
	int i = 0;
	for(Vertex * v = _vertexList.getFirst(); v != nullptr; v = v->getNext()){
		v->id = i;
		verts[i] = v->position;
		i++;
	}
	
	// iterate all faces and store vertex indicies
	const size_t num_indicies = 3*_faceList.getSize();
	unsigned int * indicies = new unsigned int[num_indicies];
	i = 0;
	for(Face * f = _faceList.getFirst(); f != nullptr; f = f->getNext()){
		for(int fi =0; fi < 3; fi++){
			indicies[i] = f->v[fi]->id;
			i++;
		}
	}

	// set new verticies in mesh, this removes all normal data
	m.set3DIndexed(
		(float*)verts, num_verts,
		indicies, num_indicies,
		Mesh::ONLY_POSITION, GL_TRIANGLES);

	delete[] indicies;
	delete[] verts;
}

void DynamicMesh::integrity_check()
{
	INFO("Checking mesh integrity...");
	// check vertex connections
	for(const Vertex * v = _vertexList.getFirst(); v != nullptr; v = v->getNext()){
		for(Edge * e : v->edges){
			Vertex * v_other = e->getOtherVertex(v);
			assert(v_other->getEdgeWithOther(v) == e);
		}
	}

	// check edges
	for(const Edge * e = _edgeList.getFirst(); e != nullptr; e = e->getNext()){
		assert(e->v[0]->getEdgeWithOther(e->v[1]) == e);

		// check connected faces
		for(const Face * f : e->faces){
			assert(f->hasVertex(e->v[0]));
			assert(f->hasVertex(e->v[1]));

			const Vertex * other_v = f->getOtherVertex(e->v[0], e->v[1]);
			assert(other_v != e->v[0]);
			assert(other_v != e->v[1]);
		}
	}

	// check faces
	for(Face * f = _faceList.getFirst(); f != nullptr; f = f->getNext()){
		for(int i = 0; i < 3; i++){
			Edge * e = f->v[i]->getEdgeWithOther(f->v[(i+1)%3]);
			assert(e != nullptr);
			assert(e->faces.find(f) != e->faces.end());
		}
	}

	INFO("-> OK.");
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

	Vertex * v0 = e->v[0];
	Vertex * v1 = e->v[1];
	assert(v0 != nullptr);
	assert(v1 != nullptr);

	// remove edge from vertex edge list
	INFO("collapsing edge %s", STR(e));
	//auto r = v0->edges.erase(e);
	//assert(r == 1);// <-- if this assertion is not fullfilled, something must be wrong with initial setting of edges/verticies
	//r = v1->edges.erase(e);// this should actually not be necessary, since v1 is going to be removed anyways, but, we want to make sure, that if w iterate over the edges of v1, the collapsing edge is skipped
	//assert(r == 1);


	// merge second vertex edge set into first vertex edge set
	std::vector<Face *> faces_to_be_removed;
	std::vector<Edge *> edges_to_be_removed;

	for(Edge * e_i: v0->edges){
		if(e_i != e){// skip collapsing edge
			Vertex * v_i = e_i->getOtherVertex(v0);
			Edge * e_j = v_i->getEdgeWithOther(v1);
			if(e_j != nullptr){
				// find face that is either shared by both edges or find two faces that share the same edge
				for(Face * f_j: e_j->faces){
					bool f_j_removed = false;
					for(Face * f_i: e_i->faces){
						if(f_j == f_i){// faces are identical -> mark as removed
							f_j_removed = true;
							break;
						}
						else{// check if faces share an edge
							Edge * e_shared = f_j->getSharedEdgeWith(f_i);
							if(e_shared != nullptr && !e_shared->hasVertex(v0) && !e_shared->hasVertex(v1)){
								f_j_removed = true;
								break;
							}
						}
					}

					if(f_j_removed){// face removed
						faces_to_be_removed.push_back(f_j);
					}
					else{// not removed? -> update vertex and edges
						f_j->setThisVertex(v1, v0);
						e_i->faces.insert(f_j);
					}
				}
				// remove all faces in marked list
				for(Face * f : faces_to_be_removed){
					f->removeThisFromEdges();
					_faceList.remove(f);
				}
				faces_to_be_removed.clear();

				// remove edge from v1
				edges_to_be_removed.push_back(e_j);
			}
			
		}
	}

	// remove all merged edges
	for(Edge * e_i : edges_to_be_removed){
		e_i->getOtherVertex(v1)->edges.erase(e_i);
		v1->edges.erase(e_i);
		_edgeList.remove(e_i);
	}

	v1->edges.erase(e);
	// move all edges that have not been deleted from v1 to v0, and reconnect faces
	for(Edge * e_i : v1->edges){
		e_i->setThisVertex(v1, v0);
		v0->edges.insert(e_i);
		for(Face * f : e_i->faces){
			f->setThisVertex(v1, v0);
		}
	}
	
	// move vertex to new merged position
	v0->position = new_position;

	// remove collapsing edge from v0
	v0->edges.erase(e);

	// delete v1
	_vertexList.remove(v1);
	
	// delete collapsed edge
	_edgeList.remove(e);
}

void DynamicMesh::Edge::upload(zer0::Mesh & m)const
{
	Vector3D v_data[2];
	v_data[0] = v[0]->position;
	v_data[1] = v[1]->position;
	m.set3D((float*)v_data, 2, Mesh::ONLY_POSITION, GL_LINES);
}

void DynamicMesh::Edge::uploadFaces(zer0::Mesh & m)const
{
	if(faces.size() == 0){
		m.clear();
	}else{
		Vector3D v_data[3*faces.size()];
		int i = 0;
		for(Face * f : faces){
			v_data[i++] = f->v[0]->position;
			v_data[i++] = f->v[1]->position;
			v_data[i++] = f->v[2]->position;
		}
		m.set3D((float*)v_data, 3*faces.size(), Mesh::ONLY_POSITION, GL_TRIANGLES);
	}
}

void DynamicMesh::getFaceMesh(zer0::Mesh & m, const Face * f)const
{
}

void DynamicMesh::getVertexMesh(zer0::Mesh & m, const Face * f)const
{
}
