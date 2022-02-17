#ifndef MODE_SWITCHER_H
#define MODE_SWITCHER_H

template <class T>
class ModeSwitcher
{
public:
	ModeSwitcher(T initial, int max): _mode(initial), _max(max){}

	T next(){
		_mode = (T)(((int)_mode)+1);
		if((int)_mode >= _max){
			_mode = (T)0;
		}
		return _mode;
	}

	T get(){
		return _mode;
	}

	bool operator==(T m){
		return m == _mode;
	}

	void set(T m){
		_mode = m;
	}

private:
	T _mode;
	int _max;
};

#endif
