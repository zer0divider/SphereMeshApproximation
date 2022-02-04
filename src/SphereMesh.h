#ifndef SPHERE_MESH_H
#define SPHERE_MESH_H

#include "zer0engine/zMath.h"
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

	/* 
	 * Calculate the offsets from sphere centers the tangent points of a plane will have on two spheres.
	 * This is needed for calculating the cylinders and triangles (interpolated spheres) of the sphere mesh.
	 * @offset1 x component is the offset from sphere center in direction to sphere s2, y component is the height to the sphere surface tangent point orthogonal to direction s1->s2 vector
	 * @offset2 same as offset1 but for sphere 2, offset2.x is also in the direction of the s1->s2 vector
	 * @dir if not set to nullptr, normalized direction vector from s1->s2 is returned through this pointer
	 * Remarks: offset1.x has always the same sign as offset2.x (or 0), offset1.y and offset2.y (heights) are always positive (or 0)
	 */
	static void calculateSphereTangent(const zer0::Vector4D & s1, const zer0::Vector4D & s2,
										zer0::Vector2D & offset1, zer0::Vector2D & offset2, zer0::Vector3D * dir = nullptr);

	/*
	 * Calculate intersection point of 2 planes given by n1.x*x + n1.y*y = offset1 and n2.x*x + n2.y*y = offset2 (2D plane equation)
	 * This is needed to calculate vertex points of triangles for sphere interpolation along faces.
	 * Therefore n1, n2 need to be normalized. offset1/offset2 is the distance from origin to the plane in normal direction.
	 * @return true on intersection, false if planes are parallel
	 */
	static bool intersectPlanes(const zer0::Vector2D & n1, float offset1,
								const zer0::Vector2D & n2, float offset2,
								zer0::Vector2D & intersect);

	/*
	 * Create an orthonormal base, (that is 3 unit length vectors all orthogonal to each other) from an initial vector t0.
	 * t0 must be unit length!
	 */
	static void createOrthonormalBase(const zer0::Vector3D & t0, zer0::Vector3D & t1, zer0::Vector3D & t2);
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
