#include "SegmentMatch.h"
#include <vector>
#include <iostream>

const int NORMALIZING = 0;

void SegmentMatch::buildNormTable()
{
	// base cases (zero case should never be used, but whatever)
	normalizationTable[0] = 0.0f;
	normalizationTable[1] = 0.5f;
	normalizationTable[2] = 0.5f;

	double last1 = 0.5;
	double last2 = 0.5;

	for(int i = 1; i < (imWidth * imHeight) + 1; ++i)
	{
		normalizationTable[i] = (float)i / 2.0f;
	}

	for(int i = 3; i < (imWidth * imHeight); i += 2)
	{
		// i should be odd
		// i + 1 should be even
		last1 = ((double)(i) / (double)(i - 1)) * last1;
		last2 = ((double)(i) / (double)(i - 1)) * last2;
		normalizationTable[i] = ((float)i / 2.0f) - last1;
		normalizationTable[i+1] = ((float)(i+1) / 2.0f) - last2;

		if((i % 1000) == 1)
		{
			std::cout << "Table[" << i << "]: " << normalizationTable[i] << std::endl;
			std::cout << "Table[" << i+1 << "]: " << normalizationTable[i+1] << std::endl;
		}

		if(normalizationTable[i] < 0 || normalizationTable[i+1] < 0)
		{
			std::cout << "WTF?????" << std::endl;
			std::cout << "AT THINGY " << i << std::endl;
			break;
		}
	}

	std::cout << "Table[500]: " << normalizationTable[500] << std::endl;
	std::cout << "Table[250]: " << normalizationTable[250] << std::endl;
}

void SegmentMatch::clearInTable()
{
	for(int i = 0; i < imWidth * imHeight; ++i)
		inTable[i] = 0;
}

void SegmentMatch::initBuffers()
{
	// I guess this doesn't have anything to do now, since setTemplate absorbed all of its functionality
}

void SegmentMatch::clearHistogram()
{
	for(int i = 0; i < imWidth * imHeight; ++i)
		histogram[i] = 0;
}

void SegmentMatch::calculateAndSumRowDistance(int y, int f, int fskip)
{
	if(f * fskip >= templateFrames)
		return;

	TemplateDiff td = templateDiffs[f * fskip];
	int offset = y * imWidth;
	SegmentFrame frame = frameBuf[f];
	int* segs = frame.segments;
	int* segSizes = frame.segmentSizes;
	int localDist = 0;
	float denom = 0.0;

	// first, initialize the histogram and local distance to our starting point...
	localDist = reinitializeDistance(0, y, f, denom);

	if(denom < 0.0)
		std::cout << "NEGATIVE DENOMINATOR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

	denom += 0.01;

	// update global dist
	if(NORMALIZING)
		totalDist[offset + 0] += ((float)localDist / denom);
	else
		totalDist[offset + 0] += localDist;

	// now iterate through the row
	for(int xi = 0; xi < imWidth - templateWidth; ++xi)
	{
		//localDist = reinitializeDistance(xi, y, f, denom);
		
		int curX, curY, curPos, curVal;

		
		// n-edge
		for(int i = 0; i < td.nEdgeSize; ++i)
		{
			curX = td.nEdge[i].x + xi;
			curY = td.nEdge[i].y + y;
			curPos = (curY * imWidth) + curX;

			curVal = segs[curPos];
			--(histogram[curVal]);
			// check to see if we are over the half-mark
			// (there is a slight bug here in that a segment with
			//  an odd size that is completely within the template will
			//  have a distance of -1)
			if(histogram[curVal] < segSizes[curVal] / 2)
				--localDist;
			else if(histogram[curVal] == (segSizes[curVal] / 2))
			{
				if(denom < preComputedFactorsTable[curVal] && denom > 0.0f)
				{
					std::cout << "Denominator dropped below zero: " << denom << std::endl;
					std::cout << "Curval: " << preComputedFactorsTable[curVal] << std::endl;
				}
				if(inTable[curVal])
				{
					denom -= preComputedFactorsTable[curVal];
					inTable[curVal] = 0;
				}
				--localDist;
			}
			else
				++localDist;
		}

		// p-edge
		for(int i = 0; i < td.pEdgeSize; ++i)
		{
			curX = td.pEdge[i].x + xi;
			curY = td.pEdge[i].y + y;
			curPos = (curY * imWidth) + curX;

			curVal = segs[curPos];
			++(histogram[curVal]);
			// check to see if we are over the half-mark
			// (there is a slight bug here in that a segment with
			//  an odd size that is completely within the template will
			//  have a distance of -1)
			if(histogram[curVal] <= segSizes[curVal] / 2)
				++localDist;
			else if(!inTable[curVal] && histogram[curVal] == (segSizes[curVal] / 2) + 1)
			{
				denom += preComputedFactorsTable[curVal];
				inTable[curVal] = 1;
				--localDist;
			}
			else
				--localDist;
		}

		// update global dist
		if(NORMALIZING)
			totalDist[offset + xi + 1] += ((float)localDist / denom);
		else
			totalDist[offset + xi + 1] += localDist;
		//totalDist[offset + xi + 1] += (1.0f / denom);
	}
}

int SegmentMatch::reinitializeDistance(int x, int y, int f, float& denom)
{
	//std::cout << "X: " << x << ", Y: " << y << ", F: " << f << std::endl;

	int* curTemplateFrame = templateBuf[f];
	SegmentFrame frame = frameBuf[f];
	int* segs = frame.segments;
	int* segSizes = frame.segmentSizes;

	int dist = 0;

	// reset the histogram
	for(int i = 0; i < imWidth*imHeight; ++i)
		histogram[i] = 0;

	// reset the inTable
	clearInTable();

	int curVal;
	int curPos;
	int tPos;
	denom = 0.0f;
	for(int yi = 0; yi < templateHeight; ++yi)
	{
		for(int xi = 0; xi < templateWidth; ++xi)
		{
			curPos = ((yi+y) * imWidth) + (xi+x);
			tPos = (yi * templateWidth) + xi;
			curVal = segs[curPos];

			if(curTemplateFrame[tPos] <= 0)
				continue;

			//std::cout << "yi: " << yi << std::endl;
			//std::cout << "xi: " << xi << std::endl;
			//std::cout << "curVal: " << curVal << std::endl;

			++(histogram[curVal]);
			// check to see if we are over the half-mark
			// (there is a slight bug here in that a segment with
			//  an odd size that is completely within the template will
			//  have a distance of -1)
			if(histogram[curVal] <= segSizes[curVal] / 2)
				++dist;
			else if(histogram[curVal] == (segSizes[curVal] / 2) + 1 && !inTable[curVal])
			{
				denom += preComputedFactorsTable[curVal];
				inTable[curVal] = 1;
				--dist;
			}
			else
				--dist;
		}
	}

	if(denom == 0.0f)
		denom = 0.5;

	//denom = 1.0f;

	return dist;
}

void SegmentMatch::calculateTemplateDiffs()
{
	// this use of STL is OK since it's only done once during initialization
	std::vector<DLoc> nedge;
	std::vector<DLoc> pedge;

	for(int f = 0; f < templateFrames; ++f)
	{
		nedge.clear();
		pedge.clear();

		for(int y = 0; y < templateHeight; ++y)
		{
			for(int x = 0; x < templateWidth; ++x)
			{
				// nedge check
				if(		((x-1)<0 && templateBuf[f][(y*templateWidth)+x]==1) 
					||	((x-1) >= 0 && templateBuf[f][(y*templateWidth)+(x-1)] == 0 && templateBuf[f][(y*templateWidth)+x]==1))
				{
					DLoc temp;
					temp.x = x;
					temp.y = y;
					nedge.push_back(temp);
				}

				// pedge check
				if(((x+1)>=templateWidth && templateBuf[f][(y*templateWidth)+x]==1) ||
				   ((x+1)<templateWidth && templateBuf[f][(y*templateWidth)+x]==1 && templateBuf[f][(y*templateWidth)+(x+1)]==0))
				{
					DLoc temp;
					temp.x = x+1;
					temp.y = y;
					pedge.push_back(temp);
				}
			}
		}

		// now dump these edges into the actual structure
		templateDiffs[f].nEdgeSize = nedge.size();
		templateDiffs[f].pEdgeSize = pedge.size();
		std::cout << "Nedge size: " << nedge.size() << std::endl;
		std::cout << "Pedge size: " << pedge.size() << std::endl;
		templateDiffs[f].nEdge = new DLoc[nedge.size()];
		templateDiffs[f].pEdge = new DLoc[pedge.size()];

		for(int i = 0; i < nedge.size(); ++i)
			templateDiffs[f].nEdge[i] = nedge[i];
		for(int i = 0; i < pedge.size(); ++i)
			templateDiffs[f].pEdge[i] = pedge[i];
	}
}

void SegmentMatch::clearDistance()
{
	for(int i = 0; i < imWidth * imHeight; ++i)
		totalDist[i] = 0;
}



void SegmentMatch::calculateDistance(int fskip)
{
	clearDistance();

	// we do this by frames and rows (do frames in reverse order
	// so that the last frame pushed is always used in the calculation)
	//for(int f = templateFrames - 1; f >= 0; --f)
	for(int f = 0; f < templateFrames; ++f)
	{
		precomputeNormFactors(f);

		for(int yi = 0; yi <= imHeight - templateHeight; ++yi)
		{
			//std::cout << "Frame: " << f << ", Row: " << yi << std::endl;
			calculateAndSumRowDistance(yi, f, fskip);
		}
	}

	distCalculated = true;
}

void SegmentMatch::precomputeNormFactors(int f)
{
	SegmentFrame frame = frameBuf[f];
	int* segsizes = frame.segmentSizes;

	for(int i = 0; i < imWidth * imHeight; ++i)
	{
		preComputedFactorsTable[i] = normalizationTable[segsizes[i]];
		if(preComputedFactorsTable[i] < 0)
		{
			std::cout << "WTF????????????????????????????" << std::endl;
			std::cout << "i: " << i << std::endl;
			std::cout << "segSize: " << segsizes[i] << std::endl;
		}
	}
}

SegmentMatch::SegmentMatch(int videoWidth, int videoHeight)
{
	imWidth = videoWidth;
	imHeight = videoHeight;

	frameBuf = NULL;
	templateBuf = NULL;
	templateDiffs = NULL;
	templateWidth = -1;
	templateHeight = -1;
	templateFrames = -1;
	histogram = new int[videoWidth * videoHeight];
	totalDist = new float[videoWidth * videoHeight];
	curFrameDist = new int[videoWidth * videoHeight];
	normalizationTable = new float[(videoWidth * videoHeight) + 1];
	preComputedFactorsTable = new float[(videoWidth * videoHeight) + 1];
	inTable = new int[videoWidth * videoHeight];
	tMass = 0;

	std::cout << "Building norm table..." << std::endl;
	buildNormTable();
	std::cout << "Done!" << std::endl;

	distCalculated = false;
}

SegmentMatch::~SegmentMatch()
{
	delete[] inTable;

	delete[] histogram;
	delete[] totalDist;
	delete[] curFrameDist;

	delete[] normalizationTable;
	delete[] preComputedFactorsTable;

	//if(frameBuf)
	//	delete[] frameBuf;

	if(templateBuf && templateDiffs && frameBuf)
	{
		for(int i = 0; i < templateFrames; ++i)
		{
			delete[] templateBuf[i];

			delete[] templateDiffs[i].nEdge;
			delete[] templateDiffs[i].pEdge;

			delete[] frameBuf[i].segments;
			delete[] frameBuf[i].segmentSizes;
		}

		delete[] templateBuf;
		delete[] templateDiffs;
		delete[] frameBuf;
	}
}

void SegmentMatch::setTemplate(int** src, int tWidth, int tHeight, int tFrames)
{
	templateWidth = tWidth;
	templateHeight = tHeight;
	templateFrames = tFrames;

	templateBuf = new int*[templateFrames];
	templateDiffs = new TemplateDiff[templateFrames];
	frameBuf = new SegmentFrame[templateFrames];

	tMass = 0;

	for(int i = 0; i < templateFrames; ++i)
	{
		templateBuf[i] = new int[templateWidth * templateHeight];
		for(int j = 0; j < templateWidth * templateHeight; ++j)
		{
			templateBuf[i][j] = src[i][j];
			if(src[i][j] > 0)
				++tMass;
		}

		frameBuf[i].segments = new int[imWidth * imHeight];
		frameBuf[i].segmentSizes = new int[imWidth * imHeight];

		// we need to actually initialize these too...
		for(int z = 0; z < imWidth * imHeight; ++z)
		{
			frameBuf[i].segments[z] = 0;
			frameBuf[i].segmentSizes[z] = 0;
		}
	}

	calculateTemplateDiffs();
}

void SegmentMatch::pushFrame(int* segments, int* segmentSizes)
{
	SegmentFrame temp = frameBuf[0];

	// rotate the buffer by one
	for(int i = 0; i < templateFrames - 1; ++i)
		frameBuf[i] = frameBuf[i+1];

	frameBuf[templateFrames-1] = temp;

	// load in the newest frame
	for(int i = 0; i < imWidth * imHeight; ++i)
	{
		frameBuf[templateFrames-1].segments[i] = segments[i];
		frameBuf[templateFrames-1].segmentSizes[i] = segmentSizes[i];
	}

	// indicate that our previous distance calculation is now invalid
	distCalculated = false;
}

float* SegmentMatch::getDistance(int fskip)
{
	if(distCalculated)
		return totalDist;
	else
	{
		calculateDistance(fskip);
		return totalDist;
	}
}
