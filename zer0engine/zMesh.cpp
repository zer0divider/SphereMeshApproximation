/* Author: Cornelius Marx
 */
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

bool Mesh::loadOBJFromFile(const char * file, unsigned char components)
{
	// loading whole file
	std::ifstream f;
	f.open(file);
	if(!f.good()){
		ERROR("Unable to open file '%s'.", file);
		return false;
	}
	f.seekg(0, f.end);
	size_t length = f.tellg();
	f.seekg(0, f.beg);
	if(!f.good()){
		ERROR("While reading file '%s'.", file);
		return false;
	}

	char * buffer = new char[length+1];
	f.read(buffer, length);
	f.close();
	buffer[length] = '\0';
	bool r = loadOBJ(buffer, components);
	delete[] buffer;

	return r;
}

bool Mesh::loadOBJ(const char * obj, unsigned char components)
{
	free();
	#define SKIP_WHITESPACE(L, I) while(L[I] == ' ' || L[I] == '\t'){I++;}
	#define SKIP_NUMBER(L, I) while(L[I] >= '0' && L[I] <= '9'){I++;}
	#define OBJ_ERROR(X, ...) ERROR(("Object file line %d: " X), line_number, ##__VA_ARGS__)

	// line buffer
	static const int LINE_BUFFER_SIZE = 256;
	static char line[LINE_BUFFER_SIZE];
	static char face_strings[4][LINE_BUFFER_SIZE];
	std::vector<Vector3D> verts;
	std::vector<Vector3D> normals;
	std::vector<Vector2D> uvs;
	
	// combined verts/normals
	std::vector<Vector3D> comb_verts;
	std::vector<Vector3D> comb_normals;
	std::vector<Vector2D> comb_uvs;
	std::vector<unsigned int> element_indicies;
	typedef std::unordered_map<std::string, unsigned int>::value_type face_verts_value_t;
	std::unordered_map<std::string, unsigned int> face_verts;
	bool object_found = false;
	Vector2D v2;
	Vector3D v3;
	int line_number = 1;
	bool error = false;
	int num_exp_components = 0;
	int read_components = 0;
	bool first_face = true;
	while(*obj != '\0'){
		// get next line
		int count = 0;
		while(*obj != '\n' && *obj != '\r' && *obj != '\0'){
			if(count < LINE_BUFFER_SIZE-1){
				line[count++] = *obj;
			}
			obj++;
		}
		while(*obj == '\n' || *obj == '\r'){obj++;}
		line[count] = '\0';

		// go to first symbol not whitespace
		int i = 0;
		SKIP_WHITESPACE(line, i);
		// only start parsing if object (o) already found
		if(object_found){
			if(line[i] == 'v'){
				int j = i+1;
				error = false;
				char m = line[j];
				j++;
				SKIP_WHITESPACE(line, j);

				// parse coordinate
				switch(m){
				case 't':{// uv coordinate
					num_exp_components = 2;
					if((read_components = sscanf(&line[j], "%f %f", &v2.x, &v2.y)) != 2){
						error = true;
					}
					else{
						uvs.push_back(v2);
					}
				}break;
				case 'n':{// normal
					num_exp_components = 3;
					if((read_components = sscanf(&line[j], "%f %f %f", &v3.x, &v3.y, &v3.z)) != 3){
						error = true;
					}
					else{
						normals.push_back(v3);
					}
				}break;
				default:{// assume vertex
					num_exp_components = 3;
					if((read_components = sscanf(&line[j], "%f %f %f", &v3.x, &v3.y, &v3.z)) != 3){
						error = true;
					}
					else{
						verts.push_back(v3);
					}
				}
				}
				if(error){
					OBJ_ERROR("Expected %d coordinate components, but only got %d.",
							num_exp_components, read_components);
					break;
				}
			}
			else if(line[i] == 'f'){// face
				int j = i+1;
				SKIP_WHITESPACE(line, j);
				if((read_components = sscanf(&line[j], "%s %s %s %s", face_strings[0], face_strings[1], face_strings[2], face_strings[3])) != 3){
					OBJ_ERROR("Expected %d index groups for face, but only got %d.", 3, read_components);
					error = true;
					break;
				}
				else{
					for(int fi = 0; fi < 3; fi++){
						int k = 0;
						GLubyte flags = 0;
						int v_index = 0, n_index = 0, u_index = 0;
						v_index = atoi(&face_strings[fi][k]);
						SKIP_NUMBER(face_strings[fi], k);
						if(face_strings[fi][k] == '/'){
							k++;
							if(face_strings[fi][k] == '/'){
								k++;
								n_index = atoi(&face_strings[fi][k]);
								flags |= NORMAL;
								SKIP_NUMBER(face_strings[fi], k);
							}
							else{
								u_index = atoi(&face_strings[fi][k]);	
								flags |= UV;
								SKIP_NUMBER(face_strings[fi], k);
								if(face_strings[fi][k] == '/'){
									k++;
									n_index = atoi(&face_strings[fi][k]);
									flags |= NORMAL;
									SKIP_NUMBER(face_strings[fi], k);
								}
							}
						}
						if(first_face){// check what this face consists of
							first_face = false;
							_flags = flags;
						}
						else if(_flags != flags){
							OBJ_ERROR("Unexpected change in provided indicies.");
							error = true;
							break;
						}
						// adjust by one (obj indicies start from 1)
						v_index -=1;	
						n_index -=1;
						u_index -=1;	
						// try inserting
						auto ret = face_verts.insert(face_verts_value_t(std::string(face_strings[fi]), comb_verts.size()));
						if(ret.second){// new element
							if(v_index < 0 || v_index >= verts.size()){
								OBJ_ERROR("Vertex index %d out of bounds.", v_index+1);
								error = true;
								break;
							}
							else if((_flags & NORMAL) && (n_index < 0 || n_index >= normals.size())){
								OBJ_ERROR("Normal index %d out of bounds.", n_index+1);
								error = true;
								break;
							}
							else if((_flags & UV) && (u_index < 0 || u_index >= uvs.size())){
								OBJ_ERROR("UV index %d out of bounds.", u_index+1);
								error = true;
								break;
							}
							else{
								element_indicies.push_back(comb_verts.size());
								comb_verts.push_back(verts[v_index]);
								if(_flags & NORMAL){
									comb_normals.push_back(normals[n_index]);
								}
								if(_flags & UV){
									comb_uvs.push_back(uvs[u_index]);
								}
							}
						}
						else{// already there
							element_indicies.push_back(ret.first->second);
						}
					}
					if(error){
						break;
					}
				}
			}
		}

		if(line[i] == 'o'){
			if(object_found){
				WARNING("Multiple object definitions found (o) in obj file, ignoring other.");
				break;
			}
			object_found = true;
		}
		// ignore all other stuff
		line_number++;
	}

	if(!error){
		_flags &= components;
		// set element buffer
		_elementCount = element_indicies.size();
		_elementType = GL_UNSIGNED_INT;
		glGenBuffers(1, &_elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*_elementCount, element_indicies.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glGenBuffers(1, &_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, _buffer);
		int single_vertex_size = 3;
		if(_flags & NORMAL){
			single_vertex_size += 3;
		}
		if(_flags & UV){
			single_vertex_size += 2;
		}
		_vertexCount = comb_verts.size();
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*single_vertex_size*_vertexCount, 0, GL_STATIC_DRAW);
		int offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*_vertexCount, comb_verts.data());
		offset += 3*_vertexCount;
		if(_flags & NORMAL){
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*offset, sizeof(float)*3*_vertexCount, comb_normals.data());
			assert(comb_normals.size() == _vertexCount);
			offset += 3*_vertexCount;
		}
		if(_flags & UV){
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*offset, sizeof(float)*2*_vertexCount, comb_uvs.data());
			assert(comb_uvs.size() == _vertexCount);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		_drawMode = GL_TRIANGLES;
		_numDimensions = 3;
	}

	return !error;
}
