#pragma once

#include <iostream>
#include <variant> // variant, visit
#include <vector>
#include <queue>

// ** Overload template for retrieving "std::variant"s ** //

//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class ...Fs>
struct overload : Fs... {
	template <class ...Ts>
	overload(Ts&& ...ts) : Fs{ std::forward<Ts>(ts) }...
	{}

	using Fs::operator()...;
};

template <class ...Ts>
overload(Ts&&...)->overload<std::remove_reference_t<Ts>...>;


// ** Coordinate ** //

struct Coord
{
	double x;
	double y;
};
