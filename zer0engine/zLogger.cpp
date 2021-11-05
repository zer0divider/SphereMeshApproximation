#include "zer0engine.h"

using namespace zer0;

Logger::Logger()
{
	_logfile = NULL;
	_std_print = true;
}

Logger::~Logger()
{
	closeLog();
}

void Logger::createLog(bool std_print, const char * logfile_path)
{
	_std_print = std_print;
	_logfile = NULL;
	if(logfile_path != NULL){
		_logfile = fopen(logfile_path, "w");
		if(_logfile == NULL)
			fprintf(stderr, "ERROR: Could not create logfile (%s)!\n", logfile_path);
	}
}

void Logger::printfMode(enum Mode mode, bool new_line, const char * fmt, ...)
{
	va_list arg_list;
	va_start(arg_list, fmt);
	printfMode(mode, new_line, fmt, arg_list);
	va_end(arg_list);
}

void Logger::printfMode(enum Mode mode, bool new_line, const char * fmt, va_list args)
{
	_currentMode = mode;
	// printing prefix
	switch(mode)
	{
	case LOG_INFO:
	break;
	case LOG_WARNING:
		printFormatted("WARNING: ");
	break;
	case LOG_ERROR:
		printFormatted("ERROR: ");
	break;
	default:
		printf("<unknown log mode %d>\n", (int)mode);
		return;
	}

	// printing actual formatted text
	printFormatted(fmt, args);

	// print a new line
	if(new_line){
		writeLines(1);
	}

}

void Logger::printFormatted(const char * fmt, ...)
{
	va_list arg_list;
	va_start(arg_list, fmt);
	printFormatted(fmt, arg_list);
	va_end(arg_list);
}

void Logger::printFormatted(const char * fmt, va_list args)
{
	if(_logfile){
		va_list args_copy;
		va_copy(args_copy, args);
		// printing to logfile if active
		vfprintf(_logfile, fmt, args_copy);
		fflush(_logfile);
		va_end(args_copy);
	}

	// printing to stdout if active
	if(_std_print) {
		FILE * out = stdout;
		if(_currentMode == LOG_ERROR)
			out = stderr;
		vfprintf(out, fmt, args);
		fflush(out);
	}
}

void Logger::writeLines(int num)
{
	if(_std_print){
		for(int i = 0; i < num; i++)
			puts("");
	}

	if(_logfile){
		for(int i = 0; i < num; i++)
			fputs("\n", _logfile);
	}
}

void Logger::closeLog()
{
	if(_logfile)
	{
		fclose(_logfile);
		_logfile = NULL;
	}
}
