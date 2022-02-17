/* Author: Cornelius Marx
 */
#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include "zer0engine/zer0engine.h"
#include "DiffuseShader.h"
#include "DynamicMesh.h"
#include "SphereMesh.h"
#include "ModeSwitcher.h"

class ModelViewer : public zer0::Application
{
public:
	enum DrawMode{FILL, LINE, FILL_AND_LINE, NUM_DRAW_MODES};
	enum SphereDrawMode{SAME_COLOR, DIFFERENT_COLOR, SKELETON, NUM_SPHERE_DRAW_MODES};

	/* initialize */
	bool init(const std::string & model_file, int num_spheres);

	ModelViewer();
	~ModelViewer();
	bool update()override;
	void render(int viewport)override;
	void eventKeyboard(SDL_Keycode key, bool pressed, int repeat)override;
	void eventMouseMotion(int x, int y, int rel_x, int rel_y)override;
	void eventMouseWheel(int wheel_x, int wheel_y)override;
	void eventWindowResized(int new_width, int new_height)override;

private:
	void drawSeparator(); // draw line dividing left/right view
	void drawMesh();
	void drawSphereMesh();
	void selectEdge(DynamicMesh::Edge * e);
	void updateSphereMeshModel();
	void printSphereMeshInfo();
	void setModelCenterPosition(); // set model matrix in shader
	void updateSphereMeshColors();

	zer0::Mesh _originalMesh;
	zer0::Mesh _faceMesh;
	zer0::Mesh _edgeMesh;
	zer0::Mesh _selectedEdgeMesh;
	zer0::Mesh _selectedEdgeFacesMesh;
	zer0::Mesh _separatorMesh; // line dividing left/right view
	
	SphereMesh _sphereMesh;
	std::string _modelFilename;
	DynamicMesh _dynamicMesh;
	DynamicMesh::Edge *  _selectedEdge;
	zer0::Camera _camera;
	zer0::Color _meshFillColor;
	zer0::Color _meshLineColor;
	zer0::Color _separatorLineColor;
	ModeSwitcher<DrawMode>       _meshDrawMode;
	ModeSwitcher<SphereDrawMode> _sphereDrawMode;
	DiffuseShader _meshShader;
	zer0::Matrix4 _projectionMat;

	zer0::Vector3D _modelCenterPosition;
};

#endif
