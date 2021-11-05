#include "ModelViewer.h"

using namespace zer0;

ModelViewer::ModelViewer()
{
	_box.loadPrimitive(Mesh::BOX, Vector3D(1,1,1));
}

ModelViewer::~ModelViewer()
{
}

void ModelViewer::render()
{
	FW->setRenderMode(RenderMode::WORLD_3D);
	Matrix4 m;
	m.setTranslation(Vector3D(0, 0, -3.0));
	Shader::DefaultShader.setViewMatrix(m);
	Shader::DefaultShader.setColor(Color::RED);
	m.setRotationY(FW->getTime()*0.5);
	Shader::DefaultShader.setModelMatrix(m);

	_box.bind();
	_box.draw();
}

bool ModelViewer::update()
{

	return true;
}
