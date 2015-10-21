#include "FloatImage.h"
#define NULL 0

FloatImage::FloatImage()
{
	width = -1;
	height = -1;
	data = NULL;
}

FloatImage::FloatImage(int nwidth, int nheight)
{
	width = nwidth;
	height = nheight;
	data = new float[width * height];
}

FloatImage::FloatImage(FloatImage& src)
{
	width = src.width;
	height = src.height;
	int numP = width * height;
	if(src.data != NULL)
	{
		data = new float[width * height];
		for(int i = 0; i < numP; ++i)
			data[i] = src.data[i];
	}
	else
		data = NULL;
}

FloatImage::~FloatImage()
{
	if(data)
		delete[] data;
	data = NULL;
}

void FloatImage::copy(FloatImage* src)
{
	float* srcData = src->data;
	for(int i = 0; i < width * height; ++i)
		data[i] = srcData[i];
}