#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include "OBJGeometry.h"
#include "../lineAlgorithm/LineRasterizeAlgorithm.h"

void OBJ_Geometry::SetWidth(float _new_width)
{
	width_ = _new_width;
}

void OBJ_Geometry::SetHeight(float _new_height)
{
	height_ = _new_height;
}

void OBJ_Geometry::SetWidthHeight(float _new_width, float _new_height)
{
	SetWidth(_new_width);
	SetHeight(_new_height);
}

void OBJ_Geometry::SetZoom(float _new_zoom)
{
	zoom_ = _new_zoom;
}

OBJ_Geometry::OBJ_Geometry(std::string _filename)
{
	LoadFromOBJFile(_filename);
}

OBJ_Geometry::OBJ_Geometry()
{
}

bool OBJ_Geometry::LoadFromOBJFile(std::string _filename)
{
	std::ifstream input_file(_filename);

	if (!input_file.is_open())
	{
		std::cerr << "Can't open file : " << _filename.c_str() << "\n";
		input_file.close();
		return false;
	}
	
	std::string buffer;
	while (getline(input_file, buffer))
	{
		// If line is comment, skip.
		if ((*std::begin(buffer)) == '#')
			continue;

		std::stringstream buffer_stream(buffer);
		std::string stream_segment;
		while (buffer_stream >> stream_segment)
		{
			if (stream_segment == "mtllib") // Material library
			{
				buffer_stream >> stream_segment; // Assume that there are no space character in file name.
				material_library_name_ = stream_segment;
				// TODO : LoadFromMTLFile() or something.
				continue;
			}
			else if (stream_segment == "o") // Object name
			{
				buffer_stream >> stream_segment; // Assume that there are no space character in object name.
				// TODO
				continue;
			}
			else if (stream_segment == "v") // Geometric vertex
			{
				Vec3f vertex(0.0f, 0.0f, 0.0f);

				buffer_stream >> stream_segment;
				vertex.x = std::stof(stream_segment);

				buffer_stream >> stream_segment;
				vertex.y = std::stof(stream_segment);

				buffer_stream >> stream_segment;
				vertex.z = std::stof(stream_segment);

				geometric_vertices_.push_back(vertex * zoom_);
				
				continue;
			}
			else if (stream_segment == "vt") // Texture vertex
			{
				Vec3f vertex(0.0f, 0.0f, 0.0f);

				buffer_stream >> stream_segment;
				vertex.x = std::stof(stream_segment);

				buffer_stream >> stream_segment;
				vertex.y = std::stof(stream_segment);

				buffer_stream >> stream_segment;
				vertex.z = std::stof(stream_segment);

				texture_vertices_.push_back(vertex);

				continue;
			}
			else if (stream_segment == "vn") // Vertex normal
			{
				Vec3f vertex(0.0f, 0.0f, 0.0f);

				buffer_stream >> stream_segment;
				vertex.x = std::stof(stream_segment);

				buffer_stream >> stream_segment;
				vertex.y = std::stof(stream_segment);

				buffer_stream >> stream_segment;
				vertex.z = std::stof(stream_segment);

				vertex_normals_.push_back(vertex);

				continue;
			}
			else if (stream_segment == "f") // Vertex normal
			{
				float half_width = width_ / 2;
				float half_height = height_ / 2;
				// First line.
				buffer_stream >> stream_segment;
				Vec3f startVec3 = geometric_vertices_[std::stoi(stream_segment) - 1];
				const Vec3f veryFirstVec3 = startVec3;

				buffer_stream >> stream_segment;
				Vec3f endVec3 = geometric_vertices_[std::stoi(stream_segment) - 1];

				wire_frame_.push_back(Line(Vec2i(static_cast<int>(startVec3.x + half_width), static_cast<int>(startVec3.y + half_height)), 
										   Vec2i(static_cast<int>(endVec3.x + half_width), static_cast<int>(endVec3.y + half_height))));

				while (buffer_stream >> stream_segment)
				{
					startVec3 = endVec3;

					endVec3 = geometric_vertices_[std::stoi(stream_segment) - 1];

					wire_frame_.push_back(Line(Vec2i(static_cast<int>(startVec3.x + half_width), static_cast<int>(startVec3.y + half_height)),
											   Vec2i(static_cast<int>(endVec3.x + half_width), static_cast<int>(endVec3.y + half_height))));
				}


				wire_frame_.push_back(Line(Vec2i(static_cast<int>(endVec3.x + half_width), static_cast<int>(endVec3.y + half_height)), 
										   Vec2i(static_cast<int>(veryFirstVec3.x + half_width), static_cast<int>(veryFirstVec3.y + half_height))));

				continue;
			}
		}
			
	}
	


	return true;
}

bool OBJ_Geometry::DrawWireframe(TGA_Image & _image)
{
	int i = 0;
	const auto maximum = wire_frame_.size();
	for (auto line : wire_frame_)
	{
		Bresenhams::LineRasterize(_image, line, Color(255, 0, 0, 255));
		std::cout << i++ << " of " << maximum << " lines." << std::endl;
	}
		

	return true;
}
