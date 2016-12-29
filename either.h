#pragma once
#ifndef EITHER_H
#define EITHER_H

#include <type_traits>
#include <algorithm>
#include <assert.h>
#include <memory>

enum type
{
	LEFT,

	RIGHT,

	LEFT_HEAP,

	RIGHT_HEAP
};

type getRealType(const type& t)
{
	if (t == LEFT_HEAP)
		return LEFT;
	else if (t == RIGHT_HEAP)
		return RIGHT;
	else return t;
}

type getHeapType(const type& t) // t == LEFT or RIGHT
{
	if (t == LEFT)
		return LEFT_HEAP;
	else 
		return RIGHT_HEAP;
}

bool isHeap(const type& t)
{
	return t == LEFT_HEAP || t == RIGHT_HEAP;
}

struct emplace_left_t {};
static emplace_left_t emplace_left;

struct emplace_right_t {};
static emplace_right_t emplace_right;

template <typename Left, typename Right>
struct either
{
	either() = delete;
	
	either(Left left) : which(LEFT)
	{
		new (&data) Left(std::move(left));
	};

	either(Right right) : which(RIGHT)
	{
		new (&data) Right(std::move(right));
	};

	either(either const& other) : which(getRealType(other.which))
	{
		if (other.isLeft())
			new (&data) Left(other.left());
		else
			new (&data) Right(other.right());
	};

	either(either && other) : which(getRealType(other.which))
	{
		if (other.isLeft())
			new (&data) Left(std::move(other.left()));
		else
			new (&data) Right(std::move(other.right()));
	};

	~either()
	{
		clear();
	};

	either& operator=(either other)
	{
		if (other.isLeft())
			emplace(emplace_left, other.left());
		else
			emplace(emplace_right, other.right());
		return *this;
	};

	bool isLeft() const
	{
		return getRealType(which) == LEFT;
	};

	Left& left()
	{
		assert(isLeft());
		if (which == LEFT)
			return *reinterpret_cast<Left*>(&data);
		return **reinterpret_cast<std::unique_ptr<Left>*>(&data);
	};

	Left const& left() const
	{
		assert(isLeft());
		if (which == LEFT)
			return *reinterpret_cast<Left const*>(&data);
		return **reinterpret_cast<std::unique_ptr<const Left> const*>(&data);
	};

	bool isRight() const
	{
		return getRealType(which) == RIGHT;
	};

	Right& right()
	{
		assert(isRight());
		if (which == RIGHT)
			return *reinterpret_cast<Right*>(&data);
		return **reinterpret_cast<std::unique_ptr<Right>*>(&data);
	};

	Right const& right() const
	{
		assert(isRight());
		if (which == RIGHT)
			return *reinterpret_cast<Right const*>(&data);
		return **reinterpret_cast<std::unique_ptr<const Right> const*>(&data);
	};

	template <typename ... Args>
	void emplace(emplace_left_t, Args&& ... args)
	{
		if (std::is_nothrow_constructible<Left, Args...>::value) 
		{
			emplace<Left>(std::forward<Args>(args)...);
			which = LEFT;
		}
		else if (which == LEFT) 
			emplaceWithHeap<Left>(left(), LEFT, args...);
		else
			emplaceWithHeap<Left>(right(), LEFT, args...);
	};

	template <typename ... Args>
	void emplace(emplace_right_t, Args&& ... args)
	{
		if (std::is_nothrow_constructible<Right, Args...>::value) 
		{
			emplace<Right>(std::forward<Args>(args)...);
			which = RIGHT;
		}
		else if (which == LEFT) 
		{
			emplaceWithHeap<Right>(left(), RIGHT, std::forward<Args>(args)...);
		}
		else 
		{
			emplaceWithHeap<Right>(right(), RIGHT, std::forward<Args>(args)...);
		}
	};

	template <typename ... Args>
	either(emplace_left_t, Args&& ... args) : which(LEFT) 
	{
		new (&data) Left(std::forward<Args>(args)...);
	};

	template <typename ... Args>
	either(emplace_right_t, Args&& ... args) : which(RIGHT)
	{
		new (&data) Right(std::forward<Args>(args)...);
	};

private:
	type which;
	typedef typename std::aligned_storage < std::max(sizeof(Left), std::max(sizeof(Right), sizeof(std::unique_ptr<Left>))),
		std::max(alignof(Left), std::max(alignof(Right), alignof(std::unique_ptr<Left>))) > ::type data_type;
	data_type data;

	template <typename Left, typename Right, typename Lhs, typename Rhs>
	friend void swapper(either<Left, Right>& a, either<Left, Right>& b,
						std::unique_ptr<Lhs> left_data, std::unique_ptr<Rhs> right_data,
						type const& left_type, type const& right_type);

	template<typename Type, typename... Args>
	void emplace(Args&&... args)
	{
		clear();
		new (&data) Type(std::forward<Args>(args)...);
	}

	template<typename NewType, typename Type, typename... Args>
	void emplaceWithHeap(Type old, type newType, Args&&... args) 
	{
		auto tmp = std::make_unique<Type>(old);
		try {
			clear();
			new (&data) NewType(std::forward<Args>(args)...);
			which = newType;
		}
		catch (...) 
		{
			which = getHeapType(newType);
			new (&data) std::unique_ptr<Type>(tmp.release());
			throw;
		}
	}

	void clear()
	{
		if (which == LEFT)
			reinterpret_cast<Left*>(&data)->~Left();
		else if (which == RIGHT)
		{
			reinterpret_cast<Right*>(&data)->~Right();
		}
		else if (which == LEFT_HEAP)
		{
			reinterpret_cast<std::unique_ptr<Left>*>(&data)->~unique_ptr();
		}
		else
		{
			reinterpret_cast<std::unique_ptr<Right>*>(&data)->~unique_ptr();
		}
	};
};

template <typename F, typename Left, typename Right>
auto apply(F const& f, either<Left, Right> const& e)
{
	if (e.isLeft())
		return f(e.left());
	else
		return f(e.right());
};

template <typename F, typename Left, typename Right>
auto apply(F const& f, either<Left, Right>& e)
{
	if (e.isLeft())
		return f(e.left());
	else
		return f(e.right());
};

template <typename Left, typename Right>
void swap(either<Left, Right>& a, either<Left, Right>& b)
{
	if (a.isLeft() && b.isLeft())
		swapper(a, b, std::make_unique<Left>(a.left()), std::make_unique<Left>(b.left()), LEFT, LEFT);
	else if (a.isRight() && b.isLeft())
		swapper(a, b, std::make_unique<Right>(a.right()), std::make_unique<Left>(b.left()), RIGHT, LEFT);
	else if (a.isLeft() && b.isRight())
		swapper(a, b, std::make_unique<Left>(a.left()), std::make_unique<Right>(b.right()), LEFT, RIGHT);
	else
		swapper(a, b, std::make_unique<Right>(a.right()), std::make_unique<Right>(b.right()), RIGHT, RIGHT);
};

template <typename Left, typename Right, typename Lhs, typename Rhs>
void swapper(either<Left, Right>& a, either<Left, Right>& b,
			std::unique_ptr<Lhs> left_data, std::unique_ptr<Rhs> right_data,
			type const& left_type, type const& right_type)
{
	try 
	{
		a.clear();
		new (&a.data) Rhs(*right_data);
		a.which = right_type;
		b.clear();
		new (&b.data) Lhs(*left_data);
		b.which = left_type;
	}
	catch (...) 
	{
		a.which = getHeapType(left_type);
		b.which = getHeapType(right_type);
		new (&a.data) std::unique_ptr<Lhs>(left_data.release());
		new (&b.data) std::unique_ptr<Rhs>(right_data.release());
		throw;
	}
};

#endif EITHER_H