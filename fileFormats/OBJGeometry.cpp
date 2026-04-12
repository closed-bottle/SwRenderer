#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <random>
#include <utility>
#include <algorithm>
#include "OBJGeometry.h"

void OBJGeometry::SetWidth(float _new_width)
{
	width_ = _new_width;
}

void OBJGeometry::SetHeight(float _new_height)
{
	height_ = _new_height;
}

void OBJGeometry::SetWidthHeight(float _new_width, float _new_height)
{
	SetWidth(_new_width);
	SetHeight(_new_height);
}

void OBJGeometry::SetZoom(float _new_zoom)
{
	zoom_ = _new_zoom;
}

OBJGeometry::OBJGeometry(String _filename)
{
	LoadFromOBJFile(_filename);
}

OBJGeometry::OBJGeometry()
{
}

bool OBJGeometry::LoadFromOBJFile(String _filename)
{
	std::ifstream input_file(_filename.c_str());



	input_file.close();


	return true;
}