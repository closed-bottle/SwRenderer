#pragma once
#include "../Math/Vector.h"

class Line
{
	Vec2i start_;
	Vec2i end_;
	int dx_;
	int dy_;
public:
	Line(Vec2i const& _start, Vec2i const& _end);

	const Vec2i& GetStartPoint() const;
	const Vec2i& GetEndPoint() const;
	Vec2i& GetStartPoint();
	Vec2i& GetEndPoint();
	int GetDx() const;
	int GetDy() const;
	float GetSlope();
};
