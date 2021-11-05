#include "zShader.h"

using namespace zer0;

Shader* Shader::_current = NULL;
Shader Shader::DefaultShader("Texture-Shader");
Shader Shader::TextShader("Text-Shader");

GLint Shader::getLocation(const char * name, bool is_uniform)
{
	GLint loc;
	if(is_uniform){// is this a uniform name
		loc = glGetUniformLocation(_program, name);
		if(loc == -1){
			WARNING("Shader '%s': Uniform attribute '%s' not found!", _name.c_str(), name);
		}
	}else{// name is a vertex attribute
		loc = glGetAttribLocation(_program, name);
		if(loc == -1){
			WARNING("Shader '%s': Vertex attribute '%s' not found!", _name.c_str(), name);
		}

	}
	return loc;
}

bool Shader::initDefaultShaders()
{
	bool ret = true;

	// default shader
	static const char* VERTEX_SHADER_SOURCE =
		"#version 120\n"
		"attribute vec3 vertex;\n"
		"attribute vec2 uv;\n"
		"attribute vec4 color;\n"
		"varying vec4 out_color;\n"
		"uniform mat4 projMat;\n"
		"uniform mat4 viewMat;\n"
		"uniform mat4 modelMat;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = projMat * viewMat * modelMat * vec4(vertex, 1);\n"
		"	out_color = color;\n"
		"}";

	static const char* FRAGMENT_SHADER_SOURCE =
		"#version 120\n"
		"varying vec4 out_color;\n"
		"uniform sampler2D tex;\n"
		"void main()\n"
		"{\n"
		"	vec4 tex_color = out_color;\n"
		"	gl_FragColor = tex_color;\n"
		"}\n";

	if(DefaultShader.load(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE)){
		DefaultShader.setVertexLocation("vertex");
		DefaultShader.setColorLocation("color");
		DefaultShader.setModelMatrixLocation("modelMat");
		DefaultShader.setViewMatrixLocation("viewMat");
		DefaultShader.setProjectionMatrixLocation("projMat");
		DefaultShader.use();
		DefaultShader.setColor(zer0::Color::WHITE);
	}else{
		ret = false;
	}

	return ret;
}

bool Shader::load(	const char * vertex_shader_source,
					const char * fragment_shader_source)
{
	if(vertex_shader_source == NULL || fragment_shader_source == NULL){
		ERROR("Shader '%s': Vertex and Fragment shader are crucial for shader program, but where not given!", _name.c_str());
		return false;
	}

	GLint success = 0;
	int error = 0;
	//create and compile vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, 0);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE){
		ERROR("Shader '%s': Error during vertex compilation", _name.c_str());
		error++;	
	}

	//create and compile fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, 0);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE){
		ERROR("Shader '%s': Error during fragment compilation", _name.c_str());
		error++;	
	}
	//create and link program with vs and fs
	_program = glCreateProgram();
	glAttachShader(_program, vertex_shader);
	glAttachShader(_program, fragment_shader);
	glLinkProgram(_program);
	glGetShaderiv(_program, GL_LINK_STATUS, &success);
	if(success == GL_FALSE){
		ERROR("Shader '%s': Error during program linking", _name.c_str());
		error++;	
	}

	//Error-handling:
	static const int bufSize = 1024;
	GLchar *buffer = new GLchar [bufSize];

	//Vertex Shader Error-Log
	glGetShaderInfoLog(vertex_shader, bufSize, 0, buffer);
	buffer[bufSize-1] = '\0';
	if(buffer[0] != '\0')//non-empty
	{
		ERROR("Shader '%s': While compiling vertex shader:\n%s", _name.c_str(), (const char*) buffer);
	}
	//Fragment Shader Error-Log
	glGetShaderInfoLog(fragment_shader, bufSize, 0, buffer);
	buffer[bufSize-1] = '\0';
	if(buffer[0] != '\0')//non-empty
	{
		ERROR("Shader '%s': While compiling fragment shader:\n%s", _name.c_str(), (const char*) buffer);
	}

	//Program Error-Log
	glGetProgramInfoLog(_program, bufSize, 0, buffer);
	buffer[bufSize-1] = '\0';
	if(buffer[0] != '\0')//non-empty
	{
		ERROR("Shader '%s': While linking shader program:\n%s", _name.c_str(), (const char*)buffer);
	}
	delete[] buffer;

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	if(error > 0){
		glDeleteProgram(_program);
		_program = 0;
		return false;
	}

	return true;

}
bool Shader::loadFromFile(	const char * vertex,
							const char * fragment)
{
	static const int NUM_FILES = 2;
	char* sources[NUM_FILES] = {NULL, NULL};
	const char* files[NUM_FILES] = {vertex,
							fragment};

	FILE * f;
	// open each file one after another and reading whole content into sources
	for(int i = 0; i < NUM_FILES; i++){
		if(files[i] == NULL)continue;
		f = fopen(files[i], "r");
		if(f == NULL) {
			ERROR("Error while loading shader from file: Could not open '%s'!", files[i]);
			return false;
		}
		// getting file size
		fseek(f, 0, SEEK_END);
		long int file_size = ftell(f);
		sources[i] = new char[file_size+1];
		fseek(f, 0, SEEK_SET);
		fread(sources[i], 1, file_size, f);
		sources[i][file_size] = '\0';
		fclose(f);
	}
	
	// compile from sources
	bool ret_value = load(sources[0], sources[1]);
	for(int i = 0; i < NUM_FILES; i++)
		delete[] sources[i];
	
	return ret_value;
}
