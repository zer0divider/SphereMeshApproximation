/* Author: Cornelius Marx
 */
#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include "zer0engine/zer0engine.h"
#include "DiffuseShader.h"
#include "DynamicMesh.h"

class ModelViewer : public zer0::Application{
public:
	enum DrawMode{FILL, LINE, FILL_AND_LINE, NUM_DRAW_MODES};

	/* initialize */
	bool init(const std::string & model_file);

	ModelViewer();
	~ModelViewer();
	bool update()override;
	void render()override;
	void eventKeyboard(SDL_Keycode key, bool pressed, int repeat)override;
	void eventMouseMotion(int x, int y, int rel_x, int rel_y)override;
	void eventMouseWheel(int wheel_x, int wheel_y)override;
	void eventWindowResized(int new_width, int new_height)override;

private:
	void drawMesh();
	void setDrawMode(DrawMode mode);

	zer0::Mesh _mesh;
	std::string _modelFilename;
	DynamicMesh _dynamicMesh;
	zer0::Camera _camera;
	zer0::Color _meshFillColor;
	zer0::Color _meshLineColor;
	DrawMode _currentDrawMode;
	DiffuseShader _meshShader;
	zer0::Matrix4 _projectionMat;
};

#endif
