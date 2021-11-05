/*
Created on: 16th March 2018
Author: Cornelius Marx

Description: Font
	- class for loading and handling true-type fonts
	- it is possible to handle up to 7 different text sizes in one Font-Object
*/
#ifndef Z_FONT_H
#define Z_FONT_H

#include "zGlyphMap.h"
#include "zLogger.h"

namespace zer0{

	/* forward declaration for friendship */
	class Framework;

	class Font
	{
	public:
		/* abstract font sizes from small to big */
		enum FontSize {VERY_SMALL, SMALL, NORMAL, BIG, VERY_BIG, NUM_SIZE_SLOTS};
		/*
	 	 * default pt-sizes mapped to abstract font sizes (e.g. Z_TINY-> 8.f)
	 	 */
		static const float DEFAULT_FONT_SIZES[NUM_SIZE_SLOTS];
		///constructors
		Font();

		///destructor
		~Font();

		//free all data allocated for this font (called on destructor or when a font is reloaded from file)
		void free();

		//load font from file
		//@return false on error else true 
		bool loadFromFile(const char * path);

		//loading font from already loaded font face
		//@param free tells whether the face source should be freed by this object
		void loadFromFace(FT_Face face, bool freeFace);

		//render bit map to a size-slot at given size
		//@param pointSize : if false -> @param size is considered to be a pixel-size
		void renderMap(FontSize slot, float size, bool pointSize = false);

		//renders all slots using default point sizes
		void renderDefaultSizes();

		///getter
		//get glyph map for given font size (e.g. Z_SMALL)
		//@return if size slot is not filled NULL is returned
		GlyphMap * getGlyphMap(FontSize size);

		// get freetype library font
		FT_Face getFace(){return _face;}

		// get default font
		static Font* getDefault(bool bold = false){return bold ? _monospaceBold : _monospaceRegular;}

		friend Framework;
	private:
		
		// initialize default fonts (called initially by framework)
		static bool initDefaultFonts(const char * path_regular, const char * path_bold);
		static void freeDefaultFonts();

		bool _freeFace;
		GlyphMap * _glyphs[NUM_SIZE_SLOTS];
		FT_Face _face;

		static Font * _monospaceRegular;
		static Font * _monospaceBold;
		//static global ft-library
		static FT_Library _FT_LIB;

	};

};


#endif
