/* Author: Cornelius Marx
 */
#ifndef DYNAMIC_MESH_H
#define DYNAMIC_MESH_H

#include "zer0engine/zMesh.h"
#include <vector>
#include <set>
#include <assert.h>
#include "BackReferenceList.h"
#include "SQEM.h"
#include <queue>

/**
 * offline mesh format, for quickly applying geometry transformations (e.g. edge collapse)
 */
class DynamicMesh
{
public:
	/**
	 * forward declaration
	 */
	struct Edge;
	struct Face;
	struct CollapseCostCompare;

	/* type of priority queue
	 * NOTE: we do not use a std::prio_queue here because removal of arbitrary elements is not possible
	 *  we have to live with the fact that inserting in order now takes O(n) time
	 */
	typedef std::priority_queue<Edge*, std::vector<Edge*>, CollapseCostCompare> CollapseListType;

	/**
	 * vertex that holds a position
	 */
	struct Vertex : public BackReferenceList<Vertex>::Item
	{
		Vertex(){}
		~Vertex()override{}
		Vertex(const zer0::Vector3D & _position): position(_position){}

		/**
		 * returns the edge in the edges-set that the given other vertex is part of
		 * if there is no such edge, then nullptr is returned
		 */
		Edge* getEdgeWithOther(const Vertex * other){
			for(Edge* e : edges){
				if(e->hasVertex(other)){
					return e;
				}
			}
			return nullptr;
		}

		// get all faces
		void getFaces(std::set<Face*> & f);

		zer0::Vector3D position;
		std::set<Edge*> edges; // edges to connected verticies

		std::string toString()const;
		size_t id; // id when verticies are moved to normal array
		SQEM Q;
		float sphere_radius;
	};

	/**
	 * triangle face formed by three verticies
	 */
	struct Face : public BackReferenceList<Face>::Item
	{
		Face(Vertex * _v0, Vertex * _v1, Vertex * _v2):v{_v0, _v1, _v2}{calculateNormal();}
		~Face()override{}
		Vertex * v[3];
		zer0::Vector3D normal;

		/**
		 * set the local element in v to new_vertex that matches given this_vertex
		 * NOTE: if given vertex is not part of this edge, this is a nop
		 */
		void setThisVertex(Vertex *this_vertex, Vertex * new_vertex)
		{
			if(this_vertex == v[0]){
				v[0] = new_vertex;
			}else if(this_vertex == v[1]){
				v[1] = new_vertex;
			}else if(this_vertex == v[2]){
				v[2] = new_vertex;
			}
		}

		/**
		 * returns true if vertex is part of this face
		 */
		bool hasVertex(const Vertex * vertex)const{
			return vertex == v[0] || vertex == v[1] || vertex == v[2];
		}

		/**
		 * returns the edge that both faces share, or nullptr if no such edge
		 */
		Edge* getSharedEdgeWith(Face * f);

		/**
		 * remove this face from edge's face lists
		 */
		void removeThisFromEdges();

		/**
		 * get the other vertex that is neither of the given ones
		 * NOTE: this assumes that the given verticies are part of the face
		 */
		Vertex* getOtherVertex(const Vertex * not_this_one, const Vertex * and_not_this_one);

		const Vertex* getOtherVertex(const Vertex * not_this_one, const Vertex * and_not_this_one)const;
		
		float getArea(){
			return 0.5f * zer0::Vector3D::cross(
				v[1]->position - v[0]->position,
				v[2]->position - v[0]->position).getLength();
		}

		/* calculate normal from all 3 vertex points, ordered CCW
		 */
		void calculateNormal();
		std::string toString()const {
			return "( v0" + v[0]->toString() + ", v1" + v[1]->toString() + ", v2" + v[2]->toString() + ")";
		}

		SQEM Q;
	};

	/**
	 * edge formed by two verticies
	 */
	struct Edge : public BackReferenceList<Edge>::Item
	{
		Edge(Vertex * _v0, Vertex * _v1): v{_v0, _v1}, needs_removal(false) {}
		Edge(const Edge * e): v{e->v[0], e->v[1]}, needs_removal(false){ faces = e->faces; assert(faces.size() == e->faces.size());}
		~Edge()override{}
		Vertex* v[2]; // two verticies form an edge
		
		/**
		 * get the other vertex that is not the given
		 * NOTE: this assumes that the given vertex is part of the edge
		 */
		Vertex* getOtherVertex(const Vertex *not_this_one){
			return not_this_one == v[0] ? v[1]: v[0];
		}

		/**
		 * set the local element in v to new_vertex that matches given this_vertex
		 * NOTE: if given vertex is not part of this edge, this is a nop
		 */
		void setThisVertex(Vertex *this_vertex, Vertex * new_vertex){
			if(this_vertex == v[0]){
				v[0] = new_vertex;
			}
			else if(this_vertex == v[1]){
				v[1] = new_vertex;
			}
		}

		/**
		 * return true if given vertex is part of this edge
		 */
		bool hasVertex(const Vertex * vertex)const{
			return vertex == v[0] || vertex == v[1];
		}

		std::string toString()const {
			return "( v0" + v[0]->toString() + ", v1" + v[1]->toString() + ")";
		}
		
		/**
		 * set given mesh to a line mesh that represents this edge
		 */
		void upload(zer0::Mesh & m)const;

		void uploadFaces(zer0::Mesh & m)const;

		/**
		 * all faces that share this edge,
		 * NOTE: for most geometry there will be at most 2 faces that share the same edge
		 */
		std::set<Face*> faces;
		SQEM Q;
		double collapse_cost;
		float sphere_radius;
		zer0::Vector3D sphere_center;

		void updateSQEM(){
			Q = v[0]->Q + v[1]->Q;
			collapse_cost = Q.minimize<zer0::Vector3D, float>(sphere_center, sphere_radius, v[0]->position, v[1]->position);
		}

		bool needs_removal;// set to true if this edge should be deleted
	}; // struct Edge

	struct CollapseCostCompare{
		bool operator()(const Edge * e1, const Edge * e2){
			return e1->collapse_cost > e2->collapse_cost;
		}
	};

	/**
	 * constructor
	 */
	DynamicMesh();

	/**
	 * destructor
	 */
	~DynamicMesh();
	
	/**
	 * Set the mesh from triangle data
	 * @param verticies list of verticies
	 * @param indicies list of 3*N indicies, with N being the total number of triangles.
	 * Each element of indicies refers to a vertex in verticies. 3 successive indicies form a triangle in CCW order.
	 */
	void set(const std::vector<zer0::Vector3D> & verticies, const std::vector<unsigned int>& indicies);
	
	/**
	 * upload vertex data to regular mesh, so it can be rendered
	 */
	void upload(zer0::Mesh & face_mesh, zer0::Mesh & edge_mesh);

	void getEdgeMesh(zer0::Mesh & m, const Edge * e)const;
	void getFaceMesh(zer0::Mesh & m, const Face * f)const;
	void getVertexMesh(zer0::Mesh & m, const Vertex * v)const;

	/* get edge with currently lowest cost
	 * if no more edges, returns nullptr
	 */
	Edge* getBestCollapseEdge(){return _collapseList.empty()? nullptr : _collapseList.top();}

	/**
	 * collapse edge to new position
	 * the vertex that remains at the collapsed position is given by new_vertex (if not null)
	 * The edges that were removed during the process are returned through deleted_edges if not nullptr, else the edges are freed from memory. This does not include the given edge e (always freed).
	 */
	void edgeCollapse(Edge * e, const zer0::Vector3D& new_position,
			Vertex ** new_vertex = nullptr, std::vector<Edge*> * removed_edges = nullptr);

	void edgeCollapseToCenter(Edge * e){
		edgeCollapse(e, (e->v[0]->position+e->v[1]->position)/2, nullptr, nullptr);
	}

	/** calculate SQEM in every vertex of the mesh so that approximation can start
	*/
	void initSQEM();
	
	/**
	 * remove all verticies, faces and edges
	 */
	void clear();


	/**
	 * run sphere mesh approximation based on SQEM until the given number of spheres is reached
	 * NOTE: initSQEM() has to be called first
	 */
	void sphereApproximation(int num_spheres);

	/*
	 * perform a single step for sphere approximation
	 * NOTE: initSQEM() has to be called first
	 */
	void sphereApproximationStep();

	void debug_print();

	void integrity_check();

	BackReferenceList<Vertex>& getVertexList(){return _vertexList;}
	BackReferenceList<Face>& getFaceList(){return _faceList;}
	BackReferenceList<Edge>& getEdgeList(){return _edgeList;}

private:
	BackReferenceList<Vertex> _vertexList;
	BackReferenceList<Face> _faceList;
	BackReferenceList<Edge> _edgeList;

	CollapseListType _collapseList; // edge collapses to be considered for mesh approximation, sorted by cost
};

#endif
