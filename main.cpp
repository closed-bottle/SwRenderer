#pragma pack(push,1) // for tight pack for struct.
#include "imageFormats/TGAImage.h"
#include "geometryFormats/OBJGeometry.h"
#include <string>
int main(int argc, const char* argv[])
{
	TGA_Image image(3000, 2000, 3);

	OBJ_Geometry obj;
	obj.SetZoom(1000.0f);
	obj.SetWidthHeight(image.GetWidth(), image.GetHeight());
	obj.LoadFromOBJFile("suzanne.obj");
	obj.DrawWireframe(image);
	

	image.FlipVertically();
	image.SaveToTGAFile("suzanne.tga", true);
	
	//getchar();
	return 0;
}