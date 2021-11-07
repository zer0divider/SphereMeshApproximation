#ifndef ZER0_TEXTURE_H
#define ZER0_TEXTURE_H

#include <SDL2/SDL.h>
#include <cassert>
#include "zLogger.h"
#include "glew/glew.h"
#include "zColor.h"

namespace zer0{

	/* forward declaration for friendship */
	class Framework;

	/**
	 *  Texture class, representing 2D texture
	 */
	class Texture2D{
	public:
		/**
		 * Constructor
		 */
		Texture2D(): _id(0), _width(0), _height(0){}

		/**
		 * Destructor
		 */
		~Texture2D(){free();}

		/**
		 * Creates texture from RGBA pixel data.
		 * @param pixels Pixel data to store in texture
		 * @param w Width of the texture
		 * @param h Height of the texture
		 * Note: Pixels array must provide at least w*h Color values.
		 * Texture will be left bound to target GL_TEXTURE_2D when function returns
		 */
		void loadFromColor(const Color * pixels, int w, int h, GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE);
		void loadFromUint(const GLuint * pixels, int w, int h, GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE);
		void load(const void * pixels, int w, int h, GLenum pixel_format, GLenum type, GLint internal_format, GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE);

		/**
		 * Load texture from file (only bmp).
		 * @param file Filename of the image to load
		 * @return True on success, false on failure
		 */
		bool loadFromFile(const char * file, GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE);

		/** 
		 * Free texture
		 */
		void free(){glDeleteTextures(1, &_id); _id = 0; _width = 0; _height = 0;}

		/**
		 * Bind this texture to 2D texture target
		 */
		void bind()const{
			glBindTexture(GL_TEXTURE_2D, _id);
		}

		/**
		 * Bind blank texture to 2D texture target
		 */
		static void bindBlank(){
			assert(_blankTex != NULL);
			_blankTex->bind();
		}

		/**
		 * Generate mipmap levels
		 */
		void generateMipMap(){
			bind();
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		/**
		 * Set texture min and mag filter.
		 * @param min_filter Parameter for GL_TEXTURE_MIN_FILTER
		 * @param mag_filter Parameter for GL_TEXTURE_MAG_FILTER
		 */
		void setFilter(GLint min_filter, GLint mag_filter){
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
		}

		/**
		 * Set texture wrap
		 * @param wrap_mode Wrap mode for S and T coordinate
		 */
		void setWrap(GLint wrap_mode){
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
		}

		/**
		 * Set texture min, mag filter and wrap mode
		 * @param min_filter Parameter for GL_TEXTURE_MIN_FILTER
		 * @param mag_filter Parameter for GL_TEXTURE_MAG_FILTER
		 * @param wrap_mode Wrap mode for S and T coordinate
		 */
		void setParams(GLint min_filter, GLint mag_filter, GLint wrap_mode){
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
		}

		/**
		 * Bind error texture
		 */
		static void bindError(){
			assert(_errorTex != NULL);
			_errorTex->bind();
		}

		/**
		 * Allow Framework class to create and destroy blank textures
		 */
		friend class Framework;
	private:
		/**
		 * Create and initialize blank texture
		 */
		static void initDefaultTextures();

		/**
		 * Free default textures
		 */
		static void freeDefaultTextures();
		
		/**
		 * Default textures
		 */
		static Texture2D * _blankTex;
		static Texture2D * _errorTex;

		int _width;
		int _height;
		GLuint _id;
	};
};

#endif
