#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>

// forward declaration for friendship
class CmdParser;

// abstract class
enum class CmdArgType{INT, FLOAT, BOOL, STRING};

class CmdArgBase
{
public:
	const std::string& getDescription()const{return _description;}
	CmdArgType getType()const{return _type;}
	unsigned int getOptions()const{return _options;}
	const std::string& getFullArg()const{return _fullArg;}
	const std::string& getShortArg()const{return _singleCharArg;}
	virtual ~CmdArgBase(){}
	friend class CmdParser;
protected:
	CmdArgType _type;
	unsigned int _options;
	std::string _description;
	std::string _fullArg;
	std::string _singleCharArg;
	bool _isSet;
	CmdArgBase(CmdArgType t):_type(t){}
};


template<typename T>
class CmdArg : CmdArgBase{
public:
	CmdArg(const T& default_value);
	~CmdArg()override{}
	const T& getValue()const{return _value;}
	
	friend class CmdParser;
private:
	T _value;
};


class CmdParser{
public:
	enum Option{NONE=0x0, IS_FILE=0x1, IS_HELP=0x2, REQUIRED=0x4};
	CmdParser(){}
	~CmdParser();

	/* add commandline argument to list of valid arguments
	 * supported types: bool, int, float, std::string
	 */
	template <typename T>
	const CmdArg<T>* addArg(const std::string& full_arg, char single_letter_arg,
	                const std::string& description, const T default_value, unsigned int options = 0)
	{
		// create new argument with default value
		CmdArg<T>* a = new CmdArg<T>(default_value);
		// add internally
		addArgBase((CmdArgBase*)a, full_arg, single_letter_arg, description, options);
		return a;
	}

	/* add --help and -h arguments, has to be called after every other argument has been added */
	void addHelp();

	enum Result{OK, HELP, ERROR};
	/* parse commandline arguments
	 * @return OK, HELP or ERROR. HELP is returned, if argument -h or --help was given
	 */
	Result parse(int argc, char ** argv);

	/* get error if parsing failed */
	const std::string & getError(){return _lastError;}

	/* get string representing help */
	std::string getHelpString();
private:
	void addArgBase(CmdArgBase* base, const std::string& full_arg, char single_letter_arg,
	                 const std::string& description, unsigned int options);
	std::string _lastError; 
	typedef std::unordered_map<std::string, CmdArgBase&> MapType;
	std::vector<CmdArgBase*> _argList;// list of all arguments, each element is unique
	MapType _argMap;// mapping argument-name (e.g. '--help') text to argument, there maybe multiple keys referring to the same argument
};

#endif
