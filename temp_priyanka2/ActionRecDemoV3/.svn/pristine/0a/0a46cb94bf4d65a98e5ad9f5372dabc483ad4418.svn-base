#ifndef _PK_STAT_MERGE_
#define _PK_STAT_MERGE_

#include <algorithm>
#include <vector>
#include "PKUnionFind.h"

class ImageEdge
{
public:
	int p1;
	int p2;
	double val;

	ImageEdge()
	{
		p1 = -1;
		p2 = -1;
		val = 1000000.0;
	}

	ImageEdge(int sp1, int sp2, double sval)
	{
		p1 = sp1;
		p2 = sp2;
		val = sval;
	}

	bool operator<(const ImageEdge &rhs) const
	{
		return val < rhs.val; 
	}
};

class StatCell
{
public:
	int N;
	int G;
	double mu_a;
	double mu_b;
	double mu_c;

	StatCell() : N(1), mu_a(0.0), mu_b(0.0), mu_c(0.0)
	{
		// do nothing additional
	}

	StatCell(double nma, double nmb, double nmc)
	{
		N = 1;
		G = -1;
		mu_a = nma;
		mu_b = nmb;
		mu_c = nmc;
	}

	void merge(StatCell &rhs)
	{
		int tN = N + rhs.N;
		double ta = (mu_a * N) + (rhs.mu_a * rhs.N);
		double tb = (mu_b * N) + (rhs.mu_b * rhs.N);
		double tc = (mu_c * N) + (rhs.mu_c * rhs.N);

		mu_a = rhs.mu_a = ta / tN;
		mu_b = rhs.mu_b = tb / tN;
		mu_c = rhs.mu_c = tc / tN;
		N = rhs.N = tN;
	}

	void merge(StatCell &rhs, int oldCutoff, int &nextGroup)
	{
		merge(rhs);
		if(G == 0 && rhs.G == 0) // get a new group...
		{
			G = nextGroup;
			rhs.G = nextGroup;
			++nextGroup;
		}
		else if(G == 0)
		{
			G = rhs.G;
		}
		else if(rhs.G == 0)
		{
			rhs.G = G;
		}
		else if(G < oldCutoff)
		{
			rhs.G = G;
		}
		else // this also includes the case where rhs.G > old && G > old
		{
			G = rhs.G;
		}
	}
};

class StatMerge
{
private:
	PKUnionFind uf;
	ImageEdge* edgeArray;
	StatCell* statsArray;
	int* retArray;
	int* sizeArray;
	int* old1;
	int* old2;
	int* old3;
	char* changeMask;
	int changeT;
	int iWidth;
	int iHeight;
	int maxGroup;
	void resetStats(int* ch1, int* ch2, int* ch3);
public:
	StatMerge(int imWidth, int imHeight);
	~StatMerge();
	int* doSegmentation(int* ch1, int* ch2, int* ch3, double p);
	int* getSizeArray(){ return sizeArray; }
	void doAvgColorThing(int* ch1, int* ch2, int* ch3);
	void doAvgColorThingWithLines(int* ch1, int* ch2, int* ch3);
	void doLinesOnly(int* ch1, int* ch2, int* ch3);
	void dumpComponents(std::vector< std::vector<unsigned int> > &comps, int level);
};

#endif // _PK_STAT_MERGE_

