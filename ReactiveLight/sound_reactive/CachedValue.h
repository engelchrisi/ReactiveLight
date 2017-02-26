/*
 * CacheValue.h
 *
 * Created: 24.02.2017 08:26:58
 *  Author: D031288
 */ 


#ifndef CACHEVALUE_H_
#define CACHEVALUE_H_


/*
	Keeps a cached value and executes a read function every TIME_SPAN_MS
*/
template<typename T, unsigned long TIME_SPAN_MS>
class CachedValue
{
	typedef T readFunctionT(void);

	T				_Value;
	unsigned long	_nextReadMs;
	readFunctionT*	_func;
	
	public:
	CachedValue(readFunctionT func)
	{
		_func= func;
		_Value= _func();
		_nextReadMs= 0;
		readValue();
	}
	
	T readValue()
	{
		unsigned long currentMs= millis();
		
		if (currentMs >= _nextReadMs)
		{
			_Value= _func();
			_nextReadMs= currentMs + TIME_SPAN_MS;
		}
		
		return _Value;
	}
};


#endif /* CACHEVALUE_H_ */