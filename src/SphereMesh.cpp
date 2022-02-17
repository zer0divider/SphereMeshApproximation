#include "SphereMesh.h"

using namespace zer0;

SphereMesh::SphereMesh(float sphere_radius_offset): _sphereColor(Color::WHITE), _cylinderColor(Color::WHITE), _triangleColor(Color::WHITE),
							 _sphereRadiusOffset(sphere_radius_offset)
{
}

void SphereMesh::clear()
{
	_spheres.clear();
	_sphereMesh.clear();
	_cylinderMeshes.clear();
	_trianglesMesh.clear();
}

void SphereMesh::init(const DynamicMesh & m, int num_segments, float min_sphere_radius, float min_cylinder_radius)
{
	// creating single sphere
	_sphereMesh.loadPrimitive(Mesh::SPHERE, Vector3D(2,2,2), num_segments);
	const DynamicMesh::Vertex * vert_i = m.getVertexList().getFirst();
	size_t num_verts = m.getVertexList().getSize();
	_spheres.resize(num_verts);
	int v_count = 0;
	float min_dist = 1000;
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
		if(r1 >= min_cylinder_radius || r2 >= min_cylinder_radius){
			Vector3D start = edge_i->v[0]->position;
			Vector3D end   = edge_i->v[1]->position;

			// create local orthonormal coordinate system t0, t1, t2
			Vector3D t0 = (end-start).getNormalized();
			Vector2D off1, off2;
			calculateSphereTangent(Vector4D(start, r1), Vector4D(end, r2), off1, off2);
			start = start + t0*off1.x;
			end = end + t0*off2.x;
			r1 = off1.y;
			r2 = off2.y;
			float len = (start-end).getLength();
			Vector3D t1, t2;
			createOrthonormalBase(t0, t1, t2);

			Vector3D * pos = (Vector3D*)vertex_data;
			Vector3D * norm = (Vector3D*)(vertex_data+vertex_count*3);
			int c = 0;
			// create cylinder vertices
			float r_delta = r2-r1;
			float sin_alpha = r_delta/len;
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
	vertex_data = nullptr;

	// creating triangle mesh
	const DynamicMesh::Face * face_i = m.getFaceList().getFirst();
	size_t num_faces = m.getFaceList().getSize();
	vertex_data = new float[num_floats_per_vert*3*num_faces*2];
	Vector3D * face_p = (Vector3D*)(vertex_data);
	Vector3D * face_n = (Vector3D*)(vertex_data+3*num_faces*3*2);
	int face_count = 0;
	Vector2D offsets[6];
	Vector2D norm_dirs[3];
	for(; face_i != nullptr; face_i = face_i->getNext()){
		// calculate positions for face in normal direction
		Vector3D *f = &face_p[face_count*3*2];
		Vector4D face_spheres[3];
		Vector3D t0 = face_i->normal;
		Vector3D t1, t2;
		createOrthonormalBase(t0, t1, t2);
		for(int vi = 0 ; vi < 3; vi++){
			face_spheres[vi].set(face_i->v[vi]->position, face_i->v[vi]->sphere_radius);
		}
		// calculate cylinder offsets and radii
		for(int vi = 0; vi < 3; vi++){
			int vi_next = (vi+1)%3;
			Vector3D n;
			calculateSphereTangent(face_spheres[vi], face_spheres[vi_next], offsets[vi*2], offsets[vi*2+1], &n);
			// project n onto this 2D face/plane defined by face normal
			norm_dirs[vi].set(Vector3D::dot(n, t1), Vector3D::dot(n, t2));
		}

		bool visible = true;
		// calculate intersections
		for(int vi = 0; vi < 3; vi++){
			int o1 = vi*2;
			int o2 = (o1 + 5)%6;
			int n1 = vi;
			int n2 = (vi+2)%3;
			float r = face_i->v[vi]->sphere_radius;
			float r_2 = r*r;
			if(r > 0.0f){// non-zer0 radius
				Vector2D intersect;
				if(intersectPlanes(norm_dirs[n1], offsets[o1].x, norm_dirs[n2], offsets[o2].x, intersect)){
					float len_2 = intersect.getSquaredLength();
					if(len_2 < r_2){
						float z = sqrt(r_2-len_2);
						Vector3D globalxy = face_i->v[vi]->position + intersect.x*t1 + intersect.y*t2;
						Vector3D normal_offset = z*t0;
						f[vi]   = globalxy + normal_offset;
						f[vi+3] = globalxy - normal_offset;
					}
					else{// intersection outside of sphere -> cylinders do not intersect
						visible = false;
						break;
					}
				}
				else{// no intersection -> we can skip this face, as it lies inside the spheres/cylinders and won't be visible
					visible = false;
					break;
				}
			}
			else{
				f[vi] = face_i->v[vi]->position;
				f[vi+3] = f[vi];
			}
		}
		if(visible){
			// calculate normal 1
			Vector3D n = Vector3D::cross(f[1] - f[0], f[2] - f[0]);
			for(int ni = 0; ni < 3; ni++){
				face_n[face_count*3*2 + ni] = n;
			}

			// calculate normal 2
			n = -Vector3D::cross(f[4] - f[3], f[5] - f[3]);
			for(int ni = 0; ni < 3; ni++){
				face_n[face_count*3*2 + ni + 3] = n;
			}
		}
		else{
			// set face to the original face
			for(int vi = 0; vi < 3; vi++){
				f[vi] = face_i->v[vi]->position + face_i->v[vi]->sphere_radius*face_i->normal;
				f[vi+3] = face_i->v[vi]->position - face_i->v[vi]->sphere_radius*face_i->normal;
				face_n[face_count*3*2 + vi ] = face_i->normal;
				face_n[face_count*3*2 + vi +3] = -face_i->normal;
			}
		}
		// reorder verticies for back faces to be culled correctly
		Vector3D swap = f[3];
		f[3] = f[4];
		f[4] = swap;
		face_count++;
	}
	_trianglesMesh.set3D(vertex_data, 2*3*face_count, Mesh::NORMAL, GL_TRIANGLES);
	delete[] vertex_data;
}

void SphereMesh::draw()
{
	drawSpheres();
	drawCylinders();
	drawTriangles();
}

void SphereMesh::drawSpheres()
{
	Matrix4 m;
	// draw spheres
	SHADER->setColor(_sphereColor);
	_sphereMesh.bind();
	for(const Vector4D & v : _spheres){
			m.setTransform(_position + v.getVector3D(), v.w-_sphereRadiusOffset, Vector3D_zer0);
			SHADER->setModelMatrix(m);
			_sphereMesh.draw();
	}
}

void SphereMesh::drawCylinders()
{
	Matrix4 m;
	m.setTranslation(_position);
	SHADER->setModelMatrix(m);
	// draw cylinders
	SHADER->setColor(_cylinderColor);
	for(size_t i = 0; i < _cylinderMeshes.size(); i++){
		_cylinderMeshes[i].bind();
		_cylinderMeshes[i].draw();
	}
}

void SphereMesh::drawTriangles()
{
	Matrix4 m;
	m.setTranslation(_position);
	SHADER->setModelMatrix(m);
	// draw triangles
	SHADER->setColor(_triangleColor);
	_trianglesMesh.bind();
	_trianglesMesh.draw();
}

void SphereMesh::calculateSphereTangent(const zer0::Vector4D & s1, const zer0::Vector4D & s2,
									zer0::Vector2D & offset1, zer0::Vector2D & offset2, zer0::Vector3D * _dir)
{
	Vector3D s1_pos(s1.x, s1.y, s1.z);
	Vector3D s2_pos(s2.x, s2.y, s2.z);
	float r1 = s1.w;
	float r2 = s2.w;
	Vector3D dir = s2_pos-s1_pos;
	float d = dir.getLength();
	// sphere 1 has zer0 radius
	if(r1 == 0.f){
		offset1.set(0,0);// no offset for sphere 1
		if(r2 == 0.f){
			offset2.set(0,0); // just a straight line from s1 to s2
		}
		else{
			float r2_2 = (r2*r2);
			float p = -r2_2/d;
			float h = sqrt(r2_2-p*p);
			offset2.set(p, h);
		}
	}
	// sphere 2 has zer0 radius
	else if(r2 == 0.f){
		offset2.set(0,0);// no offset for sphere 2
		if(r1 == 0.f){
			offset1.set(0,0); // just a straight line from s1 to s2
		}
		else{
			float r1_2 = (r1*r1);
			float p = r1_2/d;
			float h = sqrt(r1_2-p*p);
			offset1.set(p, h);
		}
	}
	else{// both spheres have non-zer0 radius
		float D = (r1*d)/(r1-r2);
		float r1_2 = (r1*r1);
		float x1 = r1_2/D;
		float h1 = sqrt(r1_2-x1*x1);
		offset1.set(x1, h1);

		float r2_over_r1 = r2/r1;
		float x2 = x1*r2_over_r1;
		float h2 = h1*r2_over_r1;
		offset2.set(x2, h2);
	}
	if(_dir != nullptr){
		*_dir = dir/d;
	}
}

bool SphereMesh::intersectPlanes(const Vector2D & n1, float offset1,
								 const Vector2D & n2, float offset2,
								 Vector2D & intersect)
{
	if(equals(fabs(Vector2D::dot(n1, n2)), 1.f)){
		return false;
	}
	
	if(n1.x == 0){// special case: plane 1 is parallel to x axis
		intersect.x = (offset2 - (n2.y*offset1)/n1.y)/n2.x;
		intersect.y = offset1/n1.y;
	}
	else{// normal case: arbitrary planes
		intersect.y = (offset2-n2.x*(offset1/n1.x))/(n2.y-n2.x*(n1.y/n1.x));
		intersect.x = (offset1-n1.y*intersect.y)/n1.x;
	}

	return true;
}

void SphereMesh::createOrthonormalBase(const zer0::Vector3D & t0, zer0::Vector3D & t1, zer0::Vector3D & t2)
{
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
	t2 = Vector3D::cross(t0, t1);
}
