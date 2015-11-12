#include "LinearShapeMatch.h"
#include <algorithm>
#include <iostream>

LinearShapeMatch::LinearShapeMatch(int vidWidth, int vidHeight, int bufFrames, int maxSegs)
{
	_width = vidWidth;
	_height = vidHeight;
	_nframes = bufFrames;
	if(maxSegs <= 0)
		_maxSegments = _width * _height;
	else
		_maxSegments = maxSegs;
	_mstorage = new int[_maxSegments];
	_dstorage = new float[_maxSegments];

	// prepopulate the buffer
	for(int f = 0; f < _nframes; ++f)
	{
		_bufFrames.push_back(new IntImage(_width, _height, 2));
		_bufFrames[f]->fill(0);
	} 

	_retImage = new FloatImage(_width, _height);
	_twidth = -1;
	_theight = -1;
	_tframes = -1;

	_fill = 0.0f;
	_usingFill = false;
	_using3dSegmentation = false;

	std::cout << "MasSegs: " << _maxSegments << std::endl;
}

LinearShapeMatch::~LinearShapeMatch()
{
	delete[] _mstorage;
	delete[] _dstorage;

	for(int f = 0; f < _bufFrames.size(); ++f)
		delete _bufFrames[f];

	delete _retImage;

	for(int f = 0; f < _templateFrames.size(); ++f)
		delete _templateFrames[f];
}

void LinearShapeMatch::setTemplate(std::vector<FloatImage*>& templateFrames)
{
	for(int f = 0; f < _templateFrames.size(); ++f)
		delete _templateFrames[f];

	_templateFrames.clear();

	for(int f = 0; f < templateFrames.size(); ++f)
		_templateFrames.push_back(templateFrames[f]);
}

void LinearShapeMatch::pushFrame(IntImage* srcImage)
{
	IntImage* temp = _bufFrames[0];
	temp->copy(*srcImage, true);

	// rotate the buffer by one
	for(int i = 0; i < _bufFrames.size() - 1; ++i)
		_bufFrames[i] = _bufFrames[i+1];

	_bufFrames[_bufFrames.size()-1] = temp;
}

void LinearShapeMatch::setFrame(IntImage* srcImage, int bufPos)
{
	_bufFrames[bufPos]->copy(*srcImage, true);
}

int LinearShapeMatch::getBufferSize()
{
	return _bufFrames.size();
}

void LinearShapeMatch::setFill(float fill, bool enabled)
{
	_fill = fill;
	_usingFill = (_fill != 0.0f) && enabled; 
}

void LinearShapeMatch::set3d(bool enabled)
{
	_using3dSegmentation = enabled;
}

FloatImage* LinearShapeMatch::doMatching(std::vector<FloatImage*>* tFrames, int* attentionMask)
{
	if(tFrames == NULL)
		tFrames = &_templateFrames;

	_twidth = (*tFrames)[0]->width;
	_theight = (*tFrames)[0]->height;

	int cFrames = std::min(tFrames->size(), _bufFrames.size());
	float sum = 0.0f;

	clearSegments();
	for(int y = 0; y < _height - _theight + 1; ++y)
	{
		for(int x = 0; x < _width - _twidth + 1; ++x)
		{
			if(attentionMask != NULL && attentionMask[(y * _width) + x] <= 0)
			{
				_retImage->data[(y * _width) + x] = 10000.0f;
				continue;
			}

			/*sum = 0.0f;
			//clearSegments();
			for(int f = 0; f < cFrames; ++f)
			{
				//if(!_using3dSegmentation)
				//	clearSegments();
				calcSinglePosDist(x, y, f, tFrames);
				if(!_using3dSegmentation)
					sum += processSegments(f);
			}
			if(_using3dSegmentation)
				sum = processSegments(cFrames - 1);*/

			sum = reallyCalcSinglePos(cFrames, x, y, tFrames);

			_retImage->data[(y * _width) + x] = sum;
			//if(sum < 0.0f)
			//	std::cout << "sum: " << sum << std::endl;
		}
	}

	return _retImage;
}

FloatImage* LinearShapeMatch::getResult()
{
	return _retImage;
}

float LinearShapeMatch::doSinglePointMatching(int x, int y, std::vector<FloatImage*>* tFrames)
{
	if(tFrames == NULL)
		tFrames = &_templateFrames;

	_twidth = (*tFrames)[0]->width;
	_theight = (*tFrames)[0]->height;

	int cFrames = std::min(tFrames->size(), _bufFrames.size());

	//std::cout << "I'm doing it with " << cFrames << std::endl;

	clearSegments();

	return reallyCalcSinglePos(cFrames, x, y, tFrames);
}

void LinearShapeMatch::clearSegments()
{
	// this could be sped up by only clearing the livesegmentslist segments, since
	// they are the only ones that should have been modified
	for(int p = 0; p < _maxSegments; ++p)
	{
		_dstorage[p] = 0.0f;
		_mstorage[p] = 0;
	}
	_liveSegmentList.clear();
}

float LinearShapeMatch::reallyCalcSinglePos(int cFrames, int x, int y, std::vector<FloatImage*>* tFrames)
{
	float sum = 0.0f;
	//clearSegments();
	for(int f = 0; f < cFrames; ++f)
	{
		//if(!_using3dSegmentation)
		//	clearSegments();
		calcSinglePosDist(x, y, f, tFrames);
		if(!_using3dSegmentation)
			sum += processSegments(f);
	}
	if(_using3dSegmentation)
		sum = processSegments(cFrames - 1);

	return sum;
}

float LinearShapeMatch::calcSinglePosDist(int x, int y, int f, std::vector<FloatImage*>* tFrames)
{
	//std::cout << "x: " << x << ", y: " << y << std::endl;

	int left = std::max(x, 0);
	int top = std::max(y, 0);
	int right = std::min(x + _twidth, _width);
	int bottom = std::min(y + _theight, _height);

	int cwidth = std::min(_twidth, right - left);
	int cheight = std::min(_theight, bottom - top);

	//std::cout << "cwidth: " << cwidth << ", cheight: " << cheight << std::endl;

	int* segs = _bufFrames[f]->getChannel(0);
	float* temp = (*tFrames)[f]->data;

	int p_seg = (top * _width) + left;
	int p_temp = 0;

	for(int cy = 0; cy < cheight; ++cy)
	{
		for(int cx = 0; cx < cwidth; ++cx)
		{
			//std::cout << "cx: " << cx << ", cy: " << cy << std::endl;
			//std::cout << "p_seg: " << p_seg << std::endl;
			//std::cout << "segs[p_seg]: " << segs[p_seg] << std::endl;
			int segnum = segs[p_seg];
			//if(segnum != 0)
			//	std::cout << segnum << std::endl;
			/*if(segnum >= _maxSegments)
			{
				std::cout << "Bad segment: " << segnum << std::endl;
				std::cout << "At: " << p_seg << std::endl;
				std::cout << "cx: " << cx << ", cy: " << cy << std::endl;
				std::cout << "x: " << x << ", y: " << y << std::endl;
				continue;
			}*/

			if(_mstorage[segnum] == 0)
			{
				/*if(segnum != 0)
				{
					std::cout << "PUshing back " << segnum << std::endl;
					std::cout << "LSL size " << _liveSegmentList.size() << std::endl;
				}*/
				_liveSegmentList.push_back(segnum);
				
			}

			_mstorage[segnum] += 1;
			_dstorage[segnum] += temp[p_temp];
			//std::cout << temp[p_temp] << " foo\n";

			p_seg += 1;
			p_temp += 1;
		}
		p_seg = ((top + cy) * _width) + left;
		p_temp = (cy * _twidth);
	}

	return 0.0f;
}

float LinearShapeMatch::processSegments(int f)
{
	float sum = 0.0f;
	float tempSum = 0.0f;
	int* segSizes = _bufFrames[f]->getChannel(1);
	int segnum;

	if(!_usingFill)
	{
		//std::cout << _liveSegmentList.size() << " asdf\n";
		for(int i = 0; i < _liveSegmentList.size(); ++i)
		{
			//std::cout << _dstorage[_liveSegmentList[i]] << " foo\n";
			if(_dstorage[_liveSegmentList[i]] < 0.0)
				sum += _dstorage[_liveSegmentList[i]];
			_dstorage[_liveSegmentList[i]] = 0.0;
			_mstorage[_liveSegmentList[i]] = 0;
		}
	}
	else
	{
		for(int i = 0; i < _liveSegmentList.size(); ++i)
		{
			segnum = _liveSegmentList[i];
			tempSum = _dstorage[segnum] + _fill * (float)(segSizes[segnum] - _mstorage[segnum]);
			//float fillComp = _fill * (float)(segSizes[segnum] - _mstorage[segnum]);
			//float dcomp = _dstorage[segnum];
			//float ss = segSizes[segnum];
			//float ms = _mstorage[segnum];
			/*if(segnum != 0)
			{
				std::cout << "Segnum: " << segnum << std::endl;
				std::cout << "d: " << dcomp << ", fill: " << fillComp << ", total: " << tempSum << std::endl;
				std::cout << "ss: " << ss << ", ms: " << ms << std::endl;
			}*/
			if(tempSum < 0.0)
				sum += tempSum;
			_dstorage[_liveSegmentList[i]] = 0.0;
			_mstorage[_liveSegmentList[i]] = 0;
		}
	}
	_liveSegmentList.clear();

	//std::cout << "Sum: " << sum << std::endl;
	return sum;
}
