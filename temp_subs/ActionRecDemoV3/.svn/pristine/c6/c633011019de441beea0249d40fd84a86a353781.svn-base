#ifndef _PK_UNION_FIND_
#define _PK_UNION_FIND_

class PKUnionFind
{
private:
	struct DataNode {
		int rank;
		int parent;
	};

	int numEntries;
	DataNode* entries;

	PKUnionFind(const PKUnionFind &u);
	PKUnionFind &operator =(const PKUnionFind &u);
public:
	PKUnionFind() : numEntries(1000)
	{
		entries = new DataNode[numEntries];
		for(int i = 0; i < numEntries; ++i)
		{
			entries[i].parent = i;
			entries[i].rank = 0;
		}
	}

	PKUnionFind(int isize)
	{
		numEntries = isize;
		entries = new DataNode[numEntries];
		for(int i = 0; i < numEntries; ++i)
		{
			entries[i].parent = i;
			entries[i].rank = 0;
		}
	}

	~PKUnionFind() 
	{
		delete[] entries;
	}

	int Find(int x)
	{
		int sy = x;

		while(entries[x].parent != x)
			x = entries[x].parent;

		int ly = sy;
		sy = entries[sy].parent;

		while(ly != sy)
		{
			entries[ly].parent = x;
			ly = sy;
			sy = entries[sy].parent;
		}

		return x;
	}

	void UFUnion(int x, int y)
	{
		int xRoot = Find(x);
		int yRoot = Find(y);
		if(entries[xRoot].rank > entries[yRoot].rank)
			entries[yRoot].parent = xRoot;
		else if(entries[xRoot].rank < entries[yRoot].rank)
			entries[xRoot].parent = yRoot;
		else if(xRoot != yRoot) // we shouldn't union the same thing to itself
		{
			entries[yRoot].parent = xRoot;
			entries[xRoot].rank += 1;
		}
	}

	void UFUnion(int x, int y, int &r1, int &r2)
	{
		int xRoot = Find(x);
		int yRoot = Find(y);
		r1 = xRoot;
		r2 = yRoot;
		if(entries[xRoot].rank > entries[yRoot].rank)
			entries[yRoot].parent = xRoot;
		else if(entries[xRoot].rank < entries[yRoot].rank)
			entries[xRoot].parent = yRoot;
		else if(xRoot != yRoot) // we shouldn't union the same thing to itself
		{
			entries[yRoot].parent = xRoot;
			entries[xRoot].rank += 1;
		}
	}

	void reset()
	{
		for(int i = 0; i < numEntries; ++i)
		{
			entries[i].parent = i;
			entries[i].rank = 0;
		}
	}

	// resets only the entries in the change mask (DON"T USE: IT DOESN'T WORK
	// AS EXPECTED!)
	void reset(char* changeMask)
	{
		for(int i = 0; i < numEntries; ++i)
		{
			if(changeMask[i])
			{
				entries[i].parent = i;
				entries[i].rank = 0;
			}
			else
			{
				//everything should be path compressed at this point, so it all gets rank 1
				entries[i].rank = 1;
			}
		}
	}

};

#endif // _PK_UNION_FIND_

