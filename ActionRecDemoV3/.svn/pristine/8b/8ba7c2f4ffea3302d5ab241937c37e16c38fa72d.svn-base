#ifndef LINEAR_SHAPE_MATCH
#define LINEAR_SHAPE_MATCH

#include <vector>
#include "FloatImage.h"
#include "IntImage.h"

class LinearShapeMatch
{
public:
	LinearShapeMatch(int vidWidth, int vidHeight, int bufFrames, int maxSegs = -1);
	~LinearShapeMatch();

	void setTemplate(std::vector<FloatImage*>& templateFrames);
	void pushFrame(IntImage* srcImage);
	void setFrame(IntImage* srcImage, int bufPos);
	int getBufferSize();
	void setFill(float fill, bool enabled = true);
	void set3d(bool enabled);

	FloatImage* doMatching(std::vector<FloatImage*>* tFrames = NULL, int* attentionMask = NULL);
	FloatImage* getResult();

	float doSinglePointMatching(int x, int y, std::vector<FloatImage*>* tFrames = NULL);
private:
	int _width;
	int _height;
	int _nframes;
	int* _mstorage;
	float* _dstorage;
	int _maxSegments;
	std::vector<int> _liveSegmentList;
	std::vector<IntImage*> _bufFrames;
	FloatImage* _retImage;
	int _twidth;
	int _theight;
	int _tframes;
	std::vector<FloatImage*> _templateFrames;
	float _fill;
	bool _usingFill;
	bool _using3dSegmentation;

	void clearSegments();
	float reallyCalcSinglePos(int cFrames, int x, int y, std::vector<FloatImage*>* tFrames);
	float calcSinglePosDist(int x, int y, int f, std::vector<FloatImage*>* tFrames);
	float processSegments(int f);
};

#endif

