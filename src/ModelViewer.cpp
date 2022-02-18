/* Author: Cornelius Marx
 */
#include "ModelViewer.h"
/* configuration */
#define CAMERA_ROTATION_FACTOR  0.005
#define CAMERA_TRANSLATE_FACTOR 0.001
#define CAMERA_ZOOM_FACTOR      1.1
#define CAMERA_FOV              70
#define BACKGROUND_COLOR        0x303030FF
#define MESH_FILL_COLOR         0x2c92ffFF
#define MESH_LINE_COLOR         0xD0F0FFFF
#define SPHERE_COLOR            0xff922cFF
#define MIN_SPHERE_RADIUS       0.001
#define SPHERE_RADIUS_OFFSET    0.0008
#define CYLINDER_COLOR          0xa0a0a0FF
#define TRIANGLE_COLOR          0xa0a0efFF
#define MIN_CYLINDER_RADIUS     0.001
#define LINE_WIDTH              1
#define NUM_SEGMENTS            64    /* number of segments (aka smoothness) used for rendering spheres and cylinders in sphere mesh */
#define KEY_MODE_SWITCH         SDLK_a
#define KEY_SPHERE_MODE_SWITCH  SDLK_d
#define SEPARATOR_LINE_WIDTH    2
#define SEPARATOR_LINE_COLOR    0x202020FF 
/*****************/

using namespace zer0;

ModelViewer::ModelViewer():
	_camera(CAMERA_ROTATION_FACTOR, CAMERA_ZOOM_FACTOR, CAMERA_TRANSLATE_FACTOR),
	_meshFillColor(MESH_FILL_COLOR),
	_meshLineColor(MESH_LINE_COLOR),
	_separatorLineColor(SEPARATOR_LINE_COLOR),
	_meshDrawMode(FILL, NUM_DRAW_MODES),
	_sphereDrawMode(SAME_COLOR, NUM_SPHERE_DRAW_MODES),
	_sphereMesh(SPHERE_RADIUS_OFFSET)
{
}

bool ModelViewer::init(const std::string & model_file, int num_spheres)
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

	// separator line
	float line_data[4] = {0,1,  0,-1};
	_separatorMesh.set2D(line_data, 2, Mesh::ONLY_POSITION, GL_LINES);

	// loading obj
	std::vector<Vector3D> vertex_data;
	std::vector<unsigned int> index_data;
	INFO("Loading mesh from '%s'...", _modelFilename.c_str());
	if(_originalMesh.loadOBJFromFile(_modelFilename.c_str(), Mesh::NORMAL, &vertex_data, &index_data)){
		INFO("  -> #vertices: %d", _originalMesh.getVertexCount());
		INFO("  -> #triangles: %d", _originalMesh.getElementCount()/3);
		INFO(" ");
	}
	else{
		return false;
	}

	_dynamicMesh.set(vertex_data, index_data);

	_modelCenterPosition = _dynamicMesh.getCenterPos();
	_sphereMesh.setPosition(-_modelCenterPosition);

	// initialize SQEM of each vertex
	INFO("-> Initializing SQEM...");
	Uint32 t = SDL_GetTicks();
	_dynamicMesh.initSQEM();
	Uint32 t2 = SDL_GetTicks();
	INFO("   Done, took %.3f seconds\n", (t2-t)/1000.f);

	// run full Approximation Algorithm
	INFO("-> Running Sphere Mesh Approximation Algorithm (reducing to %d spheres) ...", num_spheres);
	t = SDL_GetTicks();
	_dynamicMesh.sphereApproximation(num_spheres);
	INFO("   Done, took %.3f seconds.\n", (SDL_GetTicks()-t)/1000.f);

	// create sphere mesh
	INFO("-> Uploading mesh...");
	t = SDL_GetTicks();
	updateSphereMeshModel();
	INFO("   Done, took %.3f seconds.\n", (SDL_GetTicks()-t)/1000.f);

	printSphereMeshInfo();

	// show window now that we are done (window was created with hidden flag)
	FW->showWindow();

	// draw only on change
	FW->setRenderOnlyOnChange(true);

	// left/right split view
	FW->setViewportMode(Framework::VIEW_VSPLIT);

	updateSphereMeshColors();

	return true;
}

ModelViewer::~ModelViewer()
{
}

void ModelViewer::printSphereMeshInfo()
{
	INFO("Sphere Mesh:\n"
	     "  -> #spheres/vertices: %lu\n"
	     "  -> #edges:            %lu\n"
	     "  -> #faces:            %lu\n",
		 _dynamicMesh.getVertexList().getSize(),
		 _dynamicMesh.getEdgeList().getSize(),
		 _dynamicMesh.getFaceList().getSize());
}

void ModelViewer::updateSphereMeshModel()
{
	_dynamicMesh.upload(_faceMesh, _edgeMesh);
	_sphereMesh.init(_dynamicMesh, NUM_SEGMENTS, MIN_SPHERE_RADIUS, MIN_CYLINDER_RADIUS);
}

void ModelViewer::updateSphereMeshColors()
{
	if(_sphereDrawMode == DIFFERENT_COLOR){
		_sphereMesh.setColors(zer0::Color(SPHERE_COLOR), zer0::Color(CYLINDER_COLOR), zer0::Color(TRIANGLE_COLOR));
	}
	else{
		_sphereMesh.setColors(zer0::Color(SPHERE_COLOR), zer0::Color(SPHERE_COLOR), zer0::Color(SPHERE_COLOR));
	}
}

void ModelViewer::render(int viewport)
{

	/* draw depending on viewport */
	if(viewport == 0){ // complete window
		SHADER->setProjectionMatrix(Matrix4::IDENTITY);
		SHADER->setViewMatrix(Matrix4::IDENTITY);
		SHADER->setModelMatrix(Matrix4::IDENTITY);
		drawSeparator();
	}
	else{
		SHADER->setProjectionMatrix(_projectionMat);
		_camera.upload();

		if(viewport == 1){// left
			drawMesh();
		} else { // right
			drawSphereMesh();
		}
	}
}

void ModelViewer::setModelCenterPosition()
{
	Matrix4 m;
	m.setTranslation(-_modelCenterPosition);
	SHADER->setModelMatrix(m);
}

void ModelViewer::drawSphereMesh(){
	_meshShader.setLightMode(DiffuseShader::MATCAP);
	if(_sphereDrawMode == SKELETON){
		_sphereMesh.drawSpheres();
		SHADER->setColor(Color(MESH_FILL_COLOR));
		setModelCenterPosition();
		_faceMesh.bind();
		_faceMesh.draw();

		_meshShader.setLightMode(DiffuseShader::UNSHADED);
		SHADER->setColor(Color(MESH_LINE_COLOR));
		_edgeMesh.bind();
		_edgeMesh.draw();
	}
	else{ // draw underlying skeleton + spheres
		_sphereMesh.draw();
	}
}

void ModelViewer::drawMesh()
{
	setModelCenterPosition();
	glLineWidth(LINE_WIDTH);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	_originalMesh.bind();
	switch(_meshDrawMode.get()){
	case FILL:{
		_meshShader.setLightMode(DiffuseShader::MATCAP);
		SHADER->setColor(_meshFillColor);
		_originalMesh.draw();
	}break;
	case LINE:{
		_meshShader.setLightMode(DiffuseShader::UNSHADED);
		SHADER->setColor(_meshLineColor);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		_originalMesh.draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}break;
	case FILL_AND_LINE:{
		_meshShader.setLightMode(DiffuseShader::MATCAP);
		// draw fill
		SHADER->setColor(_meshFillColor);
		_originalMesh.draw();

		// draw line on top
		_meshShader.setNormalOffset(0.001);
		_meshShader.setLightMode(DiffuseShader::UNSHADED);
		SHADER->setColor(_meshLineColor);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		_originalMesh.draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		_meshShader.setNormalOffset(0);
	}break;
	}

}

void ModelViewer::drawSeparator()
{
	// draw separator line
	SHADER->setColor(_separatorLineColor);
	_meshShader.setLightMode(DiffuseShader::UNSHADED);
	glLineWidth(SEPARATOR_LINE_WIDTH);
	_separatorMesh.bind();
	_separatorMesh.draw();
	glLineWidth(1);
}

void ModelViewer::eventKeyboard(SDL_Keycode key, bool pressed, int repeat)
{
	if(repeat == 0 && pressed){// hot keys
		switch(key){
		case KEY_MODE_SWITCH:{
			_meshDrawMode.next();
			FW->renderRequest();
		}break;
		case KEY_SPHERE_MODE_SWITCH:{
			_sphereDrawMode.next();
			updateSphereMeshColors();
			FW->renderRequest();
		}break;
		case SDLK_SPACE:{
			_dynamicMesh.sphereApproximationStep();
			updateSphereMeshModel();
			printSphereMeshInfo();
			FW->renderRequest();
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
	if(state & SDL_BUTTON_LMASK){
		_camera.mouseRotate(rel_x, rel_y);
		FW->renderRequest();
	}
	else if(state & SDL_BUTTON_RMASK){
		_camera.mouseTranslate(rel_x, rel_y);
		FW->renderRequest();
	}
}

void ModelViewer::eventMouseWheel(int wheel_x, int wheel_y)
{
	_camera.mouseZoom(wheel_y);
	FW->renderRequest();
}

void ModelViewer::eventWindowResized(int new_width, int new_height)
{
	// update projection matrix
	_projectionMat.setPerspectiveY(CAMERA_FOV, FW->getViewportAspectWH(), 0.1, 100);
}

bool ModelViewer::update()
{
	return true;
}
