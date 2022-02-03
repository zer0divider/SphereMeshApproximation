#include "SphereMesh.h"

using namespace zer0;

SphereMesh::SphereMesh(): _sphereColor(Color::WHITE), _cylinderColor(Color::WHITE), _triangleColor(Color::WHITE)
{
}

void SphereMesh::clear()
{
	_spheres.clear();
	_sphereMesh.clear();
	_cylinderMeshes.clear();
	_trianglesMesh.clear();
}

void SphereMesh::init(const DynamicMesh & m, int num_segments, float min_sphere_radius, float cylinder_radius_offset, float min_cylinder_radius)
{
	// creating single sphere
	_sphereMesh.loadPrimitive(Mesh::SPHERE, Vector3D(2,2,2), num_segments);
	const DynamicMesh::Vertex * vert_i = m.getVertexList().getFirst();
	size_t num_verts = m.getVertexList().getSize();
	_spheres.resize(num_verts);
	int v_count = 0;
	for(; vert_i != nullptr; vert_i = vert_i->getNext()){
		if(vert_i->sphere_radius >= min_sphere_radius){
			_spheres[v_count].set(vert_i->position, vert_i->sphere_radius);
			v_count++;
		}
	}
	_spheres.resize(v_count);

	int num_floats_per_vert = 6;
	// creating multiple cylinder meshes
	const DynamicMesh::Edge * edge_i = m.getEdgeList().getFirst();
	size_t num_edges = m.getEdgeList().getSize();
	_cylinderMeshes.resize(num_edges);
	int vertex_count = (num_segments+1)*2;
	float * vertex_data = new float[num_floats_per_vert*vertex_count];
	int e_count = 0;
	for(; edge_i != nullptr; edge_i = edge_i->getNext()){
		float r1 = edge_i->v[0]->sphere_radius;
		float r2 = edge_i->v[1]->sphere_radius;
		r1 -= cylinder_radius_offset;
		r2 -= cylinder_radius_offset;
		if(r1 >= min_cylinder_radius && r2 >= min_cylinder_radius){
			Vector3D start = edge_i->v[0]->position;
			Vector3D end   = edge_i->v[1]->position;

			Vector3D delta = end-start;
			float height = delta.getLength();

			// create local orthonormal coordinate system t0, t1, t2
			Vector3D t0 = delta/height;
			Vector3D t1;
			if(fabs(t0.z) < fabs(t0.y)){
				if(fabs(t0.z) < fabs(t0.x)){// z is the smallest component
					t1.set(-t0.y, t0.x, 0);
				}
				else{ // x is the smallest
					t1.set(0, -t0.z, t0.y);
				}
			}
			else{
				if(fabs(t0.y) < fabs(t0.x)){// y is the smallest component
					t1.set(t0.z, 0, -t0.x);
				}
				else{ // x is the smallest
					t1.set(0, -t0.z, t0.y);
				}
			}
			t1.normalize();
			Vector3D t2 = Vector3D::cross(t0, t1);

			Vector3D * pos = (Vector3D*)vertex_data;
			Vector3D * norm = (Vector3D*)(vertex_data+vertex_count*3);
			int c = 0;
			// create cylinder vertices
			float r_delta = r2-r1;
			float sin_alpha = r_delta/height;
			for(int i = 0; i < num_segments; i++){
				float angle = 2*M_PI*i/num_segments;
				Vector3D circle_pos = cos(angle)*t1 + sin(angle)*t2;
				Vector3D n = circle_pos*fabs(r_delta) - sin_alpha*t0*fabs(r_delta);
				n.normalize();
				pos[c] = end + r2*circle_pos;
				norm[c] = n;
				c++;
				pos[c] = start + r1*circle_pos;
				norm[c] = n;
				c++;
			}
			pos[c] = pos[0];
			norm[c] = norm[0];
			c++;
			pos[c] = pos[1];
			norm[c] = norm[1];

			_cylinderMeshes[e_count].set3D(vertex_data, vertex_count, Mesh::NORMAL, GL_TRIANGLE_STRIP);
			e_count++;
		}
	}
	_cylinderMeshes.resize(e_count);
	delete[] vertex_data;

	// creating triangle mesh
}

void SphereMesh::draw()
{
	// draw spheres
	SHADER->setColor(_sphereColor);
	_sphereMesh.bind();
	Matrix4 m;
	for(const Vector4D & v : _spheres){
			m.setTransform(_position + v.getVector3D(), v.w, Vector3D_zer0);
			SHADER->setModelMatrix(m);
			_sphereMesh.draw();
	}

	m.setTranslation(_position);
	SHADER->setModelMatrix(m);

	// draw cylinders
	SHADER->setColor(_cylinderColor);
	for(size_t i = 0; i < _cylinderMeshes.size(); i++){
		_cylinderMeshes[i].bind();
		_cylinderMeshes[i].draw();
	}

	// draw triangles
	/*
	SHADER->setColor(_triangleColor);
	_trianglesMesh.bind();
	_trianglesMesh.draw();
	*/
}
