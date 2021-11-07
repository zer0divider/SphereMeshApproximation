/* Author: Cornelius Marx
 */
#ifndef ZER0_LOGGER_H
#define ZER0_LOGGER_H

#include <cstdarg>
#include <iostream>
#include "zSingleton.h"

#define LOG Logger::getInstance()
/* logging macros */
#define INFO(X, ...) LOG->printfMode(zer0::Logger::LOG_INFO, true, X, ##__VA_ARGS__)
#define WARNING(X, ...) LOG->printfMode(zer0::Logger::LOG_WARNING, true, X, ##__VA_ARGS__)
#define ERROR(X, ...) LOG->printfMode(zer0::Logger::LOG_ERROR, true, X, ##__VA_ARGS__)

namespace zer0{
	class Logger : public Singleton<Logger>
	{
	public:
		// logging modes
		enum Mode{LOG_INFO, LOG_WARNING, LOG_ERROR};

		/**
		 * constructor
		 */
		Logger();

		/**
		 * destructor 
		 * External log file is closed.
		 */
		~Logger();
		
		/**
		 * initializer
		 * @param std_print Enable printing to stdout/stderr.
		 * @param logfile_path Where to store the logfile. NULL for no output file.
		 */
		void createLog(bool std_print, const char * logfile);


		/**
		 * Log statements on given mode.
		 */
		#ifndef WIN32
			__attribute__((format(printf, 4, 5)))
		#endif
		void printfMode(enum Mode mode, bool new_line, const char * fmt, ...);
		void printfMode(enum Mode mode, bool new_line, const char * fmt, va_list args);

		/**
		 * Helper for printfMode. Printing to all streams that were registerd (std_print/file_print)
		 */
		void printFormatted(const char * fmt, va_list args);
		#ifndef WIN32
			__attribute__((format(printf, 2, 3)))
		#endif
		void printFormatted(const char * fmt, ...);

		/* 
		 * Writing out given number of linebreaks to all registered streams
		 * @param num number of linebreaks to write
		 */
		void writeLines(int num);

		/*** friends ***/
		// init and shutdown are declared as friends in order to provide access to the private singleton object
		friend void init(const char * app_name);
		friend void shutdown();
	private:
		/* close external log file if it was opened on createLog()
		 */
		void closeLog();

		FILE * _logfile;
		bool _std_print; //shall the output be written to console? (std)
		Mode _currentMode;
	};

};

#endif
