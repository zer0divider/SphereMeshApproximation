#include "DynamicMesh.h"

using namespace zer0;

std::string DynamicMesh::Vertex::toString()const
{

	return "(" +
		std::to_string(position.x) + ", " +
		std::to_string(position.y) + ", " +
		std::to_string(position.z) + ")";
}

void DynamicMesh::Vertex::getFaces(std::set<Face*> & faces)
{
	for(Edge * e: edges){
		for(Face * f: e->faces){
			faces.insert(f);
		}
	}
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
	float inf = std::numeric_limits<float>::infinity();
	Vector3D v_min(inf, inf, inf);
	Vector3D v_max(-inf, -inf, -inf);
	for(const Vector3D & v : verticies){
		vertex_ptrs.push_back(new Vertex(v));
		if(v.x < v_min.x){
			v_min.x = v.x;
		}
		if(v.x > v_max.x){
			v_max.x = v.x;
		}
		if(v.y < v_min.y){
			v_min.y = v.y;
		}
		if(v.y > v_max.y){
			v_max.y = v.y;
		}
		if(v.z < v_min.z){
			v_min.z = v.z;
		}
		if(v.z > v_max.z){
			v_max.z = v.z;
		}
	}
	_centerPos = (v_min+v_max)/2.f;

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

void DynamicMesh::upload(zer0::Mesh & face_mesh, zer0::Mesh & edge_mesh)
{
	static const int average_faces_per_vertex = 4;
	// assign a unique id to every Vertex and store their positions in an array
	const size_t num_verts = _vertexList.getSize();
	std::vector<VN> verts; 
	verts.reserve(num_verts*average_faces_per_vertex);

	for(Vertex * v = _vertexList.getFirst(); v != nullptr; v = v->getNext()){
		v->id = verts.size();

		// get all faces from vertex
		std::set<Face*> faces;
		v->getFaces(faces);
		Vector3D accum_normal(0,0,0);
		for(Face * f : faces){// for each face add vertex with corresponding normal
			f->calculateNormal();
			accum_normal += f->normal;
		}
		verts.push_back(VN(v->position, accum_normal.getNormalized()));
		for(Face * f : faces){
			verts.push_back(VN(v->position, f->normal));
		}
	}
	
	// face mesh
	// iterate all faces and store vertex indices
	const size_t num_indices = 3*_faceList.getSize();
	unsigned int * indices = new unsigned int[num_indices];
	int index_count = 0;
	for(Face * f = _faceList.getFirst(); f != nullptr; f = f->getNext()){
		for(int fi =0; fi < 3; fi++){
			// search for this face inside vertex face list
			std::set<Face*> faces;
			f->v[fi]->getFaces(faces);
			int j = 0;
			for(Face * fj : faces){
				if(fj == f){
					break;
				}
				j++;
			}
			indices[index_count] = f->v[fi]->id + j + 1;
			index_count++;
		}
	}

	// put vertices and normals into a single buffer with first verts and then normals 
	unsigned int v_size = verts.size();
	Vector3D * vert_norms = new Vector3D[v_size*2];
	for(unsigned int i = 0; i < v_size; i++){
		vert_norms[0      + i] = verts[i].v;
		vert_norms[v_size + i] = verts[i].n;
	}

	// set new verticies in mesh
	face_mesh.set3DIndexed(
		(float*)vert_norms, v_size,
		indices, num_indices,
		Mesh::NORMAL, GL_TRIANGLES);
	delete[] indices;


	// edge mesh
	const size_t num_edge_indices = 2*_edgeList.getSize();
	unsigned int * edge_indices = new unsigned int[num_edge_indices];
	int edge_count = 0;
	for(Edge * e = _edgeList.getFirst(); e != nullptr; e = e->getNext()){
		edge_indices[edge_count] = e->v[0]->id;	
		edge_count++;
		edge_indices[edge_count] = e->v[1]->id;	
		edge_count++;
	}

	edge_mesh.set3DIndexed(
		(float*)vert_norms, v_size,
		edge_indices,
		num_edge_indices,
		Mesh::ONLY_POSITION, GL_LINES);

	delete[] edge_indices;
	delete[] vert_norms;
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

void DynamicMesh::edgeCollapse(Edge * e, const zer0::Vector3D& new_position, Vertex ** new_vertex, std::vector<Edge*> * removed_edges)
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
	//INFO("collapsing edge %s", STR(e));
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
		// store removed edges pointers
		if(removed_edges != nullptr){
			removed_edges->push_back(e_i);
		}
		else{
			_edgeList.remove(e_i);
		}
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
	if(new_vertex != nullptr){
		*new_vertex = v0;
	}

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

void DynamicMesh::getEdgeMesh(zer0::Mesh & m, const Edge * e)const
{
	ERROR("Not implemented yet!");
	assert(false);
}

void DynamicMesh::getFaceMesh(zer0::Mesh & m, const Face * f)const
{
	ERROR("Not implemented yet!");
	assert(false);
}

void DynamicMesh::getVertexMesh(zer0::Mesh & m, const Vertex * v)const
{
	ERROR("Not implemented yet!");
	assert(false);
}

void DynamicMesh::initSQEM()
{
	// calculate SQEM for each face
	for(Face* f = _faceList.getFirst(); f != nullptr; f = f->getNext()){
		f->Q.setFromPlane<zer0::Vector3D>(f->v[0]->position, f->normal);
	}

	// calculate SQEM for each vertex based on SQEM of faces
	for(Vertex* v = _vertexList.getFirst(); v != nullptr; v = v->getNext()){
		v->Q.setZero();
		v->sphere_radius = 0.f;
		// get all faces that contain this vertex
		std::set<Face*> faces;
		v->getFaces(faces);
		for(Face * f: faces){
			float area = f->getArea();
			v->Q +=  f->Q * (area/3.f);
		}
	}

	// minimize SQEM for each edge (vertex-pair) and put in prio queue
	for(Edge * e = _edgeList.getFirst(); e != nullptr; e = e->getNext()){
		e->updateSQEM();
		_collapseList.push(e);
	}
}

void DynamicMesh::sphereApproximation(int num_spheres)
{
	while(_vertexList.getSize() > num_spheres){
		sphereApproximationStep();
	}
}

void DynamicMesh::sphereApproximationStep()
{
	// assume initSQEM() has been called at this point
	/////
	if(_collapseList.empty()){
		INFO("No more edges to collapse.");
		return;
	}

	// take next best collapse candidate
	Edge * collapsing_edge = _collapseList.top();
	_collapseList.pop();
	assert(collapsing_edge->needs_removal == false);

	SQEM new_q = collapsing_edge->Q;
	float sphere_radius = collapsing_edge->sphere_radius;
	Vertex * v;
	std::vector<Edge*> removed_edges;
	edgeCollapse(collapsing_edge, collapsing_edge->sphere_center, &v, &removed_edges);
	collapsing_edge = nullptr;
	
	v->Q = new_q;
	v->sphere_radius = sphere_radius;

	// mark edges from prio queue as 'removed' and remove from global edge list
	std::vector<Edge*>::iterator it = removed_edges.begin();
	while(it != removed_edges.end()){
		(*it)->needs_removal = true;
		_edgeList.extract(*it);
		it++;
	}

	// recalculate and minimize SQEM of edges that have been changed, and reinsert into collapse list
	std::set<Edge*> update_edges = v->edges;
	for(Edge * e : update_edges){
		// remove old edge from list
		_edgeList.extract(e);
		e->v[0]->edges.erase(e);
		e->v[1]->edges.erase(e);

		// create new from old edge
		Edge * updated_edge = new Edge(e);
		updated_edge->v[0]->edges.insert(updated_edge);
		updated_edge->v[1]->edges.insert(updated_edge);
		_edgeList.add(updated_edge);
		updated_edge->updateSQEM();
		_collapseList.push(updated_edge);

		// mark old edge as 'removed'
		e->needs_removal = true;
	}

	// remove all edges marked as 'removed' from top so we have best candidate at top for next iteration
	while(!_collapseList.empty() && collapsing_edge == nullptr){
		collapsing_edge = _collapseList.top();
		if(collapsing_edge->needs_removal){
			_collapseList.pop();
			delete collapsing_edge;
			collapsing_edge = nullptr;
		}
	}
}
