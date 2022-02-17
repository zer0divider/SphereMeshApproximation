/* Author: Cornelius Marx
 */
#ifndef ZER0_FRAMEWORK_H
#define ZER0_FRAMEWORK_H

#include <string>
#include <SDL2/SDL.h>
#include "glew/glew.h"
#include "zSingleton.h"
#include "zLogger.h"
#include "zTexture.h"
#include "zShader.h"
#include "zConfig.h"
#include "zRect.h"

/* singleton access */
#define FW Framework::getInstance()

#define ZER0_FRAME_MEASURE_INTERVAL 1000 //ms

namespace zer0{

	enum class RenderMode{WORLD_2D, WORLD_3D, SCREEN_TEXT};
	/**
	 * Initializes the engine. This function must be called before using any other.
	 * Creates singleton objects and initializes logger class.
	 */
	void init(const char * app_name);

	/**
	 * Shuts down framework. Releases all memory allocated by engine and quits SDL.
	 */
	void shutdown();

	/**
	 * Abstract application defining callback functions for run()-function of Framework
	 */
	class Application{
		public:
			Application(){}
			/**
			 * destructor
			 */
			virtual ~Application(){}


			/**
			 * called for every SDL event retrieved during polling in every iteration of the main loop
			 */
			virtual void event(const SDL_Event & event){}

			/**
			 * events will be signaled by calling a specific callback (e.g. eventKeyboard() when keyboard event occured) instead of calling the event() function
			 */
			virtual void eventWindowResized(int new_width, int new_height){}
			virtual void eventKeyboard(SDL_Keycode key, bool key_down, int repeat){}
			virtual void eventMouseButton(Uint8 button, bool pressed, int x, int y, int clicks){}
			virtual void eventMouseMotion(int x, int y, int rel_x, int rel_y){}
			virtual void eventMouseWheel(int wheel_x, int wheel_y){}

			/**
			 * called once per main loop cycle before the rendering function and after all events have been triggered
			 * @return True if the application should run further, false if application should be stopped
			 */
			virtual bool update(){return true;}

			/**
			 * Called once per main loop cycle after the update()
			 * @param viewport 1 if left/top viewport shall be rendered (VIEW_VSPLIT), 0 if right/bottom viewport shall be rendered (VIEW_HSPLIT), 0 when whole screen shall be rendered
			 */
			virtual void render(int viewport){}
	};
	class Framework : public Singleton<Framework>{
		public:
			
			Framework();
			~Framework();

			/**
			 * Create the main window with OpenGL context.
			 * @param window_w Width of the window in px
			 * @param window_h Height of the window in px
			 * @param multisamples Number of multisamples for rendering (e.g. 2, 4, 8, 16). If set to 0 multisampling will be disabled.
			 * @return True on success, false on error.
			 */
			bool createWindow(int window_w = 800, int window_h = 600, int multisamples = 0,
								bool fullscreen = false, bool vsync = false);

			/**
			 * Running main loop with callbacks to given application.
			 * @param app The application to run
			 */
			void run(Application * app);

			/**
			 * set whether rendering a new frame only happens on change signaled by the application
			 * @param b if set to true new frame is only drawn on change
			 */
			void setRenderOnlyOnChange(bool b){_renderOnChange = b;}

			/**
			 * re-render (only has an effect if render-on-change is active)
			 */
			void renderRequest(){_renderRequest = true;}

			enum ViewPortMode{	VIEW_NORMAL, /* normal, one viewport covers whole window */
								VIEW_VSPLIT, /* two viewports, window is split vertically */
								VIEW_HSPLIT  /* two viewports, window is split horizontally */
							};
			/**
			 * set viewport mode (see enum for description)
			 */
			void setViewportMode(ViewPortMode v){_viewportMode = v;}

			/**
			 * get window size
			 */	
			int getWindowW(){return _windowW;}
			int getWindowH(){return _windowH;}

			/**
			 * get aspect ratio window_width/window_height
			 */
			float getWindowAspectWH(){return static_cast<float>(_windowW)/_windowH;}

			/**
			 * get aspect ratio of viewport (depends on viewport mode)
			 */
			float getViewportAspectWH();
			
			/**
			 * get aspect ratio window_height/window_width
			 */
			float getWindowAspectHW(){return static_cast<float>(_windowH)/_windowW;}

			/**
			 * get current time in seconds since application start
			 */
			float getTime(){return SDL_GetTicks()/1000.f;}

			/**
			 * get time passed since last frame in seconds
			 */
			float getDeltaTime(){return (_deltaTime)/1000.f;}

			/**
			 * set render mode
			 */
			void setRenderMode(RenderMode mode, Shader * shader = NULL);

			/**
			 * get rect describing the current window size in pixel coordinates
			 */
			void getWindowRect(Rect & r){r.set(_windowW/2, _windowH/2, _windowW/2, _windowH/2);}
	
			/*** friends ***/
			// init and shutdown are declared as friends in order to provide access to the private singleton object
			friend void init(const char * app_name);
			friend void shutdown();
		private:
			/* intialize SDL and program paths 
			 * called by init()
			 */
			void init(const char* app_name);

			GLbitfield _clearMask;
			int _windowW;
			int _windowH;
			SDL_Window * _mainWindow;
			GLuint _defaultVAO;
			int _desiredFPS;
			float _measuredFPS;
			int _frameCount;
			Uint32 _lastFrameMeasureTime;
			Uint32 _deltaTime;
			bool _renderOnChange;
			bool _renderRequest;
			ViewPortMode _viewportMode;
	};

};

#endif
