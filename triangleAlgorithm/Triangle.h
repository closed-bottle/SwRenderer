#pragma once
#include "../Math/Vector.h"
#include "../ImageFormats/TGAImage.h"


class Triangle
{
	Vec2i top_;
	Vec2i middle_;
	Vec2i bottom_;
	Vec2i& left_;
	Vec2i& right_;
	
	enum TriangleType
	{
		Left,
		Right
	};
	TriangleType triangle_type_;

	void IdentifyTopMiddleBottom(Vec2i const& _a, Vec2i const& _b, Vec2i const& _c);
	void IdentifyLeftRight();

	

public:
	

	Triangle(Vec2i const& _a, Vec2i const& _b, Vec2i const& _c);
	static void TriangleRasterize(TGA_Image& _image, Triangle& _triangle, Color& _color);
};