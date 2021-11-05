#ifndef ZER0_MESH_H
#define ZER0_MESH_H
#include "glew/glew.h"
#include "zLogger.h"
#include "zVector3D.h"
#include "zVector2D.h"
#include "zShader.h"
#include <cassert>

namespace zer0{
	extern const Vector2D QUAD_VERTS[4];
	extern const Vector3D CUBE_VERTS[24];
	extern const Vector3D CUBE_NORMALS[6];
	/**
	 * Mesh class represents anything that can be drawn from verticies (e.g. 3D Cube, 2D Circle, ...)
	 */
	class Mesh{
		public:
			/*
			 * Primitive types to load with loadPrimitive()
			 */
			enum Primitive{BOX, SPHERE, CYLINDER, CONE, TORUS, CIRCLE, CIRCLE_LINE, QUAD, QUAD_LINE};

			/**
			 * vertex components 
			 */
			enum Components{NORMAL=0x01, UV=0x02, COLOR=0x04};

			/**
			 * Constructor
			 */
			Mesh():_buffer(0), _elementBuffer(0), _vertexCount(0), _elementCount(0), _drawMode(GL_TRIANGLES), _numDimensions(0){};

			/**
			 * Destructor
			 */
			~Mesh(){free();};

			/**
			 * Loading wavefront object from file
			 */
			bool loadOBJFromFile(const char * file);

			/**
			 * Loading wavefront object from string
			 */
			bool loadOBJ(const char * obj);

			/**
			 * Set vertices to represent the given primitive
			 * @param type The primitive to load
			 * @param dim Dimensions of the primitive
			 * @param segments Number of segments for rounded mesh (e.g. SPHERE, CYLINDER, CIRCLE)
			 * @param smooth If set to true: smooth normal generation for rounded meshes
			 */
			void loadPrimitive(Primitive type, const Vector3D & dim, int segments = 32, bool smooth = false);

			/**
			 * Free gl buffers
			 */
			void free();

			/**
			 * Binding buffer to vertex attribute locations and setting pointers.
			 * Must be called before drawing the mesh.
			 * @param flags specify what components to bind (if available)
			 */
			void bind(GLubyte flags = 0xFF);

			/**
			 * Drawing mesh. Call bind() before.
			 */
			void draw();

			/**
			 * Generate vertex data for circle parallel to the XY-plane.
			 * @param v Array to write vertex data to. There must be space allocated for at least sizeof(Vector3D)*segments
			 * @param segments Number of segments
			 * @param center Center around which the circle is formed
			 */
			static void generateCircleData(float radius, Vector3D * v, int segments, const Vector3D & center = Vector3D_zer0);
			static void generateCircleData(float radius, Vector2D * v, int segments, const Vector2D & center = Vector2D_zer0);


			GLuint getBuffer(){return _buffer;}
		private:
			GLuint _buffer;	
			GLuint _elementBuffer;
			int _numDimensions;//2D/3D object
			GLsizei _vertexCount;// number of verticies in buffer
			GLsizei _elementCount;// actual number of indicies in element buffer
			GLenum _drawMode;
			GLenum _elementType;
			GLubyte _flags;// flags store whether normals/uvs/colors are stored along with vertex position
	};
};

#endif
