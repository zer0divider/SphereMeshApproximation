#ifndef SPHERE_MESH_H
#define SPHERE_MESH_H

#include "zer0engine/zMesh.h"
#include "DynamicMesh.h"
#include <vector>

/* Sphere Mesh for drawing interpolated spheres along edges and faces
 * vertices = spheres
 * interpolation along edges = cylinders
 * interpolation along faces = triangle prisms
 */
class SphereMesh
{
public:
	SphereMesh();
	
	/*
	 * initialize from dynamic mesh
	 * @m the dynamic mesh to generate the sphere mesh from
	 * @num_segments is the number of segments used to generate the sphere and cylinder mesh, more segments = more smooth
	 * @min_sphere_radius if a sphere radius falls below this value, the sphere is not being rendered
	 * @cylinder_radius_offset offset cylinder radius by given amount so it doesn't 'punch through' sphere
	 * @min_cylinder_radius if both radii of a cylinder fall below this value, the cylinder is not being rendered
	 */
	void init(const DynamicMesh & m, int num_segments = 32, float min_sphere_radius = 0.001f, float cylinder_radius_offset = 0.002f, float min_cylinder_radius = 0.001f);

	/* 
	 * set colors for drawing
	 */
	void setColors(const zer0::Color & spheres, const zer0::Color & cylinders, const zer0::Color & triangles){
		_sphereColor = spheres;
		_cylinderColor = cylinders;
		_triangleColor = triangles;
	}

	/*
	 * set position
	 */
	void setPosition(const zer0::Vector3D & p){_position = p;}

	/*
	 * clear all meshes
	 */
	void clear();

	/* render all */
	void draw();

private:
	zer0::Vector3D _position;	
	std::vector<zer0::Vector4D> _spheres;
	zer0::Mesh _sphereMesh;
	std::vector<zer0::Mesh> _cylinderMeshes;
	zer0::Mesh _trianglesMesh;

	zer0::Color _sphereColor;
	zer0::Color _cylinderColor;
	zer0::Color _triangleColor;
};

#endif
