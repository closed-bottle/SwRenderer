#pragma once

template<typename T>
struct Vector2D
{
	T x = static_cast<T>(0);
	T y = static_cast<T>(0);

	Vector2D(T _x, T _y);
	Vector2D<T>& operator*(float& _mutiply);
	Vector2D<T>& operator+(Vector2D<T>& _rhs);
	Vector2D<T>& operator-(Vector2D<T>& _rhs);
	static T DotProduct(Vector2D<T>& _lhs, Vector2D<T>& _rhs);
};

template<typename T>
Vector2D<T>::Vector2D(T _x, T _y)
{
	x = _x;
	y = _y;
}

template<typename T>
Vector2D<T>& Vector2D<T>::operator*(float& _mutiply)
{
	x *= _mutiply;
	y *= _mutiply;

	return *this;
}

template<typename T>
inline Vector2D<T>& Vector2D<T>::operator+(Vector2D<T>& _rhs)
{
	x += _rhs.x;
	y += _rhs.y;

	return *this;
}

template<typename T>
inline Vector2D<T>& Vector2D<T>::operator-(Vector2D<T>& _rhs)
{
	x -= _rhs.x;
	y -= _rhs.y;

	return *this;
}

template<typename T>
inline T Vector2D<T>::DotProduct(Vector2D<T>& _lhs, Vector2D<T>& _rhs)
{
	return (_lhs.x * _rhs.x) + (_lhs.y * _rhs.y);
}
