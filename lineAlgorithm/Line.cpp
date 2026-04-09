#include "Line.h"

Line::Line(Vec2i const& _start, Vec2i const& _end)
	:start_(_start), end_(_end)
{
	dx_ = end_.x - start_.x;
	dy_ = end_.y - start_.y;
}
const Vec2i& Line::GetStartPoint() const
{
	return start_;
}

const Vec2i& Line::GetEndPoint() const
{
	return end_;
}

Vec2i& Line::GetStartPoint()
{
	return start_;
}

Vec2i& Line::GetEndPoint()
{
	return end_;
}

int Line::GetDx() const
{
	return dx_;
}

int Line::GetDy() const
{
	return dy_;
}

float Line::GetSlope()
{
	return static_cast<float>(dy_)/ static_cast<float>(dx_);
}
