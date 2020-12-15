#pragma once

class Stream
{
private:
	Stream* next;
};



template<typename R>
class Supplyer
{
	virtual const R operator()() = 0;
};

template<typename T>
class Consumer
{
	virtual const void operator(T value)() = 0;
};

template<typename R, typename T>
class Function
{
	virtual const R operator(T value)() = 0;
};

class Callable
{
	virtual const void operator()() = 0;
};

template<typename R, typename T1, typename T2>
class BiFunction
{
	virtual const R operator(T1 value1, T2 value2)() = 0;
};
