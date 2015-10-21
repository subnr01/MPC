#ifndef _PK_SEGMENT_MATCH_
#define _PK_SEGMENT_MATCH_

struct SegmentFrame
{
	int* segments;
	int* segmentSizes;
};

struct DLoc
{
	short x;
	short y;
};

struct TemplateDiff
{
	DLoc* pEdge;
	DLoc* nEdge;
	int pEdgeSize;
	int nEdgeSize;
};

class SegmentMatch
{
private:
	SegmentFrame* frameBuf;
	int** templateBuf;
	TemplateDiff* templateDiffs;
	int imWidth, imHeight;
	int templateWidth, templateHeight, templateFrames;
	int* histogram;
	float* totalDist;
	int* curFrameDist;
	float* normalizationTable;
	float* preComputedFactorsTable;
	int* inTable;
	bool distCalculated;
	int tMass;

	void buildNormTable();
	void clearInTable();

	void initBuffers();
	void clearHistogram();
	void clearDistance();
	int reinitializeDistance(int x, int y, int f, float& denom);
	void calculateTemplateDiffs();
	void calculateAndSumRowDistance(int y, int f, int fskip = 1);
	void calculateDistance(int fskip = 1);
	void precomputeNormFactors(int f);
public:
	SegmentMatch(int videoWidth, int videoHeight);
	~SegmentMatch();
	void setTemplate(int** src, int tWidth, int tHeight, int tFrames);
	void pushFrame(int* segments, int* segmentSizes);
	float* getDistance(int fskip = 1);
	int getMass(){ return tMass; }

	// this is just a convenience method
	float* doSegmentMatch(int* segments, int* segmentSizes, int fskip = 1)
	{
		pushFrame(segments, segmentSizes);
		return getDistance(fskip);
	}
};

#endif

