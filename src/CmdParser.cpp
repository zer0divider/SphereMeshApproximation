#include "CmdParser.h"

CmdParser::~CmdParser()
{
	for(auto i : _argList){
		delete(i);
	}
}

void CmdParser::addHelp()
{
	addArg<bool>("help", 'h', "Show help.", false, CmdArgBase::IS_HELP);
}

void CmdParser::addArgBase(CmdArgBase* base, const std::string& full_arg, char single_letter_arg,
                        const std::string& description, unsigned int options)
{

		base->_description = description;
		base->_options = options;
		base->_fullArg = "--" + full_arg;
		if(single_letter_arg != '\0'){
			base->_singleCharArg = std::string("-") + single_letter_arg;
		}
		// insert arg into map
		_argMap.insert(MapType::value_type(base->_fullArg, *base));
		_argMap.insert(MapType::value_type(base->_singleCharArg, *base));
		_argList.push_back(base);
}

CmdParser::Result CmdParser::parse(int argc, char ** argv)
{
	int i = 1;
	while(i < argc){
		std::string a = std::string(argv[i]);
		auto iter = _argMap.find(a);
		if(iter != _argMap.end()){
			const CmdArgBase * base = &(*iter).second;
			std::string value;
			std::string err_for_arg = std::string(" for argument '") + a + std::string("'.");
			if(i < argc-1){
				value = argv[i+1];
			}
			else if(base->getType() != CmdArgType::BOOL){
				_lastError = std::string("No value given") + err_for_arg;
				return ERROR;
			}
			switch(base->getType()){
			case CmdArgType::INT:{
				try{
					auto int_arg = (CmdArg<int>*)(base);
					int_arg->_value = std::stoi(value);
				}catch(const std::invalid_argument&){
					_lastError = std::string("Unable to convert '")+value+std::string("' to int") + err_for_arg;
					return ERROR;
				}catch(const std::out_of_range&){
					_lastError = std::string("Value '")+value+std::string("' is out of range") + err_for_arg;
					return ERROR;
				}
				i++;
			}break;
			case CmdArgType::FLOAT:{
				auto float_arg = (CmdArg<float>*)(base);
				try{
					float_arg->_value = std::stod(value);
				}catch(const std::invalid_argument&){
					_lastError = std::string("Unable to convert '")+value+std::string("' to float") + err_for_arg;
					return ERROR;
				}catch(const std::out_of_range&){
					_lastError = std::string("Value '")+value+std::string("' is out of range") + err_for_arg;
					return ERROR;
				}
				i++;
			}break;
			case CmdArgType::BOOL:{
				auto bool_arg = (CmdArg<bool>*)(base);
				bool_arg->_value = true;
			}break;
			case CmdArgType::STRING:{
				auto string_arg = (CmdArg<std::string>*)(base);
				string_arg->_value = value;
				i++;
			}break;
			default:{
				assert(false);
			}
			}
			if(base->getOptions()&CmdArgBase::IS_HELP){
				return HELP;
			}
		}
		else{ // argument not found
			_lastError = std::string("Unknown argument '") + a + std::string("'.");
			return ERROR;
		}
		i++;
	}
	return OK;
}

std::string CmdParser::getHelpString()
{
	struct StringPair{
		void set(const std::string & a, const std::string & d){
			arg = a;
			desc = d;
		}
		std::string arg;
		std::string desc;
	};
	std::vector<StringPair> arg_strings(_argList.size());
	int max_arg_len = 0;
	int count = 0;
	for(const CmdArgBase*i : _argList){
		std::string s = "  ";
		if(!i->getShortArg().empty()){
			s +=  i->getShortArg() + std::string(", ");
		}
		s += i->getFullArg();
		std::string type;
		switch(i->getType()){
		case CmdArgType::INT:{
			type = "<integer>";
		}break;
		case CmdArgType::FLOAT:{
			type = "<float>";
		}break;
		case CmdArgType::STRING:{
			if(i->getOptions() & CmdArgBase::IS_FILE){
				type = "<file>";
			}
			else{
				type = "<string>";
			}
		}break;
		}
		s += " " + type + "    ";
		int s_len = s.length();
		if(s_len > max_arg_len){
			max_arg_len = s_len;	
		}
		arg_strings[count].set(s, i->getDescription());
		count++;
	}

	std::string helper_string = "Commandline options:\n";
	for(const StringPair& s : arg_strings){
		helper_string += s.arg;
		for(int i = 0; i < max_arg_len - s.arg.length(); i++){
			helper_string += " ";
		}
		helper_string += s.desc + "\n";
	}

	return helper_string;
}

template<>
CmdArg<int>::CmdArg(const int& default_value):CmdArgBase(CmdArgType::INT), _value(default_value){}

template<>
CmdArg<float>::CmdArg(const float& default_value):CmdArgBase(CmdArgType::FLOAT), _value(default_value){}

template<>
CmdArg<bool>::CmdArg(const bool& default_value):CmdArgBase(CmdArgType::BOOL), _value(default_value){}

template<>
CmdArg<std::string>::CmdArg(const std::string& default_value):CmdArgBase(CmdArgType::STRING), _value(default_value){}
