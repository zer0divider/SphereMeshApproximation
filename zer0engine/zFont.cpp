#include "zFont.h"

using namespace zer0;

// static
FT_Library Font::_FT_LIB = 0;
Font * Font::_monospaceRegular = 0;
Font * Font::_monospaceBold = 0;

const float Font::DEFAULT_FONT_SIZES[NUM_SIZE_SLOTS] = {16.f, 24.f, 36.f, 42.f, 64.f};

bool Font::initDefaultFonts(const char * path_regular, const char * path_bold)
{
	_monospaceRegular = new Font();
	if(!_monospaceRegular->loadFromFile(path_regular)){
		return false;
	}
	_monospaceRegular->renderDefaultSizes();

	_monospaceBold = new Font();
	if(!_monospaceBold->loadFromFile(path_bold)){
		return false;
	}
	_monospaceBold->renderDefaultSizes();

	return true;
}

void Font::freeDefaultFonts()
{
	delete(_monospaceRegular);
	delete(_monospaceBold);
}

Font::Font()
{
	//library uninitialized?
	if(_FT_LIB == NULL)
	{
		FT_Error error = FT_Init_FreeType(&_FT_LIB);
		if(error)
		{
			ERROR("Could not initialize FreeType!");
		}
	}
	//setting values
	_freeFace = false;
	_face = NULL;
	memset(_glyphs, 0, sizeof(_glyphs));
}

Font::~Font()
{
	free();
}

void Font::free()
{
	if(_face != NULL)
	{
		//free glyphs
		for(int i = 0; i < NUM_SIZE_SLOTS; i++)
		{
			if(_glyphs[i] != NULL)
			{
				delete(_glyphs[i]);
				_glyphs[i] = NULL;
			}
		}

		//free face
		if(_freeFace)
			FT_Done_Face(_face);
		_face = NULL;
	}
}

bool Font::loadFromFile(const char * path)
{
	FT_Face f;
	FT_Error error = FT_New_Face(_FT_LIB, path, 0, &f);
	if(error)
	{
		ERROR("Could not load font '%s'", path);
		return false;
	}
	loadFromFace(f, true);
	return true;
}

void Font::loadFromFace(FT_Face face, bool freeFace)
{
	//freeing old font
	free();
	_face = face;
	_freeFace = freeFace;
}

void Font::renderMap(FontSize slot, float size, bool pointSize)
{
	if(_glyphs[slot] == NULL)//glyph has not been created yet
		_glyphs[slot] = new GlyphMap();
	//creating new glyph map
	if(pointSize)
	{
		_glyphs[slot]->renderMap(_face, size, 0/*dpi*/);
	}
	else//pixel size
	{
		_glyphs[slot]->renderMap(_face, static_cast<int>(size));
	}
}

void Font::renderDefaultSizes()
{
	//rendering all slots
	for(int i = 0; i < NUM_SIZE_SLOTS; i++)
	{
		renderMap(static_cast<FontSize>(i), DEFAULT_FONT_SIZES[i], true);
	}
}

GlyphMap * Font::getGlyphMap(FontSize size)
{
	//does glyph slot exist?
	if(_glyphs[size] == NULL)
	{
        WARNING("Font size %d has not been rendered, setting to default size", size);
		renderMap(size, DEFAULT_FONT_SIZES[size], true);
	}

	return _glyphs[size];

}
