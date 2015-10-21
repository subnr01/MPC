#ifndef INT_IMAGE
#define INT_IMAGE

#include <opencv/cv.h>
#include <opencv/cxcore.h>

class IntImage
{
public:
	IntImage(int width, int height, int nChannels, int** channels);
	IntImage(int width, int height, int channels);
	IntImage(IntImage& src);
	IntImage(IplImage* src);
	~IntImage();
	int width();
	int height();
	int nChannels();
	int* getChannel(int n);
	int** getAllChannels();
	void resize(int width, int height, int nChannels);
	int copy(IntImage& src, bool allowResize = false);
	int copy(IplImage* src, bool allowResize = false);
	void copyChannel(int* src, int chanDest);
	void fill(int val = 0);
	IplImage* getIplImage(IplImage* dest = NULL);
private:
	int _width;
	int _height;
	int _numChannels;
	int** _channels;
	void prepareArrays();
};

#endif
