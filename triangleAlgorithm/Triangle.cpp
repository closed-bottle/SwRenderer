#include <algorithm>
#include "Triangle.h"
#include "../lineAlgorithm/LineRasterizeAlgorithm.h"

void Triangle::IdentifyTopMiddleBottom(Vec2i const & _a, Vec2i const & _b, Vec2i const & _c)
{
	auto top_y = std::max(std::max(_a.y, _b.y), std::max(_b.y, _c.y));

	if (top_y == _a.y)
	{
		top_ = _a;

		auto middle_y = std::max(_b.y, _c.y);
		if (middle_y == _b.y)
		{
			middle_ = _b;
			bottom_ = _c;
		}
		else if (middle_y == _c.y)
		{
			middle_ = _c;
			bottom_ = _b;
		}
	}
	else if (top_y == _b.y)
	{
		top_ = _b;

		auto middle_y = std::max(_a.y, _c.y);
		if (middle_y == _a.y)
		{
			middle_ = _a;
			bottom_ = _c;
		}
		else if (middle_y == _c.y)
		{
			middle_ = _c;
			bottom_ = _a;
		}
	}
	else if (top_y == _c.y)
	{
		top_ = _c;

		auto middle_y = std::max(_a.y, _b.y);
		if (middle_y == _a.y)
		{
			middle_ = _a;
			bottom_ = _b;
		}
		else if (middle_y == _b.y)
		{
			middle_ = _b;
			bottom_ = _a;
		}
	}
}

void Triangle::IdentifyLeftRight()
{
	if (middle_.x < bottom_.x)
	{
		left_ = middle_;
		right_ = bottom_;
	}
	else
	{
		left_ = bottom_;
		right_ = middle_;
	}
		
}

Triangle::Triangle(Vec2i const& _a, Vec2i const& _b, Vec2i const& _c)
	: top_(0, 0), middle_(0, 0), bottom_(0, 0), left_(middle_), right_(bottom_)
{
	IdentifyTopMiddleBottom(_a, _b, _c);


}

void Triangle::TriangleRasterize(TGA_Image& _image, Triangle& _triangle, Color& _color)
{
	// TODO : Optimizing. 
	// Some pixel dulplicated.
	Line topToMid(_triangle.top_, _triangle.middle_);
	Line topToBot(_triangle.top_, _triangle.bottom_);
	Line midToBot(_triangle.middle_, _triangle.bottom_);

	float left_x = static_cast<float>(topToMid.GetStartPoint().x);
	float right_x = static_cast<float>(topToBot.GetStartPoint().x);
	float left_slope = topToMid.GetSlope();
	float right_slope = topToBot.GetSlope();
	int y = _triangle.top_.y;

	if (topToMid.GetEndPoint().x < topToBot.GetEndPoint().x)
		_triangle.triangle_type_ = TriangleType::Left;
	else
	{
		_triangle.triangle_type_ = TriangleType::Right;
		left_x = static_cast<float>(topToBot.GetStartPoint().x);
		right_x = static_cast<float>(topToMid.GetStartPoint().x);
		left_slope = topToBot.GetSlope();
		right_slope = topToMid.GetSlope();
	}


	float left_step = -1.0f / left_slope;
	float right_step = -1.0f / right_slope;

	// Top half
	_image.SetPixel(_triangle.top_.x, _triangle.top_.y, _color);
	while (--y >= _triangle.middle_.y)
	{
		left_x += left_step;
		right_x += right_step;
		for(int x = static_cast<int>(left_x); x <= right_x; x++)
		_image.SetPixel(x, y, _color);
	}



	if (_triangle.triangle_type_ == TriangleType::Left)
	{
		left_x = static_cast<float>(midToBot.GetStartPoint().x);
		left_slope = midToBot.GetSlope();
		left_step = -1.0f / left_slope;
	}
	else
	{
		right_x = static_cast<float>(midToBot.GetStartPoint().x);
		right_slope = midToBot.GetSlope();
		right_step = -1.0f / right_slope;
	}

	left_x += left_step;
	right_x += right_step;
	for (int x = static_cast<int>(left_x); x <= right_x; x++)
		_image.SetPixel(x, y, _color);
	// Bottom half
	while (--y >= _triangle.bottom_.y)
	{
		left_x += left_step;
		right_x += right_step;

		for (int x = static_cast<int>(left_x); x <= right_x; x++)
			_image.SetPixel(x, y, _color);
	}
}
