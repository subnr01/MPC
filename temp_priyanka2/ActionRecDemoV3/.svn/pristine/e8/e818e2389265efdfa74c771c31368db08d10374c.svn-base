CFLAGS=-g -O2 -Wall
CXXFLAGS=$(CFLAGS)

LDFLAGS=-lcv -lpthread -lhighgui

OBJS=GHCV.o SegmentMatch.o StatMerge.o LinearShapeMatch.o FloatImage.o IntImage.o

all: demo

clean:
	rm -rf demo *.o

demo: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^


