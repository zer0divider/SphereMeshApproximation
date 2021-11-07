#ifndef ZER0_SINGLETON_H
#define ZER0_SINGLETON_H

#include <stdlib.h>
#include <cassert>

namespace zer0{

	/**
	 * Singleton class used in Framework and Logger class to provide a single global instance at runtime.
	 */
	template <class T>
	class Singleton{
		public:
			static T* getInstance(){assert(_singleton != NULL); return _singleton;}
		protected:
			static void create(){_singleton = new T();}
			static void destroy(){delete _singleton; _singleton = NULL;}
			static T* _singleton;
	};
	template <class T>
	T* Singleton<T>::_singleton = NULL;
};

#endif
