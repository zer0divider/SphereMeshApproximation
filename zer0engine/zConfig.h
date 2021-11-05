#ifndef ZER0_CONFIG_H
#define ZER0_CONFIG_H
#include "zSingleton.h"
#include <string>

// this class provides global access to the common configuration of the framework
// e.g. what OpenGL version is used

#define CONFIG Config::getInstance()->getConfig()

namespace zer0{

	class Config : public Singleton<Config>
	{
	public:
		struct ConfigAttributes{
			bool SUPPORTS_NEW_GL;// true if supports new openGL
			std::string APP_NAME;// name of the application
		};
		/* get global config
		 */
		const ConfigAttributes& getConfig(){return _config;}

		friend class Framework;

		friend void init(const char * app_name, int argc, char ** argv);// see declaration in zFramework.cpp
	private:
		ConfigAttributes _config;
	};
};


#endif
