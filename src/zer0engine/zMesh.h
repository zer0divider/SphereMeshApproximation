/* Author: Cornelius Marx
 */
#ifndef ZER0_MESH_H
#define ZER0_MESH_H
#include "glew/glew.h"
#include "zLogger.h"
#include "zVector3D.h"
#include "zVector2D.h"
#include "zShader.h"
#include <cassert>
#include <fstream>
#include <vector>
#include <unordered_map>

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
			enum Components{ONLY_POSITION=0x0, NORMAL=0x01, UV=0x02, COLOR=0x04};

			/**
			 * Constructor
			 */
			Mesh():_buffer(0), _elementBuffer(0), _vertexCount(0), _elementCount(0), _drawMode(GL_TRIANGLES), _numDimensions(0){};

			/**
			 * Destructor
			 */
			~Mesh(){clear();};

			/**
			 * Loading wavefront object from file
			 * @param filename .obj file to load
			 * @param components what components (e.g. NORMAL, UV) to load if available in file
			 * @param verticies see loadOBJ()
			 * @param indicies see loadOBJ()
			 * @return false on error, true on success
			 */
			bool loadOBJFromFile(const char * filename, 
							unsigned char components = 0xFF,
							std::vector<Vector3D> * verticies = nullptr,
							std::vector<unsigned int> * indicies = nullptr);

			/**
			 * Loading wavefront object from string
			 * @param obj 0 terminated string describing the object in Wavefront format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
			 * @param components what components (e.g. NORMAL, UV) to load if available in file
			 * @param verticies if not nullptr vertex data is put into this vector
			 * @param indicies if not nullptr index data (element buffer) is put into this vector
			 * @return false on error, true on success
			 */
			bool loadOBJ(const char * obj, 
							unsigned char components = 0xFF,
							std::vector<Vector3D> * verticies = nullptr,
							std::vector<unsigned int> * indicies = nullptr);

			/**
			 * Set vertices to represent the given primitive
			 * @param type The primitive to load
			 * @param dim Dimensions of the primitive
			 * @param segments Number of segments for rounded mesh (e.g. SPHERE, CYLINDER, CIRCLE)
			 * @param smooth If set to true: smooth normal generation for rounded meshes
			 */
			void loadPrimitive(Primitive type, const Vector3D & dim, int segments = 32, bool smooth = false);

			/**
			 * set from 3D vertex data
			 * the data is thightly packed position/normal/uv data depending on what components are specified
			 */
			void set3D(const float * data, size_t num_verts, unsigned char components, GLenum draw_mode);

			/**
			 * set from indexed 3D vertex data (uses element buffer)
			 * @param vertex_data is the same as param data in set3D()
			 */
			void set3DIndexed(const float * vertex_data, size_t num_verts, const unsigned int * index_data, size_t num_indicies, unsigned char components, GLenum draw_mode);

			/**
			 * Free gl buffers
			 */
			void clear();

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

			GLsizei getVertexCount(){return _vertexCount;}
			GLsizei getElementCount(){return _elementCount;}
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
