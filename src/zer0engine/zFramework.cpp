/* Author: Cornelius Marx
 */

#include "zFramework.h"

using namespace zer0;

// init
void zer0::init(const char * app_name)
{
	// create singleton framework object
	Logger::create();
	Logger::getInstance()->createLog(true, NULL);
	Framework::create();
	Config::create();

	// initialize framework
	Framework::getInstance()->init(app_name);
}

// shutdown
void zer0::shutdown()
{
	// remove singleton instance
	Framework::destroy();
	Logger::destroy();
}

Framework::Framework() : _viewportMode(VIEW_NORMAL), _renderOnChange(false), _renderRequest(true), _desiredFPS(60), _measuredFPS(0.0), _deltaTime(0)
{
}

void Framework::init(const char * app_name)
{
	INFO("\nInitializing framework:");
	//initialize SDL
	INFO("-> SDL2");
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		ERROR("Failed to initialize SDL: %s", SDL_GetError());
		exit(1);
	}

	// set app name globally
	Config::getInstance()->_config.APP_NAME = app_name;
}

Framework::~Framework(){
	INFO("Application shutdown...");

	// free default texture
	Texture2D::freeDefaultTextures();

	// quit SDL
	SDL_Quit();
}

bool Framework::createWindow(int window_w, int window_h, int multisamples, bool fullscreen, bool vsync, bool hidden)
{
	unsigned int sdl_flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	//getting current screen resolution if fullscreen activated
	if(window_w <= 0 || window_h <= 0){
		_windowW = 800;
		_windowW = 600;
	}
	if(fullscreen){
		sdl_flags |= SDL_WINDOW_FULLSCREEN;
		//getting current display-resolution
		SDL_DisplayMode current;
		if (SDL_GetDesktopDisplayMode(0, &current) != 0){
			WARNING("Could not retrieve current display resolution: %s", SDL_GetError());
		}
		else{
			_windowW = current.w;
			_windowH = current.h;
		}
	}

	INFO(	"Creating window:\n"
			"-> resolution:   %dx%d\n"
			"-> multisamples: %d\n"
			"-> fullscreen:   %d\n"
			"-> vsync:        %d\n",
			window_w, window_h, multisamples, (int)fullscreen, (int)vsync);

	_windowW = window_w;
	_windowH = window_h;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if(multisamples > 0){
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisamples);
	}
	if(hidden){
		sdl_flags |= SDL_WINDOW_HIDDEN;
	}
	_mainWindow = SDL_CreateWindow(CONFIG.APP_NAME.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _windowW, _windowH, sdl_flags);
		
	if (_mainWindow == NULL) { //failed to create a window
		ERROR("Error while creating SDL window: %s", SDL_GetError());
		return false;
	}

	//create gl-context
	SDL_GLContext glContext = SDL_GL_CreateContext(_mainWindow);
	if (glContext == 0){
		ERROR("Error while creating OpenGL Context: %s", SDL_GetError());
		return false;
	}
	const char * gl_version_string = (const char*)glGetString(GL_VERSION);
	INFO(	"System Info:\n"
			"-> Operating System:  %s\n"
			"-> CPU cores:         %d\n"
			"-> RAM:               %dMB\n"
			"-> Video Driver:      %s\n"
			"-> OpenGL Version:    %s\n", 
			SDL_GetPlatform(),
			SDL_GetCPUCount(),
			SDL_GetSystemRAM(),
			SDL_GetVideoDriver(0),
			gl_version_string);


	// activate vsync
	if(vsync)
		SDL_GL_SetSwapInterval(1);

	// init glew
	glewExperimental = GL_TRUE;// support experimental drivers
	GLenum glew_res = glewInit();
	if (glew_res != GLEW_OK){
		ERROR("Error while initializing GLEW: %s", (const char *)glewGetErrorString(glew_res));
		return false;
	}

	// extract version from string (glGetString(GL_VERSION) is the only reliable source for the OpenGL version)
	int gl_major_version = gl_version_string[0]-'0';
	int gl_minor_version = gl_version_string[2]-'0';
	int gl_version = gl_major_version*10+gl_minor_version;
	if(gl_version >= 33){// make sure the opengl version is >= 3.3
		INFO("Using OpenGL 3.3 core functions.\n");
		Config::getInstance()->_config.SUPPORTS_NEW_GL = true;
	}
	else{
		INFO("Using OpenGL 2.1 core functions (OpenGL 3.3 not supported).\n");
		Config::getInstance()->_config.SUPPORTS_NEW_GL = false;
	}

	// creating default vao
	glGenVertexArrays(1, &_defaultVAO);
	glBindVertexArray(_defaultVAO);

	// opengl settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glViewport(0, 0, _windowW, _windowH);

	// initialize other parameters
	_desiredFPS = 60;
	_measuredFPS = 0;

	// initialize default texture
	Texture2D::initDefaultTextures();

	// initialize default fonts
	//Font::initDefaultFonts("./fonts/Bitstream_Regular.ttf", "./fonts/Bitstream_Bold.ttf");

	// initialize default shaders
	INFO("-> Compiling default shader.\n");
	Shader::initDefaultShaders();

	if(multisamples > 0){
		glEnable(GL_MULTISAMPLE);
	}

	// set initial render mode
	setRenderMode(RenderMode::WORLD_3D);

	return true;
}

void Framework::showWindow()
{
	SDL_ShowWindow(_mainWindow);
}

void Framework::run(Application * app)
{
	assert(app != NULL);
	// initial delta time
	_deltaTime = 1000/_desiredFPS;
	_lastFrameMeasureTime = SDL_GetTicks(); 
	_frameCount = 0;
	_measuredFPS = 0;
	Uint32 last_ticks = SDL_GetTicks();
	// trigger initial resize event
	app->eventWindowResized(_windowW, _windowH);
	while(1){
		last_ticks = SDL_GetTicks();
		// processing events
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:{// application closed by user
					return;
				}break;
				case SDL_WINDOWEVENT:{// window resized
					if(e.window.event == SDL_WINDOWEVENT_RESIZED){
					_windowW = e.window.data1;
					_windowH = e.window.data2;

					if(_viewportMode == VIEW_NORMAL){
						glViewport(0, 0, _windowW, _windowH);
					}
					else if(_viewportMode == VIEW_VSPLIT){
						app->eventWindowResized(_windowW/2, _windowH);
					}
					else{
						app->eventWindowResized(_windowW, _windowH/2);
					}
					_renderRequest = true;
					}
				}break;
				case SDL_KEYUP:
				case SDL_KEYDOWN:{
					app->eventKeyboard(e.key.keysym.sym, e.key.state == SDL_PRESSED, e.key.repeat);
				}break;
				case SDL_MOUSEMOTION:{
					app->eventMouseMotion(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
				}break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:{
					app->eventMouseButton(e.button.button, e.button.state == SDL_PRESSED, e.button.x, e.button.y, e.button.clicks);
				}break;
				case SDL_MOUSEWHEEL:{
					app->eventMouseWheel(e.wheel.x, e.wheel.y);
				}
				default:{// 
				}
			}
			app->event(e);
		}
		if(!app->update())
			return;

		// rendering
		if(!_renderOnChange || _renderRequest){
			glClear(_clearMask);
			switch(_viewportMode){
			case VIEW_VSPLIT:
				glViewport(0,0, _windowW/2, _windowH);
				app->render(1);
				glViewport(_windowW/2, 0, _windowW/2, _windowH);
				app->render(2);
				glClear(GL_DEPTH_BUFFER_BIT);
			break;
			case VIEW_HSPLIT:
				glViewport(0,0, _windowW, _windowH/2);
				app->render(1);
				glViewport(0, _windowH/2, _windowW, _windowH/2);
				app->render(2);
				glClear(GL_DEPTH_BUFFER_BIT);
			break;
			}
			glViewport(0, 0, _windowW, _windowH);
			app->render(0);

			// swap buffer
			SDL_GL_SwapWindow(_mainWindow);

			_renderRequest = false;
		}
		// const fps delay
		Uint32 render_time = SDL_GetTicks()-last_ticks;
		int frametime = 1000/_desiredFPS;
		int delay = frametime-render_time;
		if(delay > 0)
			SDL_Delay((Uint32)delay);

		_frameCount++;

		Uint32 ticks_now = SDL_GetTicks();
		if(ticks_now/ZER0_FRAME_MEASURE_INTERVAL != last_ticks/ZER0_FRAME_MEASURE_INTERVAL)
		{
			_measuredFPS = _frameCount*1000.0f/(ticks_now-_lastFrameMeasureTime);
			_lastFrameMeasureTime = ticks_now;
			_frameCount = 0;
		}

		// calculate delta time of last frame
		_deltaTime = SDL_GetTicks() - last_ticks;

	}
}

void Framework::setRenderMode(RenderMode mode, Shader * shader)
{
	switch(mode)
	{
		case RenderMode::WORLD_2D:
		{
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			if(shader == NULL)
				shader = &Shader::DefaultShader;
			shader->use();
			shader->setColor(Color::WHITE);
			Matrix4 m;
			float aspect = getWindowAspectWH();
			m.setOrtho(-aspect, +aspect, -1, 1, -1, 1);
			shader->setProjectionMatrix(m);
			m.loadIdentity();
			shader->setViewMatrix(m);
		}break;
		case RenderMode::WORLD_3D:
		{
			if(shader == NULL)
				shader = &Shader::DefaultShader;
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			shader->use();
			shader->setColor(Color::WHITE);
			Matrix4 m;
			m.setPerspectiveY(70.0, getWindowAspectWH(), 0.1, 1000);
			shader->setProjectionMatrix(m);
			m.loadIdentity();
			shader->setViewMatrix(m);
		}break;
		case RenderMode::SCREEN_TEXT:
		{
			if(shader == NULL)
				shader = &Shader::TextShader;
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			shader->use();
			shader->setColor(Color::BLACK);
			Matrix4 m;
			m.setOrtho(0, _windowW, _windowH, 0, -1, 1);
			shader->setProjectionMatrix(m);
			m.loadIdentity();
			shader->setViewMatrix(m);
		}break;
		default:{
		}
	}
}

float Framework::getViewportAspectWH()
{
	switch(_viewportMode){
	default:
	case VIEW_NORMAL:
		return getWindowAspectWH();
	case VIEW_VSPLIT:
		return (static_cast<float>(_windowW)/_windowH)/2.0f;
	case VIEW_HSPLIT:
		return (static_cast<float>(_windowW)/_windowH)*2.0f;
	}
}
