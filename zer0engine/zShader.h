#ifndef ZER0_SHADER_H
#define ZER0_SHADER_H

#include "glew/glew.h"
#include "zLogger.h"
#include "zColor.h"
#include "zVector4D.h"
#include "zVector3D.h"
#include "zVector2D.h"
#include "zMatrix4.h"
#include <string>
#include "zConfig.h"

/* macro for getting current shader */
#define SHADER Shader::getCurrent()
namespace zer0{

	/* forward declaration for friendship */
	class Framework;

	/**
	 * Generic shader program
	 */
	class Shader{
	public:

		/**
		 * Constructor
		 */
		Shader(const char * name = "default"): _name(name), _program(0), _vertexLocation(-1), _uvLocation(-1), _normalLocation(-1), _colorLocation(-1),
				_samplerLocation(-1), _viewMatrixLocation(-1), _projectionMatrixLocation(-1), _modelMatrixLocation(-1){}

		/**
		 * Destructor
		 */
		virtual ~Shader(){
			if(_program != 0){
				glDeleteProgram(_program);
			}
		}

		/**
		 * compile shader program from source of different shader stages
		 * @param vertex Source code for vertex shader stage
		 * @param fragment Source code for fragment shader stage
		 * @return True on success, false on failure
		 */
		bool load(	const char * vertex,
					const char * fragment);

		/**
		 * Load and compile shader program from file.
		 * @param vertex_file Filename of vertex shader
		 * @param fragment_file Filename of fragment shader
		 * @return True on success, false on failure
		 */
		bool loadFromFile(	const char * vertex_file,
							const char * fragment_file);

		/**
		 * Retrieve locations of specific vertex attribute and uniforms
		 * @param name Name of the vertex attribute/uniform as defined in shader
		 */
		void setVertexLocation(const char * name)
		{_vertexLocation = getLocation(name, false);}
		void setUVLocation(const char * name)
		{_uvLocation = getLocation(name, false);}
		void setColorLocation(const char * name)
		{_colorLocation = getLocation(name, false);}
		void setNormalLocation(const char * name)
		{_normalLocation = getLocation(name, false);}
		void setProjectionMatrixLocation(const char * name)
		{_projectionMatrixLocation = getLocation(name, true);}
		void setModelMatrixLocation(const char * name)
		{_modelMatrixLocation = getLocation(name, true);}
		void setViewMatrixLocation(const char * name)
		{_viewMatrixLocation = getLocation(name, true);}
		void setSamplerLocation(const char * name)
		{_samplerLocation = getLocation(name, true);}

		/**
		 * Get location of vertex attribute or uniform
		 * @param name Name of the vertex attribute/uniform as defined in shader
		 * @param is_uniform Set to true if name refers to a uniform and to false if name refers to a vertex attribute
		 * @return The location of the vertex attribute/uniform in the shader or -1 if name could not have been found
		 */
		GLint getLocation(const char * name, bool is_uniform);

		/**
		 * Set matrix in shader.
		 * @param m The matrix to upload to shader program
		 */
		void setProjectionMatrix(const Matrix4 & m){glUniformMatrix4fv(_projectionMatrixLocation, 1, GL_FALSE, (GLfloat*)&m);}
		void setViewMatrix(const Matrix4 & m){glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, (GLfloat*)&m);}
		void setModelMatrix(const Matrix4 & m){glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, (GLfloat*)&m);}

		/*
		void setProjectionMatrix(const Matrix3 & m){glUniformMatrix3fv(_projectionMatrixLocation, 1, GL_FALSE, (GLfloat*)&m);}
		void setViewMatrix(const Matrix3 & m){glUniformMatrix3fv(_viewMatrixLocation, 1, GL_FALSE, (GLfloat*)&m);}
		void setModelMatrix(const Matrix3 & m){glUniformMatrix3fv(_modelMatrixLocation, 1, GL_FALSE, (GLfloat*)&m);}
		*/

		/**
		 * Set color vertex attribute in shader.
		 * @param c The color to set the color vertex attribute to.
		 */
		void setColor(const Color & c){glDisableVertexAttribArray(_colorLocation); glVertexAttrib4fv(_colorLocation, (const float*)c);}

		/**
		 * Set color vertex attribute in shader to pointer. This enables vertex array on color location. A buffer must be bound to the target GL_ARRAY_BUFFER.
		 * Type is set to GL_FLOAT. Normalization is disabled.
		 * @param stride Bytes offset between consecutive vertex attributes. if set to 0, vertex attributes are understood to be tightly packed in the array.
		 * @param offset Byte offset of the first component of the first vertex attribute in the array.
		 */
		void setColorPointer(GLint size = 4, GLsizei stride = 0, size_t offset = 0){
			glEnableVertexAttribArray(_colorLocation);
			glVertexAttribPointer(_colorLocation, size, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
		}

		/**
		 * Set normal vertex attribute in shader.
		 * @param normal The normal to set the normal vertex attribute to
		 */
		void setNormal(const Vector3D & normal){
			glDisableVertexAttribArray(_normalLocation);
			glVertexAttrib4fv(_normalLocation, (const float*)normal);}

		/**
		 * Set normal vertex attribute in shader to pointer. This enables vertex array on normal location. A buffer must be bound to the target GL_ARRAY_BUFFER.
		 * Type is set to GL_FLOAT. Normalization is disabled. Size is set to 3.
		 * @param stride Bytes offset between consecutive vertex attributes. if set to 0, vertex attributes are understood to be tightly packed in the array.
		 * @param offset Byte offset of the first component of the first vertex attribute in the array.
		 */
		void setNormalPointer(GLsizei stride = 0, size_t offset = 0){
			glEnableVertexAttribArray(_normalLocation);
			glVertexAttribPointer(_normalLocation, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offset);}

		/**
		 * Set position vertex attribute in shader to pointer. This enables vertex array on vertex location. A buffer must be bound to the target GL_ARRAY_BUFFER.
		 * Type is set to GL_FLOAT. Normalization is disabled.
		 * @param size Number of components per generic vertex attribute. Must be 1, 2, 3, 4.
		 * @param stride Bytes offset between consecutive vertex attributes. if set to 0, vertex attributes are understood to be tightly packed in the array.
		 * @param offset Byte offset of the first component of the first vertex attribute in the array.
		 */
		void setVertexPointer(GLint size = 3, GLsizei stride = 0, size_t offset = 0){
			glEnableVertexAttribArray(_vertexLocation);
			glVertexAttribPointer(_vertexLocation, size, GL_FLOAT, GL_FALSE, stride, (const void*)offset);}

		/**
		 * Set uv vertex attribute in shader.
		 * @param uv The uv coordinate to set the uv vertex attribute to
		 */
		void setUV(const Vector2D & uv){
			glDisableVertexAttribArray(_uvLocation);
			glVertexAttrib2fv(_uvLocation, (const float*)uv);}

		/**
		 * Set uv vertex attribute in shader to pointer. This enables vertex array on uv location. A buffer must be bound to the target GL_ARRAY_BUFFER.
		 * Type is set to GL_FLOAT. Normalization is disabled. Size is set to 2.
		 * @param stride Bytes offset between consecutive vertex attributes. If set to 0, vertex attributes are understood to be tightly packed in the array.
		 * @param offset Byte offset of the first component of the first vertex attribute in the array.
		 */
		void setUVPointer(GLsizei stride = 0, size_t offset = 0){
			glEnableVertexAttribArray(_uvLocation);
			glVertexAttribPointer(_uvLocation, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offset);}

		/**
		 * Disable vertex array of a specific vertex attribute.
		 */
		void disableUVArray(){glDisableVertexAttribArray(_uvLocation);}
		void disableVertexArray(){glDisableVertexAttribArray(_vertexLocation);}
		void disableNormalArray(){glDisableVertexAttribArray(_normalLocation);}
		void disableColorArray(){glDisableVertexAttribArray(_colorLocation);}

		
		/**
		 * Disable all vertex arrays.
		 */
		void disableArrays(){disableUVArray(); disableVertexArray(); disableNormalArray(); disableColorArray();}

		/**
		 * Set this program as current. Calls glUseProgram() with this program's id and sets the global reference.
		 * 
		 */
		void use(){glUseProgram(_program); _current = this;}

		/**
		 * Get currently used Shader object. That is the last shader use() was called upon.
		 */
		static Shader* getCurrent(){assert(_current != NULL); return  _current;}
	
		/**
		 * getter methods for vertex attribute/uniform locations
		 */
		GLint getVertexLocation()			{return _vertexLocation;}
		GLint getUVLocation()				{return _uvLocation;}
		GLint getNormalLocation()			{return _normalLocation;}
		GLint getColorLocation()			{return _colorLocation;}
		GLint getSamplerLocation()			{return _samplerLocation;}
		GLint getViewMatrixLocation()		{return _viewMatrixLocation;}
		GLint getProjectionMatrixLocation()	{return _projectionMatrixLocation;}
		GLint getModelMatrixLocation()		{return _modelMatrixLocation;}

		/* default shaders globally accessible */
		static Shader DefaultShader;
		static Shader TextShader;

		friend class Framework;
	private:
		/* initialize default shaders
		 * NOTE: this function is called by Framework on startup
		 */
		static bool initDefaultShaders();
		static Shader * _current;

		std::string _name;
		GLuint _program;
		GLint _vertexLocation;
		GLint _uvLocation;
		GLint _normalLocation;
		GLint _colorLocation;
		GLint _samplerLocation; //location for 2D texture sampler
		GLint _viewMatrixLocation;
		GLint _projectionMatrixLocation;
		GLint _modelMatrixLocation;
	};
};
#endif
