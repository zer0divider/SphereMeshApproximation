#include "zMesh.h"

using namespace zer0;

void Mesh::loadPrimitive(Primitive p, const Vector3D & dim, int segments, bool smooth)
{
	free();
	float * verticies = NULL;
	GLsizeiptr total_vertex_size = 0;
	_vertexCount = 0;
	GLubyte * indicies = NULL;
	GLsizeiptr total_index_size = 0;
	_elementType = GL_UNSIGNED_BYTE;
	_elementCount = 0;
	_numDimensions = 3;
	_drawMode = GL_TRIANGLES;
	Vector3D half_dim = dim/2;
	switch(p){
		case BOX:{
			_vertexCount = 24;
			_elementCount = 36;
			_elementType = GL_UNSIGNED_BYTE;
			GLsizei value_count = 24*3;
			GLsizei normal_index_offset = -1;
			GLsizei uv_index_offset = -1;
			_flags = NORMAL | UV;
			normal_index_offset = value_count;
			value_count += 24*3;
			uv_index_offset = value_count;
			value_count += 24*2;
			total_vertex_size = sizeof(float)*value_count;
			verticies = new float[value_count];
			Vector3D* vertex = (Vector3D*)(&verticies[0]);
			for(int i =0; i < 24; i++){
				vertex[i] = CUBE_VERTS[i]*half_dim;
			}
			Vector3D*normals = (Vector3D*)(&verticies[normal_index_offset]);
			for(int i = 0; i < 24; i++){
				normals[i] = CUBE_NORMALS[i/4];
			}

			Vector2D* uv = (Vector2D*)(&verticies[uv_index_offset]);
			for(int i = 0; i < 6; i++){
				uv[i*4+0].set(1, 0);
				uv[i*4+1].set(0, 0);
				uv[i*4+2].set(0, 1);
				uv[i*4+3].set(1, 1);
			}
			indicies = new GLubyte[36];
			total_index_size = sizeof(GLubyte)*36;
			for(int i = 0; i < 6; i++){
				indicies[i*6+0] = 4*i + 0;
				indicies[i*6+1] = 4*i + 1;
				indicies[i*6+2] = 4*i + 2;
				indicies[i*6+3] = 4*i + 2;
				indicies[i*6+4] = 4*i + 3;
				indicies[i*6+5] = 4*i + 0;
			}
		}break;
		case SPHERE:{
		}break;
		case CYLINDER:{
		}break;
		case CONE:{
		}break;
		case TORUS:{
		}break;
		case CIRCLE:{
			_vertexCount = segments+2;
			_drawMode = GL_TRIANGLE_FAN;
			_elementCount = 0;
			total_vertex_size = sizeof(float)*_vertexCount*4;
			verticies = new float[_vertexCount*4];
			_flags = UV;
			verticies[0] = 0;
			verticies[1] = 0;
			verticies[_vertexCount*2+0] = 0.5;
			verticies[_vertexCount*2+1] = 0.5;
			generateCircleData(dim.x/2.0f, (Vector2D*)&verticies[2], segments);
			verticies[_vertexCount*2-2] = verticies[2];
			verticies[_vertexCount*2-1] = verticies[3];
			generateCircleData(0.5, (Vector2D*)&(verticies[_vertexCount*2+2]), segments, Vector2D(0.5, 0.5));
			verticies[2*_vertexCount*2-2] = verticies[2*_vertexCount+2];
			verticies[2*_vertexCount*2-1] = verticies[2*_vertexCount+3];
			_numDimensions = 2;
		}break;
		case CIRCLE_LINE:{
			_numDimensions = 2;
		}break;
		case QUAD:{
			_numDimensions = 2;
		}break;
		case QUAD_LINE:{
			_numDimensions = 2;
		}break;
		default:{// invalid primitive type
			return;
		}
	}
	assert(total_vertex_size > 0);

	// put verticies in buffer
	glGenBuffers(1, &_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _buffer);
	glBufferData(GL_ARRAY_BUFFER, total_vertex_size, verticies, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// put indicies in buffer
	if(_elementCount > 0){
		assert(total_index_size > 0);
		glGenBuffers(1, &_elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, total_index_size, indicies, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	delete[](verticies);
	delete[](indicies);

}

void Mesh::generateCircleData(float radius, Vector3D * v, int segments, const Vector3D & center)
{
	for(int i = 0; i < segments; i++){
		float angle = 2*M_PI*(i/static_cast<float>(segments));
		v[i].x = radius*cos(angle)+center.x;
		v[i].y = radius*sin(angle)+center.y;
		v[i].z = center.z;
	}
}

void Mesh::generateCircleData(float radius, Vector2D * v, int segments, const Vector2D & center)
{
	for(int i = 0; i < segments; i++){
		float angle = 2*M_PI*(i/static_cast<float>(segments));
		v[i].x = radius*cos(angle)+center.x;
		v[i].y = radius*sin(angle)+center.y;
	}
}

void Mesh::free()
{
	glDeleteBuffers(1, &_buffer);
	glDeleteBuffers(1, &_elementBuffer);
	_buffer=0;
	_elementBuffer=0;
}

void Mesh::bind(GLubyte flags){
	int offset = _numDimensions*_vertexCount;
	glBindBuffer(GL_ARRAY_BUFFER, _buffer);

	// set vertex pointer
	SHADER->setVertexPointer(_numDimensions);

	// set normal pointer
	if(_flags & NORMAL & flags){
		SHADER->setNormalPointer(0, offset*sizeof(float));
		offset +=  3*_vertexCount; //normals used -> adjust uv offset
	}

	// set uv pointer
	if(_flags & UV & flags){
		SHADER->setUVPointer(0, offset*sizeof(float));
		offset +=  2*_vertexCount; //normals used -> adjust uv offset
	}

	// set color pointer
	if(_flags & COLOR & flags){
		SHADER->setColorPointer(0, offset*sizeof(float));
	}

	// bind element buffer (Note: if _elementBuffer = 0 then nothing is bound)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBuffer);
}

void Mesh::draw()
{
	if(_elementBuffer == 0){
		glDrawArrays(_drawMode, 0, _vertexCount);
	}
	else{
		glDrawElements(_drawMode, _elementCount, _elementType, 0);
	}
}

/*** constants ***/
const Vector2D zer0::QUAD_VERTS[4] ={
	Vector2D(-1, -1),
	Vector2D(1, -1),
	Vector2D(1, 1),
	Vector2D(-1, 1)
};

const Vector3D zer0::CUBE_VERTS[24] ={
	/* front face */
	Vector3D(1, 1, 1),
	Vector3D(-1, 1, 1),
	Vector3D(-1, -1, 1),
	Vector3D(1, -1, 1),

	/* left face */
	Vector3D(-1, 1, 1),
	Vector3D(-1, 1, -1),
	Vector3D(-1, -1, -1),
	Vector3D(-1, -1, 1),

	/* right face */
	Vector3D(1, 1, -1),
	Vector3D(1, 1, 1),
	Vector3D(1, -1, 1),
	Vector3D(1, -1, -1),

	/* back face */
	Vector3D(-1, 1, -1),
	Vector3D(1, 1, -1),
	Vector3D(1, -1, -1),
	Vector3D(-1, -1, -1),

	/* top face */
	Vector3D(1, 1, -1),
	Vector3D(-1, 1, -1),
	Vector3D(-1, 1, 1),
	Vector3D(1, 1, 1),

	/* bottom face */
	Vector3D(-1, -1, -1),
	Vector3D(1, -1, -1),
	Vector3D(1, -1, 1),
	Vector3D(-1, -1, 1)
};

const Vector3D zer0::CUBE_NORMALS[6] = {
	Vector3D(0, 0, 1),
	Vector3D(-1, 0, 0),
	Vector3D(1, 0, 0),
	Vector3D(0, 0, -1),
	Vector3D(0, 1, 0),
	Vector3D(0, -1, 0)
};
