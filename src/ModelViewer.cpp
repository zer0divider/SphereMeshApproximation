/* Author: Cornelius Marx
 */
#include "ModelViewer.h"

/* configuration */
#define CAMERA_ROTATION_FACTOR 0.005
#define CAMERA_ZOOM_FACTOR     1.1
#define CAMERA_FOV             70
#define BACKGROUND_COLOR       0x303030FF
#define MESH_FILL_COLOR        0x2c92ffFF
#define MESH_LINE_COLOR        0xF0F0F0FF
#define LINE_WIDTH             2
#define KEY_MODE_SWITCH        SDLK_m
/*****************/

using namespace zer0;

ModelViewer::ModelViewer():
	_camera(CAMERA_ROTATION_FACTOR, CAMERA_ZOOM_FACTOR),
	_meshFillColor(MESH_FILL_COLOR),
	_meshLineColor(MESH_LINE_COLOR)
{
}

bool ModelViewer::init(const std::string & model_file)
{
	_modelFilename = model_file;
	// opengl configuration
	Color bg_color(BACKGROUND_COLOR);
	glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	glLineWidth(LINE_WIDTH);
	glCullFace(GL_BACK);
	// initialize shader
	INFO("-> Compiling Diffuse Shader");
	_meshShader.init();
	_meshShader.use();
	_meshShader.setLightDir(Vector3D(1.0, 1.5, 1.3).getNormalized());

	// loading test obj
	std::vector<Vector3D> vertex_data;
	std::vector<unsigned int> index_data;
	INFO("Loading mesh from '%s'...", _modelFilename.c_str());
	if(_mesh.loadOBJFromFile(_modelFilename.c_str(), Mesh::NORMAL, &vertex_data, &index_data)){
		INFO("  -> #verticies: %d", _mesh.getVertexCount());
		INFO("  -> #triangles: %d", _mesh.getElementCount()/3);
		INFO(" ");
	}
	else{
		return false;
	}

	_dynamicMesh.set(vertex_data, index_data);
	_markingEdge = _dynamicMesh.getEdgeList().getFirst();
	_dynamicMesh.getEdgeMesh(_selectedSimplex, _markingEdge);
	_dynamicMesh.debug_print();

	_dynamicMesh.upload(_mesh);

	// set initial draw mode
	setDrawMode(FILL_AND_LINE);

	return true;
}

ModelViewer::~ModelViewer()
{
}

void ModelViewer::render()
{
	/* update camera */
	_camera.upload();
	
	/* render model */
	drawMesh();
}

void ModelViewer::drawMesh()
{
	Matrix4 m;
	m.loadIdentity();
	SHADER->setModelMatrix(m);
	
	_mesh.bind();
	switch(_currentDrawMode){
	case FILL:{
		glDisable(GL_CULL_FACE);
		_meshShader.setLightMode(DiffuseShader::MATCAP);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		SHADER->setColor(_meshFillColor);
		_mesh.draw();
	}break;
	case LINE:{
		glDisable(GL_CULL_FACE);
		_meshShader.setLightMode(DiffuseShader::UNSHADED);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		SHADER->setColor(_meshLineColor);
		_mesh.draw();
	}break;
	case FILL_AND_LINE:{
		glDisable(GL_CULL_FACE);
		_meshShader.setLightMode(DiffuseShader::UNSHADED);
		// draw fill
		SHADER->setColor(_meshFillColor);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		_mesh.draw();

		// draw line on top
		_meshShader.setNormalOffset(0.0001);
		SHADER->setColor(_meshLineColor);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		_mesh.draw();
		_meshShader.setNormalOffset(0);
	}break;
	}

	// draw selected simplex
	_meshShader.setLightMode(DiffuseShader::UNSHADED);
	SHADER->setColor(Color::RED);
	glClear(GL_DEPTH_BUFFER_BIT);
	_selectedSimplex.bind();
	_selectedSimplex.draw();
}

void ModelViewer::eventKeyboard(SDL_Keycode key, bool pressed, int repeat)
{
	if(repeat == 0 && pressed){// hot keys
		switch(key){
		case KEY_MODE_SWITCH:{
			DrawMode m = (DrawMode)(_currentDrawMode +1);
			if(m >= NUM_DRAW_MODES){
				m = (DrawMode)0;
			}
			setDrawMode(m);
		}break;
		case SDLK_e:{
			if(_markingEdge != nullptr){
				_dynamicMesh.edgeCollapseToCenter(_markingEdge);
				_dynamicMesh.debug_print();
				_dynamicMesh.upload(_mesh);
				_markingEdge = _dynamicMesh.getEdgeList().getFirst();
				if(_markingEdge != nullptr){
					_dynamicMesh.getEdgeMesh(_selectedSimplex, _markingEdge);
				}
			}
		}break;
		}
	}
}

void ModelViewer::eventMouseMotion(int x, int y, int rel_x, int rel_y)
{
	Uint8 state = SDL_GetMouseState(NULL, NULL);
	if(state & SDL_BUTTON_LEFT){
		_camera.mouseRotate(rel_x, rel_y);
	}
}

void ModelViewer::eventMouseWheel(int wheel_x, int wheel_y)
{
	_camera.mouseZoom(wheel_y);
}

void ModelViewer::eventWindowResized(int new_width, int new_height)
{
	// update projection matrix
	_projectionMat.setPerspectiveY(CAMERA_FOV, FW->getWindowAspectWH(), 0.1, 1000);
	SHADER->setProjectionMatrix(_projectionMat);
}

void ModelViewer::setDrawMode(DrawMode mode)
{
	_currentDrawMode = mode;
}

bool ModelViewer::update()
{

	return true;
}
