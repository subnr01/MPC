//#include "stdafx.h"
#include "IntImage.h"
#include <iostream>

void copyArrays(int** src, int** dest, int np, int nc)
{
	for(int c = 0; c < nc; ++c)
	{
		for(int p = 0; p < np; ++p)
			dest[c][p] = src[c][p];
	}
}

void copy3ItoU_v2(int* s1, int* s2, int* s3, IplImage* dest)
{	
	int heightd     = dest->height;
	int widthd      = dest->width;
	int stepd       = dest->widthStep/sizeof(uchar);
	int channelsd   = dest->nChannels;
	uchar * datad    = (uchar *)dest->imageData;

	int dpos, spos;

	for(int y = 0; y < heightd; ++y)
	{
		for(int x = 0; x < widthd; ++x)
		{
			dpos = y*stepd + x*channelsd;
			spos = y*widthd + x;
			datad[dpos]   = s1[spos];
			datad[dpos+1] = s2[spos];
			datad[dpos+2] = s3[spos];
		}
	}
}

void copyUto3I_v2(IplImage* src, int* d1, int* d2, int* d3)
{
	int height     = src->height;
	int width      = src->width;
	int step       = src->widthStep/sizeof(uchar);
	int channels   = src->nChannels;
	uchar* data    = (uchar *)src->imageData;

	int dpos, spos;

	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			dpos = ((height - 1) - y)*width+x;
			//dpos = y*width + x;
			spos = y*step+x*channels;
			d1[dpos] = data[spos];
			d2[dpos] = data[spos+1];
			d3[dpos] = data[spos+2];
		}
	}
}

IntImage::IntImage(int width, int height, int nChannels, int** channels)
{
	_width = width;
	_height = height;
	_numChannels = nChannels;
	_channels = channels;
}

IntImage::IntImage(int width, int height, int channels)
{
	_width = width;
	_height = height;
	_numChannels = channels;
	prepareArrays();
}

IntImage::IntImage(IntImage& src)
{
	_width = src._width;
	_height = src._height;
	_numChannels = src._numChannels;
	prepareArrays();
	copyArrays(src._channels, _channels, _width * _height, _numChannels);
}

IntImage::IntImage(IplImage* src)
{
	_width = src->width;
	_height = src->height;
	_numChannels = 3;
	prepareArrays();
	copyUto3I_v2(src, _channels[0], _channels[1], _channels[2]);
}

IntImage::~IntImage()
{
	if(_width > 0 || _height > 0 || _numChannels > 0 || _channels)
	{
		for(int i = 0; i < _numChannels; ++i)
			delete[] _channels[i];
	}
	if(_channels)
		delete[] _channels;
}

int IntImage::width()
{
	return _width;
}

int IntImage::height()
{
	return _height;
}

int IntImage::nChannels()
{
	return _numChannels;
}

int* IntImage::getChannel(int n)
{
	if(n >= 0 && n < _numChannels)
		return _channels[n];
	else
		return NULL;
}

int** IntImage::getAllChannels()
{
	return _channels;
}

void IntImage::resize(int width, int height, int nChannels)
{
	if(_width > 0 || _height > 0 || _numChannels > 0 || _channels)
	{
		for(int i = 0; i < _numChannels; ++i)
			delete[] _channels[i];
	}
	if(_channels)
		delete[] _channels;

	_width = width;
	_height = height;
	_numChannels = nChannels;
	prepareArrays();
}

int IntImage::copy(IntImage& src, bool allowResize)
{
	if(src._width != _width || src._height != _height || src._numChannels != _numChannels)
	{
		if(!allowResize)
			return -1;

		resize(src._width, src._height, src._numChannels);
	}

	copyArrays(src._channels, _channels, _width * _height, _numChannels);
	return 0;
}

int IntImage::copy(IplImage* src, bool allowResize)
{
	if(!src)
		return -1;

	if(src->width <= 0 || src->height <= 0)
	{
		std::cout << "What in the hell?\n";
		return -1;
	}

	if(src->width != _width || src->height != _height || src->nChannels != _numChannels)
	{
		if(!allowResize)
			return -1;

		std::cout << "Resizing to " << src->width << " " << src->height << " " << src->nChannels << std::endl;
		resize(src->width, src->height, src->nChannels);
	}

	copyUto3I_v2(src, _channels[0], _channels[1], _channels[2]);
	return 0;
}

void IntImage::copyChannel(int* src, int chanDest)
{
	for(int i = 0; i < _width * _height; ++i)
		_channels[chanDest][i] = src[i];
}

void IntImage::fill(int val)
{
	for(int c = 0; c < _numChannels; ++c)
		for(int i = 0; i < _width * _height; ++i)
			_channels[c][i] = val;
}

IplImage* IntImage::getIplImage(IplImage* dest)
{
	if(dest == NULL)
	{
		dest = cvCreateImage(cvSize(_width, _height), IPL_DEPTH_8U, 3);
	}

	if(_numChannels < 3)
		copy3ItoU_v2(_channels[0], _channels[0], _channels[0], dest);
	else
		copy3ItoU_v2(_channels[0], _channels[1], _channels[2], dest);

	return dest;
}

void IntImage::prepareArrays()
{
	_channels = new int*[_numChannels];
	for(int i = 0; i < _numChannels; ++i)
		_channels[i] = new int[_width * _height];
}
