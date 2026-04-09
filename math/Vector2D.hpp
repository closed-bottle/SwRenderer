#pragma once

template<typename T>
struct Vector2D
{
	T x = static_cast<T>(0);
	T y = static_cast<T>(0);

	Vector2D(T _x, T _y);
};

template<typename T>
Vector2D<T>::Vector2D(T _x, T _y)
{
	x = _x;
	y = _y;
}