#pragma once
#ifndef BIND_H
#define BIND_H

#include <functional>

template <int N>
struct placeholder {};

placeholder<0> _1;
placeholder<1> _2;
placeholder<2> _3;
placeholder<3> _4;
placeholder<4> _5;
placeholder<5> _6;
placeholder<6> _7;
placeholder<7> _8;
placeholder<8> _9;
placeholder<9> _10;

template <typename F, typename ... Arguments>
struct bind_t
{
	template <typename ... Args>
	auto operator()(Args&& ... args) const
	{
		return call(typename holder<std::tuple_size<bind_tuple>::value>::type(), std::forward<Args>(args) ...);
	}

private:
	template <typename F, typename ... Args>
	friend bind_t<F, Args ...> bind(F&& f, Args&& ... args);

	typedef std::tuple<Arguments ...> bind_tuple;

	F function;
	bind_tuple arguments;

	bind_t(F&& f, Arguments&& ... args) : function(std::forward<F>(f)), arguments(std::forward<Arguments>(args) ...) {};

	template <typename Arg, typename ... Args>
	auto&& get_arg(const Arg& arg, Args& ... args) const
	{
		return arg;
	}

	template <typename Func, typename ... FArgs, typename ... Args>
	auto get_arg(const bind_t<Func, FArgs ...>& f, Args& ... args) const
	{
		return f(args ...);
	}

	template <int N, typename ... Args>
	auto&& get_arg(const placeholder<N>&, Args& ... args) const
	{
		return std::get<N>(std::forward_as_tuple(args ...));
	}

	template <int ... Sz>
	struct counter
	{};

	template <int Sz, int ... N>
	struct holder
	{
		typedef typename holder<Sz - 1, Sz - 1, N ...>::type type;
	};

	template <int ... N>
	struct holder<0, N ...>
	{
		typedef counter<N ...> type;
	};

	template <typename ... Args, int ... Sz>
	auto call(const counter<Sz ...>&, Args&& ... args) const
	{
		return function(get_arg(std::get<Sz>(arguments), args ...) ...);
	}
};

template <typename F, typename ... Args>
bind_t<F, Args ...> bind(F&& f, Args&& ... args)
{
	return bind_t<F, Args ...>(std::forward<F>(f), std::forward<Args>(args) ...);
};

#endif //BIND_H