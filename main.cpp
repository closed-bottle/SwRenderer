#pragma pack(push,1) // for tight pack for struct.
#include "imageFormats/TGAImage.h"
#include "geometryFormats/OBJGeometry.h"
#include <string>

constexpr int width = 3000;
constexpr int height = 2000;


int main(int argc, const char* argv[])
{

	/*TGA_Image image_wire(width, height, 3);
	TGA_Image image_random(width, height, 3);
	TGA_Image image_flat(width, height, 3);*/
	TGA_Image image_light(width, height, 3);

	OBJ_Geometry obj;
	obj.SetZoom(1000.0f);
	obj.SetWidthHeight(width, height);
	obj.LoadFromOBJFile("suzanne.obj");
	/*

	obj.DrawWireframe(image_wire, Color(0, 255, 0, 255));

	image_wire.FlipVertically();
	image_wire.SaveToTGAFile("suzanne_wireframe.tga", true);


	obj.DrawWithRandomColor(image_random);

	image_random.FlipVertically();
	image_random.SaveToTGAFile("suzanne_random.tga", true);

	
	obj.DrawWithFlatColor(image_flat, Color(0, 255, 0, 255));

	image_flat.FlipVertically();
	image_flat.SaveToTGAFile("suzanne_flat.tga", true);

	*/
	obj.DrawWithFlatLight(image_light);

	image_light.FlipVertically();
	image_light.SaveToTGAFile("suzanne_light.tga", true);

	

	//getchar();
	return 0;
}