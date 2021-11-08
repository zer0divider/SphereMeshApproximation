/* Author: Cornelius Marx
 */
#include "zer0engine/zer0engine.h"
#include "ModelViewer.h"
#include "CmdParser.h"


int main(int argc, char ** argv){

	/* parsing commandline arguments */
	CmdParser cmd;
	auto cmd_msaa = cmd.addArg<int>(
		"msaa", 'm',
		"Number of samples for multisampled anti-aliasing: 0, 2, 4, 8, 16.",
		0
	);
	
	auto cmd_fullscreen = cmd.addArg<bool>(
		"fullscreen", 'f',
		"Start window in fullscreen.",
		false
	);

	auto cmd_vsync = cmd.addArg<bool>(
		"vsync", 'v',
		"Enable Vertical Synchronization (V-Sync).",
		false
	);

	auto cmd_window_w = cmd.addArg<int>(
		"window-w", '\0',
		"Set window width in pixels.",
		800
	);

	auto cmd_window_h = cmd.addArg<int>(
		"window-h", '\0',
		"Set window height in pixels.",
		600
	);

	auto cmd_model = cmd.addArg<std::string>(
		"obj", 'o',
		".obj model to load from file.",
		"data/cube.obj",
		CmdParser::IS_FILE
	);

	cmd.addHelp();
	CmdParser::Result r = cmd.parse(argc, argv);
	if(r == CmdParser::HELP){
		std::cout<<cmd.getHelpString()<<std::endl;
		return 0;
	}else if(r == CmdParser::ERROR){
		std::cout<<"Error: "<<cmd.getError()<<std::endl;
		return 1;
	}

	/* initialize zer0engine */
	zer0::init("Sphere Mesh Approximation");

	/* create window */
	if(!zer0::FW->createWindow(
		cmd_window_w->getValue(),
		cmd_window_h->getValue(),
		cmd_msaa->getValue(),
		cmd_fullscreen->getValue(),
		cmd_vsync->getValue()))
	{
		zer0::ERROR("Window creation failed!");
		return 2;
	}

	/* creating and run main application */
	ModelViewer * app = new ModelViewer();
	if(app->init(cmd_model->getValue())){
		zer0::FW->run(app);
	}
	else{
		zer0::ERROR("Application initialization failed!");
	}

	/* cleanup */
	delete(app);
	zer0::shutdown();
	return 0;
}
