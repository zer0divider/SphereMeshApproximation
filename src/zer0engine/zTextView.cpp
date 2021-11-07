#include "zTextView.h"

using namespace zer0;

TextView::TextView()
{
	_textBuffer = 0;
	_capacity = 0;
	_font = NULL;
	_text = NULL;
	_xPos = 0.f;
	_yPos = 0.f;
	_size = Font::FontSize::NORMAL;
	_used = 0;
	_boundingBox.x = 0.f;
	_boundingBox.y = 0.f;
	_boundingBox.w = 0.f;
	_boundingBox.h = 0.f;
	_color[0] = 0.f;
	_color[1] = 0.f;
	_color[2] = 0.f;
	_color[3] = 1.f;
	_aligned = false;
	_horizontalAlignment = 0;
	_verticalAlignment = 0;
	_alignMargin = 0;
	_pixelPerfectAlignment = false;
}

TextView::TextView(Font * font, Font::FontSize size, int capacity): TextView()
{
	//minimum capacity required
	if(capacity <= 0)
		capacity = 1;

	_font = font;
	_size = size;
	generateTextBuffer(capacity, false);
}

TextView::TextView(Font * font, Font::FontSize size, const char * text, bool setBuffer): TextView()
{
	init(font, size, text, setBuffer);
}

void TextView::init(Font * font, Font::FontSize size, const char * text, bool setBuffer)
{
	_font = font;
	_size = size;
	setText(text, setBuffer);
}

TextView::~TextView()
{
	//free text buffer if it was allocated
	if(_textBuffer != 0)
		glDeleteBuffers(1, &_textBuffer);

	//free text if it was allocated
	if(_text != NULL)
		delete[](_text);
}


void TextView::generateTextBuffer(int capacity, bool copyToNew)
{
	if(_textBuffer == 0)
		glGenBuffers(1, &_textBuffer);

	_capacity = capacity;
	glBindBuffer(GL_ARRAY_BUFFER, _textBuffer);
	if(copyToNew)//restore old content in new buffer
	{
		BufferGlyph * g = new BufferGlyph[_used];
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BufferGlyph)*_used, g);
		glBufferData(GL_ARRAY_BUFFER, sizeof(BufferGlyph)*_capacity, NULL, GL_STATIC_DRAW);//NOTE: already allocated buffer will be automatically deleted
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BufferGlyph)*_used, g); 
		delete[](g);
	}
	else//old content is discarded
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(BufferGlyph)*_capacity, NULL, GL_STATIC_DRAW);//NOTE: already allocated buffer will be automatically deleted
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TextView::setText(const char * text, bool setBuffer)
{
	int len = strlen(text);
	if(len > _capacity)//more capacity is needed
		generateTextBuffer(len, false);

	//copying text
	if(_text != NULL)
		delete[](_text);
	_text = new char[len+1];
	memcpy(_text, text, len+1);

	if(setBuffer)
	{
		//setting text buffer
		setTextBuffer(_text, len);
	}
}

void TextView::refresh()
{
	if(_text != NULL)//text has been set
	{
		setTextBuffer(_text, strlen(_text));	
	}
}

void TextView::setTextBuffer(const char * text, int len)
{
	//rendering text to buffer
	BufferGlyph * glyphs = new BufferGlyph[len];
	_used = _font->getGlyphMap(_size)->getTextBuffer(text, glyphs, len, &_boundingBox);
	glBindBuffer(GL_ARRAY_BUFFER, _textBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BufferGlyph)*_used, glyphs);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//free glyphs (openGL has them now)
	delete[](glyphs);
}

void TextView::setCapacity(int capacity)
{
	if(capacity < _used)//shrink to fit
		generateTextBuffer(_used, true);
	else if(capacity > _used)//enlarge buffer
		generateTextBuffer(capacity, true);
	//nothing to do if capacity == _used
}

void TextView::setPosition(float x, float y)
{
	_xPos = x;
	_yPos = y;
	_aligned = false;
}

void TextView::setAlignment(float horizontal, float vertical, float margin, bool pixel_perfect)
{
	_aligned = true;
	_horizontalAlignment = horizontal;
	_verticalAlignment = vertical;
	_alignMargin = margin;
}

void TextView::setAlignment(float horizontal, float vertical, const Rect & box, float margin, bool pixel_perfect)
{
	_aligned = true;
	_horizontalAlignment = horizontal;
	_verticalAlignment = vertical;
	_alignBox = box;
	_alignMargin = margin;
}

void TextView::setAlignBox(const Rect & box)
{
	_alignBox = box;
}

void TextView::align()
{
	if(!_aligned)
		return;
	GlyphMap * g = _font->getGlyphMap(_size);
	float text_w_half = _boundingBox.w/2.f;
	float text_h_half = (g->getMaxGlyphTop() - g->getMinGlyphBot())/2.f;

	//aligning horizontally
	_xPos = _alignBox.x - text_w_half;
	if(_horizontalAlignment < 0.f) {
		_xPos = _xPos - (_alignBox.x-_alignBox.w+_alignMargin - _xPos)*_horizontalAlignment;
	}
	else if(_horizontalAlignment > 0.f) {
		_xPos = _xPos + (_alignBox.x+_alignBox.w-_alignMargin-_boundingBox.w - _xPos)*_horizontalAlignment;
	}

	if(_pixelPerfectAlignment)
		_xPos = floor(_xPos);

	//aligning vertically
	_yPos = _alignBox.y - _boundingBox.h/2.0f + _boundingBox.y;
	if(_verticalAlignment < 0.f){
		_yPos = _yPos - (_alignBox.y-_alignBox.h+_alignMargin +_boundingBox.y - _yPos)*_verticalAlignment;
	}
	else if(_verticalAlignment > 0.f){
		_yPos = _yPos + (_alignBox.y+_alignBox.h-_alignMargin-_boundingBox.h+_boundingBox.y -_yPos)*_verticalAlignment;
	}

	if(_pixelPerfectAlignment)
		_yPos = floor(_yPos);
}

void TextView::render()
{
	GLint 	vertexLocation = SHADER->getVertexLocation(),
			texcoordLocation = SHADER->getUVLocation();
	//setting up vertex attrib pointers
	glBindBuffer(GL_ARRAY_BUFFER, _textBuffer);
	glEnableVertexAttribArray(vertexLocation);
	glEnableVertexAttribArray(texcoordLocation);
	glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);
	glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (const void*)(sizeof(float)*2));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	SHADER->setColor(_color);
	//binding needed glyph texture
	_font->getGlyphMap(_size)->bindMap();
	//rendering at current position
	Matrix4 m;
	m.setTranslation(Vector3D(_xPos, _yPos, 0.f));
	SHADER->setModelMatrix(m);
	glDrawArrays(GL_TRIANGLES, 0, _used*6);
	glDisableVertexAttribArray(vertexLocation);
	glDisableVertexAttribArray(texcoordLocation);
}
