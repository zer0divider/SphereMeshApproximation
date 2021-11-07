/*
Created on: 17th March 2018
Author: Cornelius Marx

Description: zTextView
	-	used for displaying and aligning single lined texts using a Font
	-	the render() function expects the projection to be set up pixel-aligned with increasing x coordinates from left to right and
		increasing y coordinates from top to bottom
*/
#ifndef Z_TEXT_VIEW_H
#define Z_TEXT_VIEW_H

#include "zFont.h"
#include "zShader.h"
#include "zMatrix4.h"
#include "zColor.h"

namespace zer0
{
class TextView
{
public:
	///constructors
	TextView();
	//initializing with capacity (max. number of letters in buffer)
	TextView(Font * font, Font::FontSize size, int capacity);
	//initializing with given text
	//@setBuffer determines whether the textbuffer should be set according to the given text
	//				can be set to false if e.g. the font might change before the first render (due to an window update call)
	TextView(Font * font, Font::FontSize size, const char * text, bool setBuffer = true);
	void init(Font * font, Font::FontSize size, const char * text, bool setBuffer = true);

	///destructor
	~TextView();

	///setter
	//set text (any alignments will be discarded)
	//WARNING: font and size must be set before calling!
	//@setBuffer determines whether to refresh the text buffer, can be set to false and set later on with a call to refresh()
	void setText(const char * text, bool setBuffer = true);

	//set font size (any alignments will be discarded, refresh() needed)
	void setSize(Font::FontSize size){_size = size;}

	//set font (any alignments will be discarded, refresh() needed)
	void setFont(Font * font){_font = font;}

	//set capacity (the maximum amount of glyphs that can be stored in buffer)
	//@param capacity: if smaller than the number of actually needed glyphs, the buffer (capacity) is shrinked to fit
	void setCapacity(int capacity);

	//update the text buffer (call this when either the font or the size has been changed)
	void refresh();

	///positioning text
	//set absolute position of base line on screen
	//discards all previously made alignments
	void setPosition(float x, float y);

	//align text in given box (align needs to be called manually after adjustments to alignment have been made)
	//@param horizontal: -1 align left, 0 align center, 1 align right, other -> don't align horizontally
	//@param vertical: -1 align top, 0 align center, 1 align bottom, other -> don't align vertically
	//@param pixel_perfect: if true this function will make sure that the text's coordinates will result in an integer
	void setAlignment(float horizontal, float vertical, const Rect & box, float margin = 0, bool pixel_perfect = true);
	void setAlignment(float horizontal, float vertical, float margin = 0, bool pixel_perfect = true);

	//setting box to align (align needs to be called manually after adjustments to alignment have been made)
	void setAlignBox(const Rect & box);

	//align with saved alignment values (automatically called when text changes)
	void align();

	void setColor(float r, float g, float b, float a = 1.f){_color[0] = r; _color[1] = g; _color[2] = b; _color[3] = a;}
	void setColor(const Color & color){_color[0] = color.r; _color[1] = color.g; _color[2] = color.b; _color[3] = color.a;}

	///getter
	const char* getText(){return _text;}
	Font* getFont(){return _font;}
	Font::FontSize getSize(){return _size;}
	float getXPos(){return _xPos;}
	float getYPos(){return _yPos;}

	///render
	//render text to screen at current position
	//NOTE: vertex attribute pointer (vertex and texcoord) must be enabled before calling!
	//@param vertexLocation, texcoordLocation: location of the attributes in current shader
	//@no_transform: if true, no translation is done before rendering
	void render();
	
	const Rect& getBoundingBox(){return _boundingBox;}
private:
	///private functions
	//generating text buffer with given capacity
	//if a text buffer already exists it is deleted first
	//@param copyToNew: if true -> old data is copied over to new buffer
	void generateTextBuffer(int capacity, bool copyToNew);

	//setting text buffer from given text
	void setTextBuffer(const char * text, int len);


	GLuint _textBuffer;//vertex buffers for displaying text
	int _capacity;//number of glyphs _textBuffer can store at max
	int _used;//how many glyphs are actually used in buffer
	Font * _font;
	Font::FontSize _size;
	Rect _boundingBox;//bounding box surrounding the current text
	char * _text;
	float _xPos, _yPos;	 
	float _color[4];//text color (default black)

	//auto-aligning
	Rect _alignBox;
	float _horizontalAlignment;
	float _verticalAlignment;
	float _alignMargin;
	bool _pixelPerfectAlignment;
	bool _aligned;//is this text aligned to a box?
};

}

#endif
