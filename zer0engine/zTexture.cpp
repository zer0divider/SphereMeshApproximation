#include "zTexture.h"
using namespace zer0;

Texture2D * Texture2D::_blankTex = NULL;
Texture2D * Texture2D::_errorTex = NULL;

void Texture2D::loadFromColor(const Color * pixels, int w, int h, GLint filter, GLint wrap)
{
	load((const void*)pixels, w, h, GL_RGBA, GL_FLOAT, GL_RGBA, filter, wrap);
}

void Texture2D::loadFromUint(const GLuint * pixels, int w, int h, GLint filter, GLint wrap)
{
	load((const void*)pixels, w, h, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, filter, wrap);
}

void Texture2D::load(const void * pixels, int w, int h, GLenum pixel_format, GLenum type, GLint internal_format, GLint filter, GLint wrap)
{
	free();
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, pixel_format, type, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	_width = w;
	_height = h;
}

bool Texture2D::loadFromFile(const char * file, GLint filter, GLint wrap)
{
	SDL_Surface * s = SDL_LoadBMP(file);
	if(s == NULL){
		ERROR("While loading texture from file: %s", SDL_GetError());
		return false;
	}
	bool success = true;
	/*
	INFO("'%s':", file);
	INFO("-> Resolution:      %dx%d", s->w, s->h);
	INFO("-> Bytes per pixel: %d", s->format->BytesPerPixel);
	INFO("-> Red Mask:        0x%.8x", s->format->Rmask);
	INFO("-> Green Mask:      0x%.8x", s->format->Gmask);
	INFO("-> Blue Mask:       0x%.8x", s->format->Bmask);
	INFO("-> Alpha Mask:      0x%.8x", s->format->Amask);
	*/
	switch(s->format->BytesPerPixel){
	case 1:{
		WARNING("Single component textures may not be supported!");
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		load(s->pixels, s->w, s->h, GL_RED, GL_UNSIGNED_BYTE, GL_RED, filter, wrap);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}break;
	case 2:{
		WARNING("Dual component textures may not be supported!");
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		load(s->pixels, s->w, s->h, GL_RG, GL_UNSIGNED_BYTE, GL_RG, filter, wrap);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}break;
	case 3:{
		GLenum internal_format = GL_RGB;
		if(s->format->Rmask == 0x00FF0000){// flip around byte order
			INFO("-> flipping byte order");
			internal_format = GL_BGR;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
		load(s->pixels, s->w, s->h, internal_format, GL_UNSIGNED_BYTE, GL_RGB, filter, wrap);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}break;
	case 4:{
		GLenum internal_format = GL_RGBA;
		if(s->format->Rmask == 0x00FF0000){// flip around byte order
			INFO("-> flipping byte order");
			internal_format = GL_BGRA;
		}
		load(s->pixels, s->w, s->h, internal_format, GL_UNSIGNED_BYTE, GL_RGBA, filter, wrap);
	}break;
	default:{
		ERROR("'%s' has invalid amount of Bytes per pixel: %d", file, s->format->BytesPerPixel);
		success = false;
	}
	}

	SDL_FreeSurface(s);
	return success;
}

void Texture2D::initDefaultTextures(){
	_blankTex = new Texture2D();
	GLuint white = 0xFFFFFFFF;
	_blankTex->loadFromUint(&white, 1, 1, GL_NEAREST, GL_REPEAT);

	_errorTex = new Texture2D();
	GLuint error_pixels[265];
	for(int i = 0; i < 256; i++){
		if(i%2 != (i/16)%2){
			error_pixels[i] = 0xFFFF00FF;
		}
		else{
			error_pixels[i] = 0xFF000000;
		}
	}
	_errorTex->loadFromUint(error_pixels, 16, 16, GL_NEAREST, GL_REPEAT);
}

void Texture2D::freeDefaultTextures()
{
	delete _blankTex;
	_blankTex = NULL;

	delete _errorTex;
	_errorTex = NULL;
}
