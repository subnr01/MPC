#include "StatMerge.h"
#include <math.h>
#include <iostream>

StatMerge::StatMerge(int imWidth, int imHeight) : uf(imWidth * imHeight), iWidth(imWidth), iHeight(imHeight)
{
	edgeArray = new ImageEdge[imWidth * imHeight * 2];
	statsArray = new StatCell[imWidth * imHeight];
	retArray = new int[imWidth * imHeight];
	sizeArray = new int[imWidth * imHeight];

	old1 = new int[imWidth * imHeight];
	old2 = new int[imWidth * imHeight];
	old3 = new int[imWidth * imHeight];

	changeMask = new char[imWidth * imHeight];
	changeT = -1; //100; // change threshold (-1 means just consider everything to have changed every frame)

	// initialize the old images to black and every pixel in its own old group
	for(int i = 0; i < imWidth * imHeight; ++i)
	{
		old1[i] = 0;
		old2[i] = 0;
		old3[i] = 0;
		retArray[i] = i;
	}
	maxGroup = imWidth * imHeight;
}
	
StatMerge::~StatMerge()
{
	delete[] edgeArray;
	delete[] statsArray;
	delete[] retArray;
}

double findVal(int* ch1, int* ch2, int* ch3, int p1, int p2)
{
	double d1 = ch1[p1] - ch1[p2];
	double d2 = ch2[p1] - ch2[p2];
	double d3 = ch3[p1] - ch3[p2];

	return (d1*d1) + (d2*d2) + (d3*d3);
}

void StatMerge::resetStats(int* ch1, int* ch2, int* ch3)
{
	for(int i = 0; i < iWidth * iHeight; ++i)
	{
		// set new means
		statsArray[i].N = 1;
		statsArray[i].mu_a = (double)ch1[i];
		statsArray[i].mu_b = (double)ch2[i];
		statsArray[i].mu_c = (double)ch3[i];

		// calculate change mask, make unchanged pixels inherit their old group
		int d1 = ch1[i] - old1[i];
		int d2 = ch2[i] - old2[i];
		int d3 = ch3[i] - old3[i];

		int dd = (d1*d1) + (d2*d2) + (d3*d3);
		if(dd > changeT)
		{
			changeMask[i] = 1;
			statsArray[i].G = 0; // no group
		}
		else
		{
			changeMask[i] = 0;
			statsArray[i].G = 0; // no group
			//statsArray[i].G = retArray[i]; // old group
		}

		// update the old images
		// IMPORTANT: THIS HAS TO HAPPEN AFTER THE CHANGES ARE CALCULATED!
		old1[i] = ch1[i];
		old2[i] = ch2[i];
		old3[i] = ch3[i];
	}
}

bool shouldMerge(StatCell &sc1, StatCell &sc2, double Qg)
{
	double max_mu = std::max(abs(sc1.mu_a-sc2.mu_a), std::max(abs(sc1.mu_b-sc2.mu_b),abs(sc1.mu_c-sc2.mu_c)));
	return max_mu <= Qg * sqrt((double)(sc1.N + sc2.N) / (double)(sc1.N * sc2.N));
}

int* StatMerge::doSegmentation(int* ch1, int* ch2, int* ch3, double p)
{
	// first, build the edge array
	for(int x = 0; x < iWidth - 1; ++x)
	{
		for(int y = 0; y < iHeight - 1; ++y)
		{
			int p1 = (y * iWidth) + x;
			int p2 = p1 + 1;
			int p3 = p1 + iWidth;

			// vertical edge
			edgeArray[p1 * 2].p1 = p1;
			edgeArray[p1 * 2].p2 = p2;
			edgeArray[p1 * 2].val = findVal(ch1,ch2,ch3,p1,p2);

			// horizontal edge
			edgeArray[(p1 * 2)+1].p1 = p1;
			edgeArray[(p1 * 2)+1].p2 = p3;
			edgeArray[(p1 * 2)+1].val = findVal(ch1,ch2,ch3,p1,p3);
		}
	}

	// we have to handle the very bottom and right differently
	int y = iHeight - 1;
	int x = 0;
	for(x = 0; x < iWidth - 1; ++x)
	{
		int p1 = (y * iWidth) + x;
		int p2 = p1 + 1;

		// vertical edge
		edgeArray[p1 * 2].p1 = p1;
		edgeArray[p1 * 2].p2 = p2;
		edgeArray[p1 * 2].val = findVal(ch1,ch2,ch3,p1,p2);
	}

	y = 0;
	x = iWidth - 1;
	for(y = 0; y < iHeight - 1; ++y)
	{
		int p1 = (y * iWidth) + x;
		int p2 = p1 + iWidth;

		// horizontal edge
		edgeArray[(p1 * 2)+1].p1 = p1;
		edgeArray[(p1 * 2)+1].p2 = p2;
		edgeArray[(p1 * 2)+1].val = findVal(ch1,ch2,ch3,p1,p2);
	}

	// now sort the edges (probably the most expensive part of this entire operation).
	std::sort(edgeArray, edgeArray + (iWidth * iHeight * 2));

	// now let us prepare for the merging!
	uf.reset();
	resetStats(ch1,ch2,ch3);

	double Qg = 255.0 * sqrt(0.5 * log(2.0 / p));

	int p1, p2;
	int p1r, p2r;

	// first, note what our old maximum group number was
	int oldCutoff = maxGroup;
	++maxGroup;

	// first merge: merge unchanged pixels
	for(int i = 0; i < (iWidth*iHeight*2); ++i)
	{
		p1 = edgeArray[i].p1;
		p2 = edgeArray[i].p2;

		if(p1 < 0 || p2 < 0)
			continue;

		// don't merge if one of us has changed or if we're in different groups
		if(changeMask[p1] || changeMask[p2] || (retArray[p1] != retArray[p2]))
			continue;

		//std::cout << "I should never be called!\n";

		// otherwise, merge
		p1r = uf.Find(p1);
		p2r = uf.Find(p2);

		uf.UFUnion(p1r, p2r);
		//statsArray[p1r].merge(statsArray[p2r]);

		// cheat a bit to make sure new groups are formed
		statsArray[p1r].merge(statsArray[p2r], -1, maxGroup);
	}

	// second merge: merge everything again according to statistical rules
	//std::cout << "MaxGroup: " << maxGroup << std::endl;

	
	for(int i = 0; i < (iWidth*iHeight*2); ++i)
	{
		p1 = edgeArray[i].p1;
		p2 = edgeArray[i].p2;

		if(p1 < 0 || p2 < 0)
			continue;

		p1r = uf.Find(p1);
		p2r = uf.Find(p2);

		// (curLabels[p1]==curLabels[p2])
		// if we aren't already merged and the stats indicate we should, merge
		if(p1r != p2r && (false || shouldMerge(statsArray[p1r], statsArray[p2r], Qg)))
		{
			uf.UFUnion(p1r, p2r);

			// this merge call makes sure that new groups merge into old ones and that
			// new groups are properly created
			statsArray[p1r].merge(statsArray[p2r], oldCutoff, maxGroup);
		}
	}

	// now to fill in the return array thingy...
	int curR;
	int curSize;
	for(int curP = 0; curP < iWidth * iHeight; ++curP)
	{
		curR = uf.Find(curP);
		//curG = statsArray[curR].G;
		//retArray[curP] = changeMask[curR];
		curSize = statsArray[curR].N;
		sizeArray[curP] = curSize;
		retArray[curP] = curR;
	}

	return retArray;
}

void StatMerge::doAvgColorThing(int* ch1, int* ch2, int* ch3)
{
	for(int i = 0; i < iWidth * iHeight; ++i)
	{
		int curR = retArray[i];
		ch1[i] = (int)(statsArray[curR].mu_a);
		ch2[i] = (int)(statsArray[curR].mu_b);
		ch3[i] = (int)(statsArray[curR].mu_c);
	}
}

void StatMerge::doAvgColorThingWithLines(int* ch1, int* ch2, int* ch3)
{
	for(int i = 0; i < iWidth * iHeight - 1; ++i)
	{
		int curR = retArray[i];
		int downR = 0;
		if(i+iWidth < iWidth * iHeight)
			downR = retArray[i+iWidth];
		int rightR = retArray[i+1];

		if(curR != downR || curR != rightR)
		{
			ch1[i] = 255;
			ch2[i] = 255;
			ch3[i] = 255;
		}
		else
		{
			ch1[i] = (int)(statsArray[curR].mu_a);
			ch2[i] = (int)(statsArray[curR].mu_b);
			ch3[i] = (int)(statsArray[curR].mu_c);
		}
	}
}

void StatMerge::doLinesOnly(int* ch1, int* ch2, int* ch3)
{
	for(int i = 0; i < iWidth * iHeight - 1; ++i)
	{
		int curR = retArray[i];
		int downR = 0;
		if(i+iWidth < iWidth * iHeight)
			downR = retArray[i+iWidth];
		int rightR = retArray[i+1];

		if(curR != downR || curR != rightR)
		{
			ch1[i] = 255;
			ch2[i] = 255;
			ch3[i] = 255;
		}
		else
		{
			ch1[i] = 0;
			ch2[i] = 0;
			ch3[i] = 0;
		}
	}
}

void StatMerge::dumpComponents(std::vector< std::vector<unsigned int> > &comps, int level)
{
	// This is rough and tumble cowboy code! It doesn't care about anyone or anything!
	// Least of all efficiency!
	// It's the wild west and it's every code snipped for itself!
	// You gotta grab the heap by the horns!

	// Watch me allocate 25,000+ vectors!
	std::vector<unsigned int>* compArray = new std::vector<unsigned int>[iWidth * iHeight];

	// We need to calculate this offset for this crazy file format!
	int offset = level * iWidth * iHeight;

	// start adding things! Yeehaw!
	for(int i = 0; i < iWidth * iHeight; ++i)
	{
		int curCent = retArray[i];
		compArray[curCent].push_back(i + offset);
	}

	// start adding comps! Yeehaw!
	// Yes, this is using the copy constructor to unnecessarily copy vectors!
	// Go STL!
	for(int i = 0; i < iWidth * iHeight; ++i)
	{
		if(compArray[i].size() > 0)
			comps.push_back(compArray[i]);
	}

	delete[] compArray;
}
