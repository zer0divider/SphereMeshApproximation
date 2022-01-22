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
#define SPHERE_COLOR           0xff922cFF
#define LINE_WIDTH             2
#define KEY_MODE_SWITCH        SDLK_m
/*****************/

using namespace zer0;

ModelViewer::ModelViewer():
	_camera(CAMERA_ROTATION_FACTOR, CAMERA_ZOOM_FACTOR),
	_meshFillColor(MESH_FILL_COLOR),
	_meshLineColor(MESH_LINE_COLOR),
	_sphereColor(SPHERE_COLOR)
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

	// creating single sphere
	_singleSphereMesh.loadPrimitive(Mesh::SPHERE, Vector3D(2,2,2), 16);

	// loading test obj
	std::vector<Vector3D> vertex_data;
	std::vector<unsigned int> index_data;
	INFO("Loading mesh from '%s'...", _modelFilename.c_str());
	if(_faceMesh.loadOBJFromFile(_modelFilename.c_str(), Mesh::NORMAL, &vertex_data, &index_data)){
		INFO("  -> #verticies: %d", _faceMesh.getVertexCount());
		INFO("  -> #triangles: %d", _faceMesh.getElementCount()/3);
		INFO(" ");
	}
	else{
		return false;
	}

	_dynamicMesh.set(vertex_data, index_data);


	INFO("Initializing SQEM...");
	Uint32 t = SDL_GetTicks();
	_dynamicMesh.initSQEM();
	Uint32 t2 = SDL_GetTicks();
	INFO("Took %.2f seconds\n", (t2-t)/1000.f);
	INFO("Done.");

	_dynamicMesh.upload(_faceMesh, _edgeMesh);

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

	/*
	Matrix4 m;
	m.loadIdentity();
	SHADER->setModelMatrix(m);
	SHADER->setColor(_meshFillColor);
	_meshShader.setLightMode(DiffuseShader::MATCAP);
	_singleSphereMesh.bind();
	_singleSphereMesh.draw();
	*/
	drawSphereMesh();
}

void ModelViewer::drawSphereMesh(){
	Matrix4 m;
	m.loadIdentity();

	BackReferenceList<DynamicMesh::Vertex> & verts = _dynamicMesh.getVertexList();
	SHADER->setColor(_sphereColor);
	_singleSphereMesh.bind();
	for(DynamicMesh::Vertex * v = verts.getFirst(); v != nullptr; v = v->getNext()){
		if(v->sphere_radius > 0.001f){
			m.setTransform(v->position, v->sphere_radius, Vector3D_zer0);
			//m.setTranslation(v->position);
			SHADER->setModelMatrix(m);
			_singleSphereMesh.draw();
		}
	}
}

void ModelViewer::drawMesh()
{
	Matrix4 m;
	m.loadIdentity();
	SHADER->setModelMatrix(m);
	
	glLineWidth(LINE_WIDTH);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	switch(_currentDrawMode){
	case FILL:{
		_faceMesh.bind();
		_meshShader.setLightMode(DiffuseShader::MATCAP);
		SHADER->setColor(_meshFillColor);
		_faceMesh.draw();
	}break;
	case LINE:{
		_edgeMesh.bind();
		_meshShader.setLightMode(DiffuseShader::UNSHADED);
		SHADER->setColor(_meshLineColor);
		_edgeMesh.draw();
	}break;
	case FILL_AND_LINE:{
		_faceMesh.bind();
		_meshShader.setLightMode(DiffuseShader::MATCAP);
		// draw fill
		SHADER->setColor(_meshFillColor);
		_faceMesh.draw();

		// draw line on top
		//_meshShader.setNormalOffset(0.0001);
		SHADER->setColor(_meshLineColor);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		_edgeMesh.bind();
		_edgeMesh.draw();
		//_meshShader.setNormalOffset(0);
	}break;
	}

/*
	glClear(GL_DEPTH_BUFFER_BIT);
	_meshShader.setLightMode(DiffuseShader::UNSHADED);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// draw faces of selected edge
	SHADER->setColor(Color::GREEN);
	_selectedEdgeFacesMesh.bind();
	_selectedEdgeFacesMesh.draw();
	*/

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
		case SDLK_SPACE:{
		// collapse into single vertex
			Uint32 t = SDL_GetTicks();
			_dynamicMesh.sphereApproximation(20);
			INFO("Done, took %.2f seconds.", (SDL_GetTicks()-t)/1000.f);
			_dynamicMesh.upload(_faceMesh, _edgeMesh);
		}break;
		case SDLK_RIGHT:
		case SDLK_LEFT:{
			if(_selectedEdge != nullptr){
				DynamicMesh::Edge * next_edge;
				if(key == SDLK_RIGHT){
					next_edge = _selectedEdge->getNext();
					if(next_edge == nullptr){
						next_edge = _dynamicMesh.getEdgeList().getFirst();
					}
				}
				else if(key == SDLK_LEFT){
					next_edge = _selectedEdge->getPrev();
					if(next_edge == nullptr){
						next_edge = _dynamicMesh.getEdgeList().getLast();
					}
				}
				selectEdge(next_edge);
			}
		}break;
		}
	}
}

void ModelViewer::selectEdge(DynamicMesh::Edge * e)
{
	_selectedEdge = e;
	if(e != nullptr){
		_selectedEdge->upload(_selectedEdgeMesh);
		_selectedEdge->uploadFaces(_selectedEdgeFacesMesh);
	}
	else{
		_selectedEdgeMesh.clear();
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
