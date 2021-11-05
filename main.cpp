#include "zer0engine/zer0engine.h"
#include "ModelViewer.h"

int main(int argc, char ** argv){
	zer0::init("Sphere Mesh Approximation", argc, argv);

	if(!zer0::FW->createWindow()){
		zer0::ERROR("Window creation failed!");
		return 1;
	}

	ModelViewer * app = new ModelViewer();
	zer0::FW->run(app);
	delete(app);

	zer0::shutdown();
	return 0;
}
