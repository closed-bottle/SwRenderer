#pragma once
#include <cmath>
template<typename T>
struct Vector3D
{
	T x = static_cast<T>(0);
	T y = static_cast<T>(0);
	T z = static_cast<T>(0);


	Vector3D(T _x, T _y, T _z);
	Vector3D<T> operator*(float& _rhs);
	Vector3D<T> operator/(float& _rhs);
	Vector3D<T> operator+(Vector3D<T>& _rhs);
	Vector3D<T> operator-(Vector3D<T>& _rhs);
	Vector3D<T>& operator/=(float& _rhs);
	Vector3D<T>& operator-=(Vector3D<T>& _rhs);
	Vector3D<T>& Normalize();
	float GetLength();
	static Vector3D<T> CrossProduct(Vector3D<T>& _lhs, Vector3D<T>& _rhs);
	static T DotProduct(Vector3D<T>& _lhs, Vector3D<T>& _rhs);
};


template<typename T>
inline Vector3D<T>::Vector3D(T _x, T _y,T _z)
{
	x = _x;
	y = _y;
	z = _z;
}

template<typename T>
inline Vector3D<T> Vector3D<T>::operator*(float& _rhs)
{
	return Vector3D<T>(x * _rhs, y * _rhs, z * _rhs);
}

template<typename T>
inline Vector3D<T> Vector3D<T>::operator/(float& _rhs)
{
	return Vector3D<T>(x / _rhs, y / _rhs, z / _rhs);
}

template<typename T>
inline Vector3D<T> Vector3D<T>::operator+(Vector3D<T>& _rhs)
{
	return Vector3D<T>(x + _rhs.x, y + _rhs.y, z + _rhs.z);
}

template<typename T>
inline Vector3D<T> Vector3D<T>::operator-(Vector3D<T>& _rhs)
{
	return Vector3D<T>(x - _rhs.x, y - _rhs.y, z - _rhs.z);
}

template<typename T>
inline Vector3D<T>& Vector3D<T>::operator/=(float& _rhs)
{
	x /= _rhs;
	y /= _rhs;
	z /= _rhs;

	return *this;
}

template<typename T>
inline Vector3D<T>& Vector3D<T>::operator-=(Vector3D<T>& _rhs)
{
	x -= _rhs.x;
	y -= _rhs.y;
	z -= _rhs.z;

	return *this;
}

template<typename T>
inline Vector3D<T>& Vector3D<T>::Normalize()
{
	auto length = GetLength();
	*this /= length;
	
	return *this;
}

template<typename T>
inline float Vector3D<T>::GetLength()
{
	return std::sqrt((x * x) + (y * y) + (z * z));
}

template<typename T>
inline Vector3D<T> Vector3D<T>::CrossProduct(Vector3D<T>& _lhs, Vector3D<T>& _rhs)
{
	return Vector3D<T>((_lhs.y * _rhs.z) - (_lhs.z * _rhs.y),
			(_lhs.z * _rhs.x) - (_lhs.x * _rhs.z), 
			(_lhs.x * _rhs.y) - (_lhs.y * _rhs.x));
}

template<typename T>
inline T Vector3D<T>::DotProduct(Vector3D<T>& _lhs, Vector3D<T>& _rhs)
{
	return (_lhs.x * _rhs.x) + (_lhs.y * _rhs.y) + (_lhs.z * _rhs.z);
}