/* Author: Cornelius Marx
 */
#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include "zer0engine/zer0engine.h"
#include "DiffuseShader.h"
#include "DynamicMesh.h"
#include "SphereMesh.h"

class ModelViewer : public zer0::Application
{
public:
	enum DrawMode{FILL, LINE, FILL_AND_LINE, NUM_DRAW_MODES};

	/* initialize */
	bool init(const std::string & model_file);

	ModelViewer();
	~ModelViewer();
	bool update()override;
	void render(int viewport)override;
	void eventKeyboard(SDL_Keycode key, bool pressed, int repeat)override;
	void eventMouseMotion(int x, int y, int rel_x, int rel_y)override;
	void eventMouseWheel(int wheel_x, int wheel_y)override;
	void eventWindowResized(int new_width, int new_height)override;

private:
	void drawMesh();
	void drawSphereMesh();
	void setDrawMode(DrawMode mode);
	void selectEdge(DynamicMesh::Edge * e);

	zer0::Mesh _originalMesh;
	zer0::Mesh _faceMesh;
	zer0::Mesh _edgeMesh;
	zer0::Mesh _selectedEdgeMesh;
	zer0::Mesh _selectedEdgeFacesMesh;
	
	SphereMesh _sphereMesh;
	std::string _modelFilename;
	DynamicMesh _dynamicMesh;
	DynamicMesh::Edge *  _selectedEdge;
	zer0::Camera _camera;
	zer0::Color _meshFillColor;
	zer0::Color _meshLineColor;
	DrawMode _currentDrawMode;
	DiffuseShader _meshShader;
	zer0::Matrix4 _projectionMat;

	zer0::Mesh _separatorMesh;
};

#endif
