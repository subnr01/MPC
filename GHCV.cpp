

// OpenCVTest.cpp : Defines the entry point for the console application.
//TEAM 5

#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <list>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include "StatMerge.h"
//#include "SegmentMatch.h"
#include "FloatImage.h"
#include "IntImage.h"
#include "LinearShapeMatch.h"
#include <fstream>
#include <pthread.h>
#include <deque>


using namespace std;
std::deque<std::string> actions_queue;

pthread_mutex_t lock;

const double TIME_CALC_FRAMES = 1;

const int DB = 0;

const float THRESH = 0.15f;

const int WRITE_FRAMES = 200;



// Proper Shape matcher things
#define MAX_SHAPE_UNITS 4

void copyUtoF(IplImage* src, IplImage* dest)
{
    int height     = src->height;
    int width      = src->width;
    int step       = src->widthStep/sizeof(uchar);
    int channels   = src->nChannels;
    uchar* data    = (uchar *)src->imageData;
    
    int heightd     = dest->height;
    int widthd      = dest->width;
    int stepd       = dest->widthStep/sizeof(float);
    int channelsd   = dest->nChannels;
    float * datad    = (float *)dest->imageData;
    
    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            datad[y*step+x*channels] = data[y*step+x*channels] / 255.0f;
            datad[y*step+x*channels+1] = data[y*step+x*channels+1] / 255.0f;
            datad[y*step+x*channels+2] = data[y*step+x*channels+2] / 255.0f;
        }
    }
}

void int32toint8(CvMat* src, CvMat* dest)
{
    int cols = src->cols;
    int rows = src->rows;
    
    uchar* dp = dest->data.ptr;
    int* ip = src->data.i;
    
    for(int j = 0; j < rows * cols; ++j)
    {
        // why was there a *2 over here?
        dp[j] = (uchar)ip[j];
    }
}


void copy3ItoU(int* s1, int* s2, int* s3, IplImage* dest)
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

void copyUto3I(IplImage* src, int* d1, int* d2, int* d3)
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
            spos = y*step+x*channels;
            d1[dpos] = data[spos];
            d2[dpos] = data[spos+1];
            d3[dpos] = data[spos+2];
        }
    }
}

void straightIntToInt32(int* src, CvMat* dest, int s)
{
    int* dp = dest->data.i;
    
    for(int j = 0; j < s; ++j)
    {
        dp[j] = src[j];
    }
}

std::string IntToString(int number)
{
    std::ostringstream oss;
    
    // Works just like cout
    oss<< number;
    
    // Return the underlying string
    return oss.str();
}

void writeImage(IplImage* img, std::string prefix, int curNum)
{
    std::string filename = prefix + IntToString(curNum) + ".PNG";
    cvSaveImage(filename.c_str(), img);
}

struct ThreeLayerIntImage
{
    int* r;
    int* g;
    int* b;
    int width;
    int height;
};

void copyTLII(ThreeLayerIntImage& src, ThreeLayerIntImage& dest)
{
    // only allow copies between images of same dimensions
    if(src.width != dest.width || src.height != dest.height)
        return;
    
    for(int i = 0; i < src.width * src.height; ++i)
    {
        dest.r[i] = src.r[i];
        dest.g[i] = src.g[i];
        dest.b[i] = src.b[i];
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//
//  Here is where the interesting stuff starts
//
//
///////////////////////////////////////////////////////////////////////////////


class ActionInstance
{
public:
    std::vector<FloatImage*>* tData;
    int actionType;
    int id;
    float tmass;
    float dist;
};

class ActionType
{
public:
    std::string actionName;
    bool actionEnabled;
    int count;
    int sendKey;
};

bool actionInstanceSorter(ActionInstance* lhs, ActionInstance* rhs)
{
    return lhs->dist < rhs->dist;
}

ActionInstance* loadTemplateByName(std::string actionName, int actionType, int id, int count)
{
    std::cout << "loading " << actionName << " " << id << std::endl;
    
    
    std::string totalPrefix = "/Users/priyankakulkarni/Documents/Project/MPC/ActionRecDemoV3/data copy/" + actionName;
    
    //std::string totalPrefix = "/Users/admin/data/" + actionName;
    
    
    ActionInstance* ret = new ActionInstance;
    ret->tData = new std::vector<FloatImage*>;
    ret->id = id;
    ret->actionType = actionType;
    ret->tmass = 0.0f;
    ret->dist = 0.0f;
    
    for(int i = 0; i < count; ++i)
    {
        int curNum = 10000 + (100 * id) + i;
        std::string fname = totalPrefix + "_" + IntToString(curNum) + ".png";
        //std::string fname = "/Users/admin/data/up_10100.PNG";
        std::cout << "Filename: " << fname << std::endl;
        IntImage* tempII = new IntImage(cvLoadImage(fname.c_str()));
        int* c0 = tempII->getChannel(0);
        FloatImage* tempF = new FloatImage(tempII->width(), tempII->height());
        for(int p = 0; p < tempF->width * tempF->height; ++p)
        {
            if(c0[p] > 127)
            {
                tempF->data[p] = -1.0f;
                ret->tmass += 1.0f;
            }
            
            else
                tempF->data[p] = 1.0f;
        }
        ret->tData->push_back(tempF);
        delete tempII;
    }
    
    std::cout << "Template had mass of " << ret->tmass << std::endl;
    
    return ret;
}

void drawRectangle(IntImage* dest, int cx, int cy, int w, int h, int r, int g, int b)
{
    int* rc = dest->getChannel(0);
    int* gc = dest->getChannel(1);
    int* bc = dest->getChannel(2);
    int iw = dest->width();
    int ih = dest->height();
    
    // left edge
    for(int y = cy; y < std::min(ih, cy + h); ++y)
    {
        rc[(y * iw) + cx] = r;
        gc[(y * iw) + cx] = g;
        bc[(y * iw) + cx] = b;
    }
    
    // right edge
    if(cx + w < iw)
    {
        for(int y = cy; y < std::min(ih, cy + h); ++y)
        {
            rc[(y * iw) + cx + w] = r;
            gc[(y * iw) + cx + w] = g;
            bc[(y * iw) + cx + w] = b;
        }
    }
    
    // top edge
    for(int x = cx; x < std::min(iw, cx + w); ++x)
    {
        rc[(cy * iw) + x] = r;
        gc[(cy * iw) + x] = g;
        bc[(cy * iw) + x] = b;
    }
    
    // bottom edge
    if(cy + h < ih)
    {
        for(int x = cx; x < std::min(iw, cx + w); ++x)
        {
            rc[((cy + h) * iw) + x] = r;
            gc[((cy + h) * iw) + x] = g;
            bc[((cy + h) * iw) + x] = b;
        }
    }
}

void copySubImage(int* sr, int* sg, int* sb, int* dr, int* dg, int* db, int sw, int sh, int dw, int dh, int sx, int sy)
{
    for(int y = 0; y < dh; ++y)
    {
        for(int x = 0; x < dw; ++x)
        {
            int srcpos = ((y + sy) * sw) + (x + sx);
            int destpos = (y * dw) + x;
            dr[destpos] = sr[srcpos];
            dg[destpos] = sg[srcpos];
            db[destpos] = sb[srcpos];
        }
    }
}

IntImage* renderTemplateFrame(std::vector<FloatImage*>* tData, int f)
{
    int twidth = (*tData)[0]->width;
    int theight = (*tData)[0]->height;
    IntImage* ret = new IntImage(twidth, theight, 3);
    ret->fill(0);
    
    // first, figure out the maximum value
    float maxval = 0.0f;
    float* curfdata = ((*tData)[f])->data;
    
    for(int p = 0; p < twidth * theight; ++p)
        if(fabs(curfdata[p]) > maxval)
            maxval = abs(curfdata[p]);
    
    std::cout << "maxval: " << maxval << std::endl;
    
    // now, figure out the multiplier
    float mult = 255.0f / maxval;
    
    int* rchan = ret->getChannel(0);
    int* gchan = ret->getChannel(1);
    int* bchan = ret->getChannel(2);
    
    // now multiply away and stuff
    for(int p = 0; p < twidth * theight; ++p)
    {
        int sval = (int)(curfdata[p] * mult);
        
        if(sval > 0) // put into green channel
        {
            gchan[p] = sval;
        }
        else // sval <= 0 // put into red channel
        {
            rchan[p] = -sval;
        }
        bchan[p] = 0;
    }
    
    return ret;
}

/* this function is run by the second thread */
void *process_actions(void *x_void_ptr)
{
    std::string actions_sample[10];
    
    
   while(1)
   {
  //     std::cout<<"Running"<<std::endl;
    //check if >10 items in queue
   // std::cout<<"\n size of the Queue is "<< actions_queue.size();
    if(actions_queue.size() > 10)
    {
  //       std::cout<<"Greater than 10!"<<std::endl;
        //if yes, pop 10 items
     
        
        std::cout<<"\n size of the Queue inside if "<< actions_queue.size();

        std::deque<std::string>::iterator it = actions_queue.begin();
        for(int i=0;i<10;i++)
        {
            actions_sample[i]= *it++;
            actions_queue.pop_front();
        }
    /*    std::cout<<"New sample: ";
        for(int i=0;i<10;i++)
        {
            //process 10 items
            std::cout << ' ' << actions_sample[i];
            
        } */
        
    }
    
    int idle_count=0;
    int left_count=0;
    int right_count=0;
    
    for (int i=0; i<10; i++) {
        if(actions_sample[i]=="left")
            left_count++;
        else if (actions_sample[i]=="right")
            right_count++;
        else
            idle_count++;
    }
    
    if(left_count>right_count)
    {
        std::cout<<"ACTION DETECTED: left"<<std::endl;
           }
    else if (left_count<right_count)
    {
        std::cout<<"ACTION DETECTED: right"<<std::endl;
    }
    
    else
    {
        
    }
    
    left_count=0;
    right_count=0;
   }
    
    /* the function must return something - NULL will do */
    return NULL;
    
}

void *push_to_queue(void *y_void_ptr)
{
    std::string *y_ptr = (std::string *)y_void_ptr;
    actions_queue.push_back(*y_ptr);
 //   std::cout << "Size: "<<actions_queue.size() << std::endl;
    
  //  if(actions_queue.size()%10==0)
    //    sleep(1);
    
    /* the function must return something - NULL will do */
    return NULL;

}



int main(int argc, char* argv[])
{
    int cameraid; // = 1;
    bool create_thread = false;

    int writeFCount = 0;
    if (argc==2) cameraid=1;
    else cameraid =0;
    
    // define processing and display resolutions
    int process_width = 180;
    int process_height = 144;
    int display_width = 320;
    int display_height = 240;
    
    int searchX = 20;
    int searchY = 0;
    int searchW = 150;
    int searchH = 65;
    bool normalizing = true;
    
    //CHANGES BEGIN HERE
    
//    int actionFrames = 10;
    //
       int actionFrames = 9;
    
  /*
    
    int numActions = 4;
    ActionType* actionTypes = new ActionType[numActions];
    actionTypes[0].actionName = "up";
    actionTypes[0].actionEnabled = true;
    actionTypes[0].count = 15;
    actionTypes[0].sendKey = 1049;
    actionTypes[1].actionName = "left";
    actionTypes[1].actionEnabled = true;
    actionTypes[1].count = 15;
    actionTypes[1].sendKey = 1062;
    actionTypes[2].actionName = "right";
    actionTypes[2].actionEnabled = true;
    actionTypes[2].count = 15;
    actionTypes[2].sendKey = 1064;
    actionTypes[3].actionName = "idle";
    actionTypes[3].actionEnabled = true;
    actionTypes[3].count = 15;
    actionTypes[3].sendKey = -1;
    
    */
     
    pthread_mutex_init(&lock, NULL);
     int numActions = 3;
     ActionType* actionTypes = new ActionType[numActions];
     actionTypes[0].actionName = "left";
     actionTypes[0].actionEnabled = true;
     actionTypes[0].count = 9;
     actionTypes[0].sendKey = 1049;
     
     actionTypes[1].actionName = "right";
     actionTypes[1].actionEnabled = true;
     actionTypes[1].count = 9;
     actionTypes[1].sendKey = 1062;
     
     actionTypes[2].actionName = "idle";
     actionTypes[2].actionEnabled = true;
     actionTypes[2].count = 9;
     actionTypes[2].sendKey = 1;
     
    
    
    //CHANGES END HERE
    
    int* voteArray = new int[numActions];
    int knearestk = 5;
    int winval = 3;
    
    int numSFrames = 5;
    int sframeMajority = 3;
    int* sframes = new int[numSFrames];
    int sframePos = 0;
    int minRepeatTime = 10;
    int repeatTimout = 0;
    
    int timeoutPeriod = 5;
    int* timeoutArray = new int[numActions];
    for(int i = 0; i < numActions; ++i)
        timeoutArray[i] = 0;
    int timeoutPeriodArray[4] = {12, 5, 5, 5};
    
    std::vector<ActionInstance*> actionInstances;
    
    // load the template library
    std::cout << "Loading action library...\n";
    for(int at = 0; at < numActions; ++at)
    {
        for(int i = 0; i < actionTypes[at].count; ++i)
        {
            ActionInstance* curInstance = loadTemplateByName(actionTypes[at].actionName, at, i+1, actionFrames);
            actionInstances.push_back(curInstance);
        }
    }
    std::cout << "Done loading action library; loaded " << actionInstances.size() << " instances.\n";
    
 //   cvWaitKey(10000);
    
    IntImage* timg = renderTemplateFrame(actionInstances[0]->tData, 2);
    cvNamedWindow( "Template", CV_WINDOW_AUTOSIZE );
    IplImage* timgipl = timg->getIplImage();
    cvShowImage("Template", timgipl);

   cvWaitKey(10000);
    
    LinearShapeMatch* lsm = new LinearShapeMatch(process_width, process_height, actionFrames);
    lsm->setFill(1.0f, true);
    
    // get access to webcam
    CvCapture* srcVideoCapture = cvCaptureFromCAM( cameraid );
    
    //   CvCapture* srcVideoCapture = cvCaptureFromFile("output.avi");
    
    float scale_x = display_width / process_width;
    float scale_y = display_height / process_height;
    
    // initialize some buffers
    IntImage* src_img = new IntImage(display_width, display_height, 3);
    IntImage* src_segmentation = new IntImage(process_width, process_height, 3);
    
  //  IplImage* rawFrame = cvCreateImage(cvSize(process_width, process_height), IPL_DEPTH_8U, 3);;
    IplImage* rawFrame;
    
    IplImage* resizedPFrame = cvCreateImage(cvSize(process_width, process_height), IPL_DEPTH_8U, 3);
    IplImage* resizedSrcFrame = cvCreateImage(cvSize(display_width, display_height), IPL_DEPTH_8U, 3);
    IplImage* finalDisplayFrame = cvCreateImage(cvSize(display_width, display_height), IPL_DEPTH_8U, 3);
    IplImage* display_temp = cvCreateImage(cvSize(process_width, process_height), IPL_DEPTH_8U, 3);
    
    IntImage* src_r_img = new IntImage(process_width, process_height, 3);
    IntImage* dest_img = new IntImage(process_width, process_height, 3);
    IntImage* seg_img = new IntImage(process_width, process_height, 3);
    //IntImage* bground_img = new IntImage(process_width, process_height, 3);
    
    // clear out the color buffers
    src_img->fill(0);
    
    // the probability threshold for merging segments during the segmentation
    // step; larger -> smaller segments
    float seg_prob = 0.99f;
    
    // initialize the segmenter and do an initial segmentation just to provide
    // the shape units with some valid segmentation to start with
    int* tempSeg;
    int* tempSegSizes;
    StatMerge* statMerge = new StatMerge(process_width, process_height);
    tempSeg = statMerge->doSegmentation(src_r_img->getChannel(0), src_r_img->getChannel(1), src_r_img->getChannel(2), seg_prob);
    tempSegSizes = statMerge->getSizeArray();
    src_segmentation->copyChannel(tempSeg, 0);
    src_segmentation->copyChannel(tempSegSizes, 1);
    
    cvNamedWindow( "Source Video", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "Template Locations", CV_WINDOW_AUTOSIZE );
    
    std::cout << "About to enter main loop.\n";
    
    // prepare some timing stuff
    std::list<clock_t> clocktimes(TIME_CALC_FRAMES);
    clock_t curTime, frontTime;
    float avg_delay, fps;
    int last_delay = 0;
    
    float desired_delay = 1.0f / 12.0f;
    
    /*
     clock_t cst = clock();
     std::cout << "cst: " << cst << std::endl;
     cvWaitKey(10000);
     clock_t cnd = clock();
     std::cout << "cnd: " << cnd << std::endl;
     int clockFactor = cnd - cst;
     std::cout << "Clock factor: " << clockFactor << std::endl;
     cvWaitKey();
     */
    int clockFactor = 1;
    
    clock_t prevTime = clock();
    curTime = clock();
    
    
    /*
     //PRIYANKA'S CHANGES
     
     std::ofstream outfile;
     outfile.open("/Users/priyankakulkarni/Documents/test.txt", std::ios_base::app);
     
     //  system("/Users/priyankakulkarni/Documents/Project/MPC/keyboard/simulate_keypress_mac");
     
     // std::cout << "keyboard simulation done";
     */
    
    
    

    // enter main loop-- this runs the display as fast as possible
    while(1)
    {
        
        // deal with timing stuff
        prevTime = curTime;
        curTime = clock();
        frontTime = clocktimes.front();
        
        clocktimes.pop_front();
        clocktimes.push_back(curTime);
        
        avg_delay = (float)(curTime - prevTime) / (float)(TIME_CALC_FRAMES * clockFactor); // * CLOCKS_PER_SEC);
        fps = 1.0f / avg_delay;
        //        std::cout << "avg_delay: " << avg_delay << std::endl;
        //        std::cout << "fps: " << fps << std::endl;
        
        
        // grab a frame
        rawFrame = cvQueryFrame(srcVideoCapture);
        
      IplImage* rawFrames[3] = {rawFrame, rawFrame, rawFrame};
        
   //     std::cout<<"entering loop"<<std::endl;
        for(int i=0;i<3;i++)
        {
           // std::cout<<"running frame no: "<<i<<std::endl;
        cvResize(rawFrames[i], resizedSrcFrame);
        cvResize(rawFrames[i], resizedPFrame);
        
        cvShowImage("Source Video", resizedSrcFrame);
//        cvWaitKey(10);
        
        src_r_img->copy(resizedPFrame, true);
        dest_img->copy(resizedPFrame, true);
        src_img->copy(resizedSrcFrame, true);
        cvShowImage("Source Video", resizedSrcFrame);
//        cvWaitKey(10);
        
        
        
        // segment it
        tempSeg = statMerge->doSegmentation(src_r_img->getChannel(0), src_r_img->getChannel(1), src_r_img->getChannel(2), seg_prob);
        tempSegSizes = statMerge->getSizeArray();
        src_segmentation->copyChannel(tempSeg, 0);
        src_segmentation->copyChannel(tempSegSizes, 1);
        
        statMerge->doLinesOnly(seg_img->getChannel(0), seg_img->getChannel(1), seg_img->getChannel(2));
        
        // give the segmentation to the matcher
        lsm->pushFrame(src_segmentation);
        
        float bestDist = 1000000.0f;
        
        // run through every instance and compare it...
        for(int inst = 0; inst < actionInstances.size(); ++inst)
        {
            ActionInstance* curInstance = actionInstances[inst];
            float curDist = 1000000.0f;
            if(actionTypes[curInstance->actionType].actionEnabled)
            {
                float curVal = lsm->doSinglePointMatching(searchX, searchY, curInstance->tData);
                if(normalizing)
                    curDist = (curInstance->tmass + curVal) / (curInstance->tmass);
                else
                    curDist = (curInstance->tmass + curVal);
            }
            if(curDist < bestDist)
                bestDist = curDist;
            curInstance->dist = curDist;
        }
        
        // sort instances according to distance..
        std::sort(actionInstances.begin(), actionInstances.end(), actionInstanceSorter);
        
        for(int i = 0; i < numActions; ++i)
            voteArray[i] = 0;
        
        // get the k nearest to vote
        for(int i = 0; i < knearestk; ++i)
            voteArray[actionInstances[i]->actionType] += 1;
        
        std::string bestAction = "ambiguous";
        bool ambiguous = true;
        float bestDist2 = actionInstances[0]->dist;
        int bestActionID = -1;
        
        for(int i = 0; i < numActions; ++i)
        {
            if(voteArray[i] >= winval)
            {
                bestAction = actionTypes[i].actionName;
                bestActionID = i;
                ambiguous = false;
                break;
            }
        }
        
        /*
         sframePos = (sframePos + 1) % numSFrames;
         sframes[sframePos] = bestActionID;
         */
        
        int sendActionID = -1;
        std::string sendActionName = "none";
        
        // count actions
        /*
         for(int i = 0; i < numActions; ++i)
         voteArray[i] = 0;
         
         for(int i = 0; i < numSFrames; ++i)
         if(sframes[i] != -1)
         voteArray[sframes[i]] += 1;
         
         for(int i = 0; i < numActions; ++i)
         {
         if(voteArray[i] >= sframeMajority)
         {
         sendActionID = i;
         sendActionName = actionTypes[sendActionID].actionName;
         break;
         }
         }*/
        
        if(bestActionID != -1 && timeoutArray[bestActionID] <= 0)
        {
            sendActionID = bestActionID;
            sendActionName = actionTypes[sendActionID].actionName;
            //timeoutArray[bestActionID] = timeoutPeriod;
            timeoutArray[bestActionID] = timeoutPeriodArray[bestActionID];
        }
        
        for(int i = 0; i < numActions; ++i)
            timeoutArray[i] -= 1;
        
        // get the best one...
        //ActionInstance* best = actionInstances[0];
        //std::string bestAction = actionTypes[best->actionType].actionName;
        //std::cout << "Best action: " << bestAction << " " << best->id << " with a distance of " << best->dist << std::endl;
        //std::cout << "Real best dist: " << bestDist << std::endl;
        //        std::cout << "Best action: " << bestAction << std::endl;
 //       std::cout << "Send action: " << sendActionName << std::endl;
        //        std::cout << "Best distance: " << bestDist2 << std::endl;
        //    outfile << sendActionName << "\n";
        
            
        /* QUEUE CODE */
            
            /* this variable is our reference to the second thread */
            pthread_t process_actions_thread, push_to_queue_thread;
 
            
#define SUBBU 1
#if SUBBU
           
            pthread_mutex_lock(&lock);
                       //push action to queue
            actions_queue.push_back(sendActionName);
            pthread_mutex_lock(&unlock);
            
            if (!create_thread)
            {
            
                
            /* create a second thread */
            if(pthread_create(&process_actions_thread, NULL, process_actions, NULL)) {
                
                fprintf(stderr, "Error creating thread\n");
                return 1;
                
            }
            
            
        
          /*
            
            if(pthread_create(&push_to_queue_thread, NULL, push_to_queue, &sendActionName)) {
                
                fprintf(stderr, "Error creating thread\n");
                return 1;
                
            }
          */
            create_thread = true;
        }

#endif
        if(sendActionID >= 0)
        {
            int keyToSend = actionTypes[sendActionID].sendKey;
            if(keyToSend > 0)
            {
                std::string sendString = IntToString(keyToSend) + "\n";
                write(3, sendString.c_str(), sendString.size());
                
            }
        }
        
        drawRectangle(dest_img, searchX, searchY, searchW, searchH, 255, 255, 255);
        dest_img->getIplImage(display_temp);
        cvShowImage("Template Locations", display_temp);
        
        //copy3ItoU(seg_r, seg_b, seg_g, segmentation_temp);
        //cvShowImage("Segmentation", segmentation_temp);
        
        // do background subtraction step (TODO!)
        // Note: it is safe to overwrite tempSeg and tempSegSizes since they are
        // 'renewed' every time the segmentation is done (they aren't used to
        // warm start the segmenter or anything)
        
        // pad out the frame with a delay if we are going too fast:
        // the idea here is that if our delay last frame was less than desired,
        // increase the delay a bit; if our delay was too large, decrease it
        int delayTime = last_delay;
        if(avg_delay < desired_delay)
            delayTime += (int)((desired_delay - avg_delay) * 100);
        else
        {
            delayTime -= (int)((avg_delay - desired_delay) * 100);
        }
        if(delayTime < 1)
            delayTime = 1;
        
        last_delay = delayTime;
        
        //int keyPressed = cvWaitKey(delayTime);
        int keyPressed = cvWaitKey(1);
 //       std::cout << "key : " << (keyPressed & 255) << std::endl;
        if( (keyPressed & 255) == 27 ) // esc key
        {
            std::cout << "Closing " << std::endl;
            
            //       outfile.close();
            
            // quit
            break;
        }
    }
    }
  
    return 0;
}