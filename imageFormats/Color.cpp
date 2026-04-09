#include "Color.h"

Color::Color()
	: red(static_cast<unsigned char>(0)), green(static_cast<unsigned char>(0)),
	  blue(static_cast<unsigned char>(0)), alpha(static_cast<unsigned char>(255))
{}

Color::Color(int _red, int _green, int _blue, int _alpha)
	: red(static_cast<unsigned char>(_red)), green(static_cast<unsigned char>(_green)),
	  blue(static_cast<unsigned char>(_blue)), alpha(static_cast<unsigned char>(_alpha)) {}

bool Color::operator==(Color _input)
{
	return (_input.red != red || _input.green != green || _input.blue != blue || _input.alpha != alpha);
}

bool Color::operator!=(Color _input)
{
	return !(*this == _input);
}
