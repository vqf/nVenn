#ifndef TOPOL_H_INCLUDED
#define TOPOL_H_INCLUDED

#include <vector>
#include <cmath>
#include <time.h>
#include <fstream>


#define CIRCLE_MASS 200.0f
#define POINT_MASS 20

typedef unsigned int UINT;


float sk = 1e3f;
float dt = 1e-1f;
float baseBV = 5.0f;


using namespace std;

UINT twoPow(UINT n)
{
    if (n > 8*sizeof(UINT)-1)
        return 0;
    UINT result = 1;
    result = result << n;
    return result;
}




float sprod(vector<float> a, vector<float> b)
{
    float result;
    UINT i;
    result = 0;
    for (i = 0; i < a.size(); i++)
    {
        result += a[i] * b[i];
    }
    return result;
}

vector<float> arr2vec(float arr[], int n)
{
    int i;
    vector<float> result;
    for (i = 0; i < n; i++)
    {
        result.insert(result.end(), arr[i]);
    }
    return result;
}


vector<int> toBin(int number, UINT nBits = 0)
{
    //  Takes an integer and returns a vector containing
    //  its binary representation

    UINT i;                          // iterate over each bit
    int bit;                        // temp storage of each bit
    vector<int> bits;               // return vector
    while (number > 1)
    {
        bit = number % 2;
        number = number / 2;
        bits.insert(bits.begin(), bit);
    }
    if (number > 0)
        bits.insert(bits.begin(),1);
    if (nBits > bits.size())
    {
        for (i = bits.size(); i < nBits; i++)
        {
            bits.insert(bits.begin(), 0);
        }
    }
    return bits;
}
int toInt(vector<int> v)
{
    // Takes a vector containing the binary representation
    // of an integer and returns that integer

    int i;                          // decreasing iterator
    int counter = 0;                // increasing iterator
    int result = 0;
    for (i = v.size()-1; i >= 0; --i)
    {
        result = result + v[i] * twoPow(counter);
        counter++;
    }
    return result;
}

class fileText
{
    string text;
public:
    fileText(){
      text = "";
    }
    void addLine(string t)
    {
        text = text + t +"\n";
    }
    void clearText()
    {
        text = "";
    }
    string getText()
    {
        return text;
    }
};

float distance(float x0, float y0, float x1, float y1)
{
    float result = 0;
    float rx, ry;
    rx = x1 - x0;
    ry = y1 - y0;
    rx *= rx;
    ry *= ry;
    result = sqrt(rx + ry);
    return result;
}

typedef struct blData{
  float minratio;
  float stepdt;
  bool fixCircles;
  bool signalEnd;
  bool smoothSVG;
  float surfRatio;
  float minSurfRatio;
  float minDx;
  float minDy;
  float startdt;
  float marginScale;
  float margin;
  float totalCircleV;
  float totalLineV;
  int contacts;
  UINT ncycles;
  UINT maxRunningTime;
  UINT ncyclesInterrupted;
  float maxf;
  float maxv;
  int checkFor; // If any of the previous or following 10 point is sticking to
                           // the surface, the current point will also stick
  string inputFile;
  string fname;
} blData;


class point
{
public:
    UINT n;
    float x;
    float y;
    float vx;
    float vy;
    float fx;
    float fy;
    float radius;
    float mass;
    float orig;
    bool cancelForce;
    bool inContact;
    bool softenVel;
    point()
    {
        x = 0;
        y = 0;
        radius = 0;
        reset();
    }
    void reset()
    {
        resetv();
        resetf();
    }
    void resetv()
    {
        vx = 0;
        vy = 0;
    }
    void resetf()
    {
        fx = 0;
        fy = 0;
    }
};

class timeMaster
{
    UINT count;
    float unstable;
    float defaultst;
    float stored;
public:

    timeMaster()
    {}

    void init(float unst)
    {
        count = 0;
        unstable = unst;
        defaultst = unst;
        stored = unst;
    }

    void clear()
    {
        count = 0;
        unstable = defaultst;
    }

    void report(float unst)
    {
        if (unst == unstable)
        {
            ++count;
        }
        else
        {
            count = 0;
            unstable = unst;
        }
        if (count >= 5)
        {
            stored = unstable;
        }
    }

    float unstabledt()
    {
        return stored;
    }
};

class lCounter
{

    UINT counter;
    UINT lLimit;
    UINT uLimit;
    UINT span;
    bool toInf; // Disables the counter

public:

    lCounter(UINT lowerLimit, UINT upperLimit)
    {
        counter = lowerLimit;
        lLimit = lowerLimit;
        uLimit = upperLimit;
        span = uLimit - lLimit;
        toInf = false;
    }

    void setAsStable(){
      toInf = true;
    }

    lCounter()
    {
        counter = 0;
        lLimit = 0;
        uLimit = 0;
        span = 0;
        toInf = false;
    }

    void setLimits(UINT lowerLimit, UINT upperLimit)
    {
        counter = lowerLimit;
        lLimit = lowerLimit;
        uLimit = upperLimit;
        span = uLimit - lLimit;
    }

    bool isMax()
    {
      if (toInf) return false;
        if (counter == uLimit)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool isMin()
    {
      if (toInf) return false;
        if (counter == lLimit)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void operator++(int)
    {
      if (toInf) return;
        if (counter < uLimit)
        {
            counter++;
        }
        else
        {
            counter = lLimit;
        }
    }
    void operator++()
    {
      if (toInf) return;
        if (counter < uLimit)
        {
            counter++;
        }
        else
        {
            counter = lLimit;
        }
    }
    void operator--(int)
    {
      if (toInf) return;
        if (counter > lLimit)
        {
            counter--;
        }
        else
        {
            counter = uLimit;
        }
    }
    void operator--()
    {
      if (toInf) return;
        if (counter > lLimit)
        {
            counter--;
        }
        else
        {
            counter = uLimit;
        }
    }
    operator UINT()
    {
        return counter;
    }
    void operator=(UINT a)
    {
        UINT tCount;
        tCount =  a - lLimit;
        tCount = tCount % span;
        counter = lLimit + tCount;
    }
};

class scale
{
    float x;
    float y;
    float X;
    float Y;
    bool clear;
public:
    scale(){
      x = -1;
      X = 1;
      y = -1;
      Y = 1;
      clear = false;
    }

    void initScale(){
      x = -1;
      X = 1;
      y = -1;
      Y = 1;
      clear = false;
    }

    bool isClear(){
      return clear;
    }
    void setClear(bool v = true){
      clear = v;
    }

    float minX(){
      return x;
    }

    void setMinX(float v){
      x = v;
    }

    float maxX(){
      return X;
    }
    void setMaxX(float v){
      X = v;
    }

    float minY(){
      return y;
    }

    void setMinY(float v){
      y = v;
    }

    float maxY(){
      return Y;
    }

    void setMaxY(float v){
      Y = v;
    }

    float xSpan()
    {
        float result = X - x;
        if (result == 0) result = 1.0f;
        return result;
    }
    float ySpan()
    {
        float result = Y - y;
        if (result == 0) result = 1.0f;
        return result;
    }
    float ratio()
    {
        float result;
        if (xSpan() == 0)
            return 1.0f;
        result = ySpan() / xSpan();
        return result;
    }
};

struct rgb
{
    float red;
    float green;
    float blue;
};

template<typename T>
string toString(T input)
{
    ostream result;
    result << input;
    return result;
}




float perimeter(vector<point> v, bool close = false)
{
    UINT i;
    float result = 0;
    float rx, ry;
    for (i = 0; i < v.size() - 1; i++)
    {
        rx = v[i+1].x - v[i].x;
        ry = v[i+1].y - v[i].y;
        result += sqrt(rx * rx + ry * ry);
    }
    if (close)
    {
        rx = v[v.size()-1].x - v[0].x;
        ry = v[v.size()-1].y - v[0].y;
        result += sqrt(rx * rx + ry * ry);
    }
    return result;
}



class binMap
{

    int ngroups;
    vector<vector<int> > column;
    vector<vector<vector<int> > > row;

    signed int firstUnmatched1(vector<int> v)
    {
        /***********************************************************

            Returns an integer with the position of the first
            unmatched 1. If none is found, the function
            returns -1.

            @v is the input vector to be searched for unmatched 1s

        ***********************************************************/

        signed int counter = 0;
        UINT i;
        for (i = 0; i < v.size(); i++)
        {

            if (v[i] == 1)
            {
                counter++;
                if (counter > 0)
                    return i;
            }
            else
            {
                counter--;
            }
        }
        return -1;      // no unmatched @which
    }

    signed int firstUnmatched0(vector<int> v)
    {
        /***********************************************************

            Returns an integer with the position of the first
            unmatched 0. If none is found, the function
            returns -1.

            @v is the input vector to be searched for unmatched 0s

        ***********************************************************/

        signed int counter = 0;
        UINT i, j;
        bool ismatched = false;
        for (i = 0; i < v.size() - 1; i++)
        {
            counter = 0;
            if (v[i] == 0)
            {
                ismatched = false;
                for (j = i + 1; j < v.size(); j++)
                {
                    if (v[j] == 0)
                    {
                        counter++;
                    }
                    else
                    {
                        counter--;
                    }
                    if (counter < 0)
                        ismatched = true;
                }
                if (!ismatched)
                    return i;
            }
        }
        if (v[v.size() - 1] == 0)
            return (v.size()-1);
        return -1;      // no unmatched @which
    }
    vector<vector<int> > getBranches(vector<int> v)
    {
        /***********************************************************

            Returns a vector containing all of the branches derived
            from the input vector. These are obtained by changing to
            1 each 0 to the right of the last 1 if the resulting
            vector has no unmatched 1s. If there are no branches,
            the return vector is empty.

            @v input binary vector

        ***********************************************************/

        UINT i;
        int last1=0;
        vector<int> tempV;
        vector<vector<int> > result;
        for (i = 0; i < v.size(); i++)
        {// Get last 1
            if (v[i]==1)
                last1 = i;
        }
        for (i = last1 + 1; i < v.size(); i++)
        {
            tempV = v;
            tempV[i] = 1;
            if (firstUnmatched1(tempV) == -1)
            {
                result.insert(result.end(), tempV);
            }
        }
        return result;
    }

    void fillColumn(vector<int> v)
    {
        vector<int> result;
        int position = 0;
        result = v;
        position = firstUnmatched0(result);
        column.insert(column.end(), v);
        while (position != -1)
        {
            result[position] = 1;
            column.insert(column.end(), result);
            position = firstUnmatched0(result);
        }
        row.insert(row.end(), column);
        column.clear();
    }

    void fillRow(vector<vector<int> > c)
    {
        UINT i;
        vector<vector<int> > tempc;
        for (i = 0; i < c.size(); i++)
        {
            fillColumn(c[i]);
            tempc = getBranches(c[i]);
            fillRow(tempc);
        }
    }

public:

    friend class borderLine;

    binMap(int n)
    {
        vector<int> currentV;
        vector<vector<int> > currentC;
        ngroups = n;
        currentV = toBin(0, n);
        fillColumn(currentV);
        currentC = getBranches(currentV);
        fillRow(currentC);
    }
    void textOut()
    {
        UINT i, j, k;
        for (i = 0; i < row.size(); i++)
        {
            for (j = 0; j < row[i].size(); j++)
            {
                for (k = 0; k < row[i][j].size(); k++)
                {
                    cout << row[i][j][k];
                }
                cout << " ";
            }
            cout << endl;
        }
    }
};


class borderLine
{
    friend class glGraphics;
    binMap* bm;
    vector<string> groups;
    vector<point> p;
    vector<vector<point> > bl;
    vector<vector<point> > bl_old5;
    vector<vector<point> > bl_old10;
    vector<point> circles;
    vector<point> circles_old5;
    vector<point> circles_old10;
    UINT ngroups;
    UINT startPerim;
    lCounter blCounter;
    lCounter deciderCounter;
    lCounter refreshScreen;
    vector<float> circRadii;
    vector<float> w;
    vector<float> origw;
    vector<rgb> colors;
    vector<string> svgcolors;
    vector<point> warn;
    scale internalScale;
    int totalExpectedSurface;
    vector<char*> dataDisplay;
    timeMaster udt;
    blData blSettings;
    float minRat;

    void initBlData(blData* b){
      b->minratio = 0.005f;
      b->stepdt = 0.6f;
      b->fixCircles = false;
      b->signalEnd = false;
      b->smoothSVG = false;
      b->surfRatio = 0;
      b->minSurfRatio = 0;
      b->minDx = 0;
      b->minDy = 0;
      b->startdt = 0;
      b->marginScale = 0.05f;
      b->margin = 0;
      b->totalCircleV = 0;
      b->totalLineV = 0;
      b->contacts = 0;
      b->checkFor = 30;
      b->ncycles = 0;
      b->ncyclesInterrupted = 0;
      b->maxRunningTime = 200; // 300 seconds to finish the first part
    }



    void attention(float x, float y)
    {
        point temp;
        initPoint(&temp);
        temp.x = x;
        temp.y = y;
        temp.radius = 1;
        warn.insert(warn.end(), temp);
    }

    string float2string(float f){
        char* dsp = (char*) calloc(100, sizeof(char));
        if (dsp){
          sprintf(dsp, "%g", f);
          string result(dsp);
          free(dsp);
          return result;
        }
        return "Error";
    }

    string UINT2string(UINT f){
        char* dsp = (char*) calloc(100, sizeof(char));
        if (dsp){
          sprintf(dsp, "%u", f);
          string result(dsp);
          free(dsp);
          return result;
        }
        return "Error";
    }

    string bool2string(bool f){
        string result = f ? "1" : "0";
        return result;
    }

    void displayFloat(string label, float d){
        char* dsp = (char*) calloc(100, sizeof(char));
        if (dsp){
          sprintf(dsp, "%s: %g", label.c_str(), d);
          dataDisplay.insert(dataDisplay.end(), dsp);
        }
    }

    void displayUINT(string label, UINT d){
        char* dsp = (char*) calloc(100, sizeof(char));
        if (dsp){
          sprintf(dsp, "%s: %u", label.c_str(), d);
          dataDisplay.insert(dataDisplay.end(), dsp);
        }
    }

    rgb toRGB(int color, int max)
    {
        rgb result;
        result.red = (0x00FF0000 & color)/0x10000;
        result.red = result.red * max / 0xFF;
        result.green = (0x0000FF00 & color)/0x100;
        result.green = result.green * max / 0xFF;
        result.blue = (0x000000FF & color);
        result.blue = result.blue * max / 0xFF;
        return result;
    }


    void initOlds()
    {
        bl_old5 = bl;
        bl_old10 = bl;
        circles_old5 = circles;
        circles_old10 = circles;
    }

    point place(scale s, point m)
    {
        point r;
        initPoint(&r);
        scale tempScale;
        tempScale.initScale();
        float offset;
        float Scale;
        if (s.ratio() <= internalScale.ratio())
        {
            offset = s.xSpan() - s.ySpan() / internalScale.ratio();
            offset /= 2;
            tempScale.setMinX(offset + s.minX());
            tempScale.setMaxX(s.maxX() - offset);
            tempScale.setMinY(s.minY());
            tempScale.setMaxY(s.maxY());
        }
        else
        {
            offset = s.ySpan() - internalScale.ratio() * s.xSpan();
            offset /= 2;
            tempScale.setMinX(s.minX());
            tempScale.setMaxX(s.maxX());
            tempScale.setMinY(offset + s.minY());
            tempScale.setMaxY(s.maxY() - offset);
        }
        r.x = m.x - internalScale.minX();
        r.y = m.y - internalScale.minY();
        r.x /= internalScale.xSpan();
        r.y /= internalScale.ySpan();
        r.x *= tempScale.xSpan();
        r.y *= tempScale.ySpan();
        r.x += tempScale.minX();
        r.y += tempScale.minY();
        Scale = tempScale.xSpan() / internalScale.xSpan();
        r.radius = Scale * m.radius;
        return r;
    }

    bool isInside(point p0, point p1, point target)
    {
        /**
        Returns true if target is inside the box limited
        by p0 and p1. False otherwise
        **/

        bool result = true;
        float xdiscr, ydiscr;
        xdiscr = (target.x - p0.x) * (target.x - p1.x);
        ydiscr = (target.y - p0.y) * (target.y - p1.y);
        if (xdiscr > 0 || ydiscr > 0)
        {
            result = false;
        }
        return result;
    }
    void resetCircleRadius(){
        UINT i;
        float minCoord = internalScale.xSpan();
        if (internalScale.ySpan() < minCoord) minCoord = internalScale.ySpan();
        minRat = 0.01 * minCoord;
        for (i = 0; i < circles.size(); i++){
            float trad = circles[i].radius;
            float tr = circRadii[i];
            //if (tr > 0){
            //  displayFloat("TR", tr);
            //  displayFloat("TRAD", trad);
            //}
            if (tr > 0 && tr < minRat){
                //if (tr < minRat){
                  trad = minRat;
                //}
            }
            else if (trad != tr){
              trad = tr;
            }
            circles[i].radius = trad;
        }

    }

    void setCircles(binMap b, vector<float> o)
    {
        UINT i, j;
        int n;
        int height;
        int offset;
        float maxw = 0;
        point cpoint;
        initPoint(&cpoint);
        height = b.row[0].size();
        for (i = 0; i < w.size(); i++)
        {
            if (w[i] > maxw)
                maxw = w[i];
        }
        for (i = 0; i < b.row.size(); i++)
        {
            offset = (height - b.row[i].size()) / 2;
            for (j = 0; j < b.row[i].size(); j++)
            {
                n = toInt(b.row[i][j]);
                cpoint.n = n;
                cpoint.x = 6 * i;
                cpoint.y = 6 * (j + offset - 1);
                cpoint.radius = 2 * sqrt(w[n] / maxw);
                circRadii.insert(circRadii.end(), 2 * sqrt(origw[n] / maxw));
                cpoint.orig = o[n];
                circles.insert(circles.end(), cpoint);
            }
        }
    }


    void setScale(point p)
    {
        if (internalScale.isClear() == true)
        {
            internalScale.setMinX(p.x);
            internalScale.setMinY(p.y);
            internalScale.setMaxX(p.x);
            internalScale.setMaxY(p.y);
            internalScale.setClear(false);
        }
        else
        {
            if (p.x < internalScale.minX())
                internalScale.setMinX(p.x);
            if (p.y < internalScale.minY())
                internalScale.setMinY(p.y);
            if (p.x > internalScale.maxX())
                internalScale.setMaxX(p.x);
            if (p.y > internalScale.maxY())
                internalScale.setMaxY(p.y);
        }
    }


    void setPoints(binMap b, UINT ngroup)
    {
        int i, counter;
        int cstart;         //placement of first and last point
        //depending on ngroup
        point cpoint;       //point storage
        initPoint(&cpoint);
        point fpoint;       //first point
        initPoint(&fpoint);
        int height;
        int width;
        int omyheight;      //offset
        int myheight;
        int isOne;
        height = b.row[0].size();
        // First point
        if (ngroup < (ngroups - 1))
        {
            cstart = b.ngroups - ngroup - 2;
        }
        else
        {
            cstart = ngroups - 1;
        }
        fpoint.x = -6 * (1 + cstart);
        fpoint.y = 6 * (height + 1 + cstart);
        setScale(fpoint);
        p.insert(p.end(), fpoint);
        // Variable points
        width = b.row.size();
        for (i = 0; i < width; i++)
        {
            cpoint.x = 6 * i - 2;
            // mygroup
            myheight = b.row[i].size();
            omyheight = (height - myheight) / 2;  // the diff is always even
            isOne = 1;
            counter = myheight;                   // counts the row
            if (b.row[i][counter -1][ngroup] == 1)
            {
                while (isOne == 1)
                {
                    counter--;
                    if (counter >= 0)
                    {
                        isOne = b.row[i][counter][ngroup];
                    }
                    else
                    {
                        isOne = 0;
                    }
                }
            }
            else
            {
                counter--;
            }
            cpoint.y = 6 * (omyheight + counter) - 3;
            setScale(cpoint);
            p.insert(p.end(), cpoint);
            cpoint.x = 6 * i + 2;
            setScale(cpoint);
            p.insert(p.end(), cpoint);
            // add circles
        }
        //last point
        cpoint.x = 6 * (width + 1 + cstart);
        cpoint.y = 6 * (height + 1 + cstart);
        cpoint.mass = POINT_MASS;
        setScale(cpoint);
        p.insert(p.end(), cpoint);
        initOlds();
    }

    void clearForces()
    {
        UINT i, j;
        //clear line point forces
        for (i = 0; i < bl.size(); i++)
        {
            for (j = 0; j < bl[i].size(); j++)
            {
                bl[i][j].fx = 0;
                bl[i][j].fy = 0;
            }
        }
        //clear circle forces
        for (i = 0; i < circles.size(); i++)
        {
            circles[i].fx = 0;
            circles[i].fy = 0;
        }
    }

    void resetOld()
    {
        UINT i, j;
        for (i = 0; i < bl_old5.size(); i++)
        {
            for(j = 0; j < bl_old5[i].size(); j++)
            {
                bl_old5[i][j].reset();
            }
        }
        for (i = 0; i < bl_old10.size(); i++)
        {
            for (j = 0; j < bl_old10[i].size(); j++)
            {
                bl_old10[i][j].reset();
            }
        }
        blSettings.surfRatio = estSurf();
        blSettings.minSurfRatio = blSettings.surfRatio;
        blSettings.minDx = internalScale.xSpan();
        blSettings.minDy = internalScale.ySpan();
    }



    point eqforce(point &p0, point &p1, float kattr = 5e-2f)
    {
        point result;
        initPoint(&result);
        float radius;
        float dx, dy;
        float fatt;
        float d;
        radius = 1.0 * (p0.radius + p1.radius);
        float maxrad = p0.radius;
        if (p1.radius > maxrad) maxrad = p1.radius;
        //radius *= 1.2;
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        d = distance(p0.x, p0.y, p1.x, p1.y);
        fatt = 0;
        if (d > maxrad) fatt = sk * kattr * (d - radius);
        if (d < maxrad) fatt = -fatt;
        fatt /= twoPow(ngroups);
        result.fx = dx * fatt;
        result.fy = dy * fatt;
        p0.fx += result.fx;
        p1.fx -= result.fx;
        p0.fy += result.fy;
        p1.fy -= result.fy;
        return result;
    }

    point spring(point &p0, point &p1, float dampen = 0, float springK = sk)
    {
        point result;
        initPoint(&result);
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        result.fx = springK * dx;
        result.fy = springK * dy;
        p0.fx += result.fx - dampen * p0.vx * p0.vx * p0.vx;
        p1.fx -= result.fx + dampen * p1.vx * p1.vx * p1.vx;
        p0.fy += result.fy - dampen * p0.vy * p0.vy * p0.vy;
        p1.fy -= result.fy + dampen * p1.vy * p1.vy * p1.vy;
        return result;
    }

    /*point contact(point &p0, point &p1, float hardness = 5e3f){
        point result;
        point zero;
        bool isContact;
        float d;
        float dx;
        float dy;
        float kx;
        float ky;
        float dsq;
        float radius;
        float ratio;
        UINT np = bl[0].size();
        hardness /= np;
        zero.fx = 0;
        zero.fy = 0;
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        dsq = dx*dx + dy*dy;
        if (dsq == 0) dsq = 1e-10f;
        isContact = false;
        radius = p0.radius + p1.radius;
        //check contact
        d = distance(p0.x, p0.y, p1.x, p1.y);
        //if (d < radius*0.7f) d = radius*0.9f;
        if (d < radius){
            ratio = (d/radius);
            kx = dx * (-1.0f + ratio);
            ky = dy * (-1.0f + ratio);
            result.fx = hardness * sk * kx;
            result.fy = hardness * sk * ky;
            p0.fx += result.fx;
            p0.fy += result.fy;
            p1.fx -= result.fx;
            p1.fy -= result.fy;
            return result;
        } else {
            return zero;
        }
    }*/

    void writeCoords(){
        ofstream result;
        string outputFigData = blSettings.fname + ".data";
        fileText datafile = saveFigure();
        result.open(outputFigData.c_str());
        result.write(datafile.getText().c_str(), datafile.getText().size());
        result.close();
    }

    point contact(point &p0, point &p1, float hardness = 5e1f)
    {
        point result;
        initPoint(&result);
        point zero;
        initPoint(&zero);
        float d;
        float dx;
        float dy;
        float radius;
        float kx, ky;
        float crat;
        zero.fx = 0;
        zero.fy = 0;
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        radius = p0.radius + p1.radius;
        radius += blSettings.margin;
        //check contact
        d = distance(p0.x, p0.y, p1.x, p1.y);


        if (d <= (radius))
        {
            if (d <= radius){
                blSettings.contacts++;
                crat = (d - radius)/(radius);
                kx = dx * (crat);
                ky = dy * (crat);
                result.fx = hardness * sk * kx;
                result.fy = hardness * sk * ky;
                p0.fx += result.fx;
                p0.fy += result.fy;
                p1.fx -= result.fx;
                p1.fy -= result.fy;
                p0.inContact = true;
                p1.inContact = true;
//                attention(p0.x, p0.y, 1, 1);
            }
            else{
              if (p0.inContact == true){
                p0.softenVel = true;
                p0.inContact = false;
              }
              if (p1.inContact == true){
                p1.softenVel = true;
                p1.inContact = false;
              }
            }
            return result;
        }
        else
        {
          if (p0.inContact == true){
            p0.softenVel = true;
            p0.inContact = false;
          }
          if (p1.inContact == true){
            p1.softenVel = true;
            p1.inContact = false;
          }
          return zero;
        }
    }

    point rope(point &circ, point &p0, point &p1)
    {
        float a, b;
        float d;
        point result;
        initPoint(&result);
        point zero;
        initPoint(&zero);
        point virt;
        initPoint(&virt);
        float d0, d1;
        a = (p1.y - p0.y)/(p1.x - p0.x);
        b = p0.y - a * p0.x;
        virt.x = ((circ.x + a*circ.y - a * b)/(a*a + 1));
        virt.y = (a * circ.x + a*a*circ.y + b)/(a*a + 1);
        d0 = distance(p0.x, p0.y, virt.x, virt.y) /
             distance(p0.x, p0.y, p1.x, p1.y);
        d1 = distance(p1.x, p1.y, virt.x, virt.y) /
             distance(p0.x, p0.y, p1.x, p1.y);
        virt.vx = p0.vx + (p1.vx-p0.vx) * d0;
        virt.fx = p0.fx + (p1.fx-p0.fx) * d0;
        virt.vy = p0.vy - (p1.vy-p0.vy) * d0;
        virt.fy = p0.fy - (p1.fy-p0.fy) * d0;
        virt.mass = p0.mass;
        d = distance(circ.x, circ.y, virt.x, virt.y);
        if (d < circ.radius && isInside(p0, p1, virt))
        {
            result = contact(circ, virt);
            p0.fx = -d1 * result.fx;
            p0.fy = -d1 * result.fy;
            p1.fx = -d0 * result.fx;
            p1.fy = -d0 * result.fy;
            //attention(p0.x, p0.y, p0.x+1e-3*p0.fx, p0.y+1e-3*p0.fy);
            //attention(p1.x, p1.y, p1.x+1e-3*p1.fx, p1.y+1e-3*p1.fy);
            //Sleep(100);
            return result;
        }
        else
        {
            return zero;
        }
    }

    void setRadii(){
      UINT i, j;
      for (i = 0; i < bl.size(); i++){
        for (j = 0; j < bl[i].size(); j++){
          bl[i][j].radius = blSettings.marginScale * i;
        }
      }
    }

    void setAsStable(){
      dt *= blSettings.stepdt;
      deciderCounter.setAsStable();
    }

    void setForces1()
    {
        UINT i, j;
        UINT size;
        point f;
        initPoint(&f);
        //line points
        /*******/
        float damp = sk / 20;
        for (i = 0; i < bl.size(); i++)
        {
            //first point
            size = bl[i].size();
            f = spring(bl[i][0], bl[i][size-1], damp);
            //rest of the points
            for (j = 1; j < size; j++)
            {
                f = spring(bl[i][j], bl[i][j-1], damp);
            }
        }

    }

    void setContacts()
    {
        UINT i, j, k;
        int size;
        point f;
        initPoint(&f);

        //contact forces
        /*******/
        //line-circle

        for (k = 0; k < bl.size(); k++)
        {
            size = bl[k].size();
            for (j = 0; j < circles.size(); j++)
            {
                if (circles[j].radius > 0)
                {
                    /******first line with circles****/
                    f = rope(circles[j], bl[k][size-1], bl[k][0]);
                    /******/
                    for (i = 0; i < bl[k].size(); i++)
                    {
                        /****rest of the lines with circles**/
                        if (i > 0)
                        {
                            f = rope(circles[j], bl[k][i-1], bl[k][i]);
                        }
                        /****line points with circles**/
                        f = contact(circles[j], bl[k][i]);
                        /******/
                    }

                }
            }
        }
        /*******/
        //circle-circle
        for (i = 0; i < circles.size() - 1; i++)
        {
            for (j = i + 1; j < circles.size(); j++)
            {
                //f = eqforce(circles[i], circles[j]);
                f = contact(circles[i], circles[j]);
            }
        }
        /*******/
        if (blSettings.fixCircles){
          for (i = 0; i < circles.size(); i++){
            circles[i].fx = 0;
            circles[i].fy = 0;
            circles[i].vx = 0;
            circles[i].vy = 0;
          }
        }
    }
    int nextPoint(UINT i, UINT j){
      UINT result = j + 1;
      if (result >= bl[i].size()){
        result = 0;
      }
      return result;
    }

    int prevPoint(int i, int j){
      int result = j - 1;
      if (result < 0){
        result = bl[i].size() - 1;
      }
      return result;
    }


    bool sticking(int i, int j, int k){
      int c;
      bool prev = false;
      float rad = bl[i][j].radius + circles[k].radius + blSettings.marginScale * i;
      int l = j;
      for (c = 0; c < blSettings.checkFor; c++){
        l = prevPoint(i, l);
        float prad = distance(bl[i][l].x, bl[i][l].y, circles[k].x, circles[k].y);
        if (prad < (rad)){
          prev = true;
        }
      }
      bool next = false;
      l = j;
      for (c = 0; c < blSettings.checkFor; c++){
        l = nextPoint(i, l);
        float prad = distance(bl[i][l].x, bl[i][l].y, circles[k].x, circles[k].y);
        if (prad < (rad)){
          next = true;
        }
      }
      bool result = prev & next;
      return result;
    }

    void setForces3()
    {
        UINT i, j, k;
        point p1, p2;
        initPoint(&p1); initPoint(&p2);
        /*******/
        for (i = 0; i < bl.size(); i++)
        {
            for (j = 0; j < bl[i].size(); j++)
            {
                bl[i][j].cancelForce = false; // Reuse to point those points inside a circle
                k = closestToSurf(i,j);
                float rad = bl[i][j].radius + circles[k].radius + blSettings.marginScale * i;
                float prad = distance(bl[i][j].x, bl[i][j].y, circles[k].x, circles[k].y);
                bool st = sticking(i, j, k);
                if (st){
                  float rat = rad / prad;
                  bl[i][j].x = circles[k].x + rat * (bl[i][j].x - circles[k].x);
                  bl[i][j].y = circles[k].y + rat * (bl[i][j].y - circles[k].y);
                  bl[i][j].cancelForce = true;
                }
            }
            // Soften borders
            for (j = 0; j < bl[i].size(); j++)
            {
              if (bl[i][j].cancelForce == true){
                int k = prevPoint(i, j);
                int l = prevPoint(i, k);
                int m = nextPoint(i, j);
                int n = nextPoint(i, m);

                if (bl[i][k].cancelForce == false && bl[i][l].cancelForce == false){
                  bl[i][j].x = (bl[i][k].x + bl[i][m].x) / 2;
                  bl[i][j].y = (bl[i][k].y + bl[i][m].y) / 2;
                  bl[i][k].x = (bl[i][l].x + bl[i][j].x) / 2;
                  bl[i][k].y = (bl[i][l].y + bl[i][j].y) / 2;
                }
                if (bl[i][m].cancelForce == false && bl[i][n].cancelForce == false){
                  bl[i][j].x = (bl[i][k].x + bl[i][m].x) / 2;
                  bl[i][j].y = (bl[i][k].y + bl[i][m].y) / 2;
                  bl[i][m].x = (bl[i][n].x + bl[i][j].x) / 2;
                  bl[i][m].y = (bl[i][n].y + bl[i][j].y) / 2;
                }
              }
            }
        }
    }

    int closestToSurf(UINT i, UINT j){
      // Get the closest circle to a line point
      UINT result = 0;
      point P = bl[i][j];
      while (circles[result].radius == 0 && result < circles.size()){
        result++;
      }
      float mindist = distance(P.x, P.y, circles[result].x, circles[result].y) - circles[result].radius;
      UINT k = result;
      while (k < circles.size()){
        float dst = distance(P.x, P.y, circles[k].x, circles[k].y) - circles[k].radius;
        if (dst < mindist){
          mindist = dst;
          result = k;
        }
        k++;
      }
      return result;
    }

    void setForces2()
    {
        UINT i, j, k;
        UINT i1, i2;
        point f;
        initPoint(&f);
        point previous;
        initPoint(&previous);
        point next;
        initPoint(&next);
        string temp;
        blSettings.fixCircles = true;
        float dampen = sk / 5;
        /*******/
        //line-circle
        for (k = 0; k < bl.size(); k++)
        {
            for (i = 0; i < bl[k].size(); i++)
            {
                //j = closestToSurf(k, i);
                for (j = 0; j < circles.size(); j++)
                {
                    if (circles[j].radius > 0)
                    {
                        f = eqforce(circles[j], bl[k][i], 1e-5f * sk);
                        if (i > 0)
                        {
                            i1 = i - 1;
                        }
                        else
                        {
                            i1 = bl[k].size() - 1;
                        }
                        if (i < bl[k].size() - 1)
                        {
                            i2 = i + 1;
                        }
                        else
                        {
                            i1 = 0;
                        }
                        f = spring(bl[k][i1], bl[k][i], dampen);
                        f = spring(bl[k][i], bl[k][i2], dampen);

                    }
                }
            }
        }
        /** Circle-circle
        for (i = 0; i < circles.size() - 1; i++){
          float tmp = circles[i].radius;
          circles[i].radius += 2 * ngroups * Scale;
          for (j = i + 1; j < circles.size(); j++){
            float tmp2 = circles[j].radius;
            circles[j].radius += 2 * ngroups * Scale;
            f = contact(circles[i], circles[j], 1e15f * sk);
            circles[j].radius = tmp2;
          }
          circles[i].radius = tmp;
        }
        **/
    }

    void limitVel(point &P, float maxv)
    {
        float f;
        f = P.vx * P.vx + P.vy * P.vy;
        if (f > maxv*maxv)
        {
            f = sqrt(f);
            P.vx *= maxv/f;
            P.vy *= maxv/f;
            //attention(P.x, P.y);
        }
    }

    void limitForce(point &P, float maxf)
    {
        float f;
        f = P.fx * P.fx + P.fy * P.fy;
        if (f > maxf*maxf)
        {
            f = sqrt(f);
            P.fx *= maxf/f;
            P.fy *= maxf/f;
        }
    }


    void solve(bool resetVelocity = false)
    {
        UINT i;
        float kb = baseBV;
        for (i = 0; i < dataDisplay.size(); i++){
          free(dataDisplay[i]);
        }
        dataDisplay.clear();

        //Init the scale for the new frame
        internalScale.setClear(true);
        setContacts();
        updPos(kb, resetVelocity);
        clearForces();
//Show dt
        displayFloat("DT", dt);
        displayUINT("CYCLES", blSettings.ncycles);
        if (checkTopol() || blSettings.surfRatio > (2 * blSettings.minSurfRatio)
                         || internalScale.xSpan() > (2 * blSettings.minDx)
                         || internalScale.ySpan() > (2 * blSettings.minDy))
        {
            bl = bl_old10;
            circles = circles_old10;
            udt.report(dt);
            dt *= blSettings.stepdt;
            deciderCounter = 0;
        }
        else if (blCounter == 0)
        {
            bl_old10 = bl_old5;
            bl_old5 = bl;
            resetOld();
            circles_old10 = circles_old5;
            circles_old5 = circles;
        }
        else if (deciderCounter.isMax())
        {
            if (dt/blSettings.stepdt < udt.unstabledt())
            {
                dt /= blSettings.stepdt;
            }
        }
        resetCircleRadius();
        blSettings.surfRatio = estSurf();
        if (blSettings.minSurfRatio == 0){
          blSettings.minSurfRatio = blSettings.surfRatio;
        }
        if (blSettings.minDx == 0) blSettings.minDx = internalScale.xSpan();
        if (blSettings.minDy == 0) blSettings.minDy = internalScale.ySpan();
        if(blSettings.minSurfRatio > blSettings.surfRatio){
          blSettings.minSurfRatio = blSettings.surfRatio;
        }
        if (blSettings.minDx > internalScale.xSpan()) blSettings.minDx = internalScale.xSpan();
        if (blSettings.minDy > internalScale.ySpan()) blSettings.minDy = internalScale.ySpan();
        blCounter++;
        deciderCounter++;
        blSettings.ncycles++;
    }

    void updPos(float kb, bool resetVelocity)
    {
        UINT i, j;
        //Apply each force to each point
        for (i = 0; i < bl.size(); i++)
        {
            for (j = 0; j < bl[i].size(); j++)
            {
                bl[i][j].fx -= kb * bl[i][j].vx;
                bl[i][j].fy -= kb * bl[i][j].vy;

                //Limit force to avoid artifacts
                if (bl[i][j].softenVel == true){
                  limitVel(bl[i][j], blSettings.maxv);
                  bl[i][j].softenVel = false;
                }
                //

                bl[i][j].vx += bl[i][j].fx * dt / bl[i][j].mass;
                bl[i][j].vy += bl[i][j].fy * dt / bl[i][j].mass;
                blSettings.totalLineV += bl[i][j].vx * bl[i][j].vx + bl[i][j].vy * bl[i][j].vy;

                bl[i][j].x += bl[i][j].vx * dt;
                bl[i][j].y += bl[i][j].vy * dt;
                /*******/
                //attention(bl[i][j].x, bl[i][j].y, 0.1);
                /*******/
                //limitVel(bl[i][j], maxv);
                //Prepare the scale for the new frame
                setScale(bl[i][j]);

                if (resetVelocity)
                {
                    bl[i][j].vx = 0;
                    bl[i][j].vy = 0;
                }
            }

            //float p = perimeter(bl[i], true);
            //char* t = (char*) calloc(100, sizeof(char));
            //sprintf(t, "P%u: %.4f", i, p);
            //dataDisplay.insert(dataDisplay.end(), t);
        }
        for (i = 0; i < circles.size(); i++)
        {

            /*******
            limitForce(circles[i], maxf);
            *******/
            circles[i].fx -= kb * circles[i].vx;
            circles[i].fy -= kb * circles[i].vy;
            //Limit force to avoid artifacts
            if (circles[i].softenVel == true){
              //limitVel(circle[i], blSettings.maxv);
              circles[i].softenVel = false;
            }
            //
            circles[i].vx += circles[i].fx * dt / (CIRCLE_MASS);
            circles[i].vy += circles[i].fy * dt / (CIRCLE_MASS);
            blSettings.totalCircleV += circles[i].vx * circles[i].vx + circles[i].vy * circles[i].vy;
            //limitVel(circles[i], maxv);

            circles[i].x += circles[i].vx * dt;
            circles[i].y += circles[i].vy * dt;
            if (resetVelocity)
            {
                circles[i].vx = 0;
                circles[i].vy = 0;
            }
        }
        displayFloat("LINEV", blSettings.totalLineV);
        displayFloat("MINRAT", minRat);
        blSettings.totalCircleV = 0;
        blSettings.totalLineV = 0;
    }

    float estSurf(int nPoints = 100){
      float tsurf = (float) (internalScale.xSpan() * internalScale.ySpan());
      tsurf /= (float) totalExpectedSurface;
      return tsurf;
    }

    bool isTopolCorrect(point P, vector<int> belong){
      UINT j, k;
      int lastPoint;
      bool mustBeIn;  //Must the circle be inside the curve?
      bool isIn;      //Is the circle inside the curve?
      bool ch;        //Must isIn change?
      bool isAbove;   //Is the curve point above the circle center?
      bool isRight;   //Is the curve point to the right of the circle center?
      float a, b;
      point previousPoint;
      initPoint(&previousPoint);
      point testPoint;
      initPoint(&testPoint);
      for (j = 0; j < bl.size(); j++)
      {
          lastPoint = bl[j].size()-1;
          isIn = false;
          mustBeIn = (belong[j] == 1);
          isAbove = (bl[j][lastPoint].y > P.y);
          isRight = (bl[j][lastPoint].x > P.x);
          for (k = 0; k < bl[j].size(); k++)
          {
              ch = (bl[j][k].y > P.y);
              ch = (ch ^ isAbove);
              if (ch == true)
              {    /* Lines cross */
                  isAbove = !isAbove;
                  if ((bl[j][k].x > P.x) && isRight)
                  { /* Not an extreme point */
                      isIn = !isIn;
                  }
                  else if ((bl[j][k].x > P.x) ^ isRight)
                  { /*Extreme point */
                      if (k > 0)
                      {
                          previousPoint = bl[j][k - 1];
                      }
                      else
                      {
                          previousPoint = bl[j][lastPoint];
                      }
                      a = (bl[j][k].y - previousPoint.y)/
                          (bl[j][k].x - previousPoint.x);
                      b = bl[j][k].y - a * bl[j][k].x;
                      testPoint.y = P.y;
                      testPoint.x = (testPoint.y - b) / a;
                      if (testPoint.x > P.x
                              && isInside(previousPoint, bl[j][k], testPoint))
                      {
                          isIn = !isIn;
                      }
                  }
              }   /* Lines cross */
              isRight  = (bl[j][k].x > P.x);
          }
          if (isIn ^ mustBeIn)
          {
              //attention(circles[i].x, circles[i].y, 2);
              //Sleep(1000);
              return true;
          }
      }
      return false;
    }

    bool checkTopol()
    {
      UINT i;
      for (i = 0; i < circles.size(); i++)
      {
          if (circles[i].radius > 0)
          { /* Circle has radius */
              vector<int> belong = toBin(circles[i].n, bl.size()); // This might be taken out and calculated only once. Does not take long, though
              bool result = isTopolCorrect(circles[i], belong);
              if (result){
                return result;
              }
          } /* Circle has radius */
      }
      return false;
    }

    void wlimit(){
      UINT i;
      float wmax = 0;
      for (i = 0; i < w.size(); i++){
        if (w[i] > 0 && w[i] > wmax) wmax = w[i];
      }
      for (i = 0; i < w.size(); i++){
        if (w[i] > 0 && w[i] < (wmax * blSettings.minratio)) w[i] = blSettings.minratio * wmax;
      }
    }

public:
    borderLine(){}
    borderLine(binMap* b, vector<string> g, vector<float> tw, string inputFile = "venn.txt", string outputFile = "result.svg")
    {
        UINT i;
        bm = b;
        groups = g;
        ngroups = bm->ngroups;
        internalScale.initScale();
        initBlData(&blSettings);
        minRat = 0;
        blSettings.signalEnd = false;
        blSettings.contacts = 0;
        blSettings.fixCircles = false;
        blSettings.minratio = 0.1f * (ngroups * ngroups * ngroups)/ (4 * 4 * 4);
        blSettings.marginScale = 0.02f;
        blSettings.totalCircleV = 0;
        blSettings.totalLineV   = 0;
        blSettings.minSurfRatio = 0;
        blSettings.maxf = 5e20f;
        blSettings.maxv = 5e0f;
        blSettings.margin = 1.2 * ngroups * blSettings.marginScale;
        blSettings.startdt = dt;
        blSettings.stepdt = 0.6f;
        blSettings.inputFile = inputFile;
        blSettings.fname = outputFile;
        blSettings.ncycles = 0;
        srand(time(0));
        w = tw;         //keep a copy of the weights
        for (i = 0; i < tw.size(); i++){
          origw.insert(origw.end(), tw[i]);
        }
        wlimit();



        //init circles
        setCircles(*bm, origw);

        totalExpectedSurface = 0;
        for (i = 0; i < w.size(); i++){
          totalExpectedSurface += (int) (circles[i].radius * circles[i].radius);
        }

        //init counters
        blCounter.setLimits(0, 5u);
        deciderCounter.setLimits(0, 5u);
        refreshScreen.setLimits(1, 50);

        //init internal scale
        internalScale.setClear(true);

        //init time parameters
        udt.init(blSettings.startdt);

        //init points
        for (i = 0; i < ngroups; i++)
        {
            p.clear();
            setPoints(*bm, i);
            bl.insert(bl.end(), p);
        }
        startPerim = (UINT) perimeter(bl[0]);
        UINT np = (UINT) (0.5f * (float) startPerim);
        interpolate(np);


        int arr[] = {
          0xE6194B,
          0x3CB44B,
          0xffe119,
          0x0082c8,
          0xf58231,
          0x911eb4,
          0x46f0f0,
          0xf032e6,
          0xd2f53c,
          0xfabebe,
          0x008080,
          0xe6beff,
          0xaa6e28,
          0xfffac8,
          0x800000,
          0xaaffc3,
          0x808000,
          0xffd8b1,
          0x000080,
          0x808080,
          0xFFFFFF,
          0x000000
        };
        for (i = 0; i < ngroups; i++){
          char c[8];
          sprintf(c, "#%06x", arr[i]);
          svgcolors.insert(svgcolors.end(), c);
        }
        //init colors
        for (i = 0; i < ngroups; i++)
        {
          colors.insert(colors.end(),toRGB(arr[i], 1));
        }
        /*writeSVG()*/
    }

    bool isThisTheEnd(){
      return blSettings.signalEnd;
    }

    vector<vector<point> > getPoints(){
      return bl;
    }

    void setCoords(string dataFile){
        ifstream vFile;
        vFile.open(dataFile.c_str());
        UINT ncI = 0;
        if (vFile.good() == true){
            bl.clear();
            bl_old5.clear();
            bl_old10.clear();
            string line;
            getline(vFile, line); // _F
            getline(vFile, line); // ncyclesInterrupted or _L
            if (line != "_L"){
                int c = atoi(line.c_str());
                if (c > 0) ncI = (UINT) c;
                blSettings.ncyclesInterrupted = ncI;
                getline(vFile, line); // _L
            }
            while (line == "_L" && vFile.eof() == false){
                vector<point> thisline;
                getline(vFile, line); // First x coord
                while (line != "_L" && line != "_C" && vFile.eof() == false){
                    float x = atof(line.c_str());
                    getline(vFile, line);
                    float y = atof(line.c_str());
                    point p; initPoint(&p);
                    p.x = x; p.y = y;
                    setScale(p);
                    thisline.insert(thisline.end(), p);
                    getline(vFile, line);
                }
                bl.insert(bl.end(), thisline);
            }
            if (line == "_C"){
                getline(vFile, line);
                int i = 0;
                while (vFile.eof() == false){
                    float x = atof(line.c_str());
                    getline(vFile, line);
                    float y = atof(line.c_str());
                    getline(vFile, line);
                    float r = atof(line.c_str());
                    getline(vFile, line);
                    circles[i].x = x; circles[i].y = y; circles[i].radius = r;
                    i++;
                }
            }
        }
    }

    fileText saveFigure(){
        fileText result;
        result.addLine("_F");
        string nc = UINT2string(blSettings.ncyclesInterrupted);
        result.addLine(nc);
        UINT i; UINT j;
        for (i = 0; i < bl.size(); i++){
            result.addLine("_L");
            for (j = 0; j < bl[i].size(); j++){
                string x = float2string(bl[i][j].x);
                string y = float2string(bl[i][j].y);
                result.addLine(x);
                result.addLine(y);
            }
        }
        result.addLine("_C");
        for (i = 0; i < circles.size(); i++){
            string x = float2string(circles[i].x);
            string y = float2string(circles[i].y);
            string r = float2string(circles[i].radius);
            result.addLine(x);
            result.addLine(y);
            result.addLine(r);
        }
        return result;
    }


    string coord(float c){
      char t[500];
      sprintf(t, "%.2f", c);
      string result = t;
      return result;
    }

    string num(int c){
      char t[500];
      sprintf(t, "%d", c);
      string result = t;
      return result;
    }

    point fstCtrlPoint(point prev, point start, point nxt, float sc = 0.5f){
      point result;
      initPoint(&result);
      result.x = start.x + sc * (nxt.x - prev.x);
      result.y = start.y + sc * (nxt.y - prev.y);
      return result;
    }

    point scndCtrlPoint(point prev, point start, point nxt, float sc = 0.5f){
      point result;
      initPoint(&result);
      result.x = start.x - sc * (nxt.x - prev.x);
      result.y = start.y - sc * (nxt.y - prev.y);
      return result;
    }

    string join(string interm, vector<string> arr){
      int i;
      string result;
      int j = arr.size() - 1;
      for (i = 0; i < j; i++){
        result += arr[i] + interm;
      }
      result += arr[j];
      return result;
    }

    fileText toSVG(){
      fileText svg;
      char temp[512];
      scale sc;
      sc.initScale();
      sc.setMinX(10.0f);
      sc.setMinY(10.0f);
      sc.setMaxX(490.0f);
      sc.setMaxY(490.0f);
      int fsize = 10;
      UINT i, j;
      string tst;
      point svgtemp;
      initPoint(&svgtemp);
      svg.addLine("<svg width=\"700\" height=\"500\">");
      svg.addLine("<defs>");
      svg.addLine("<style type=\"text/css\"><![CDATA[");
      svg.addLine("  .borderLine {");
      svg.addLine("	   stroke: none;");
      svg.addLine("	   fill-opacity: 0.4;");
      svg.addLine("  }");
      svg.addLine("  .outLine {");
      svg.addLine("	   stroke-width: 1;");
      svg.addLine("	   fill: none;");
      svg.addLine("  }");
      svg.addLine("  .circle {");
      svg.addLine("	   stroke: #888888;");
      svg.addLine("	   stroke-width: 0.5;");
      svg.addLine("	   fill: none;");
      svg.addLine("    pointer-events: all;");
      svg.addLine("  }");
      svg.addLine("  .nLabel {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      char t[200];
      sprintf(t, "	   font-size: %dpx;", fsize);
      svg.addLine((string) t);
      svg.addLine("	   text-anchor: middle;");
      svg.addLine("	   alignment-baseline: central;");
      svg.addLine("  }");
      svg.addLine("  .belong {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      sprintf(t, "	   font-size: %dpx;", fsize / 2);
      svg.addLine((string) t);
      svg.addLine("	   text-anchor: middle;");
      svg.addLine("	   alignment-baseline: central;");
      svg.addLine("  }");
      for (i = 0; i < ngroups; i++){
        svg.addLine("  .p" + num(i) + "{");
        svg.addLine("    stroke: none;");
        svg.addLine("    fill: " + svgcolors[i] + ";");
        svg.addLine("  }");
        svg.addLine("  .q" + num(i) + "{");
        svg.addLine("    fill: none;");
        svg.addLine("    stroke: " + svgcolors[i] + ";");
        svg.addLine("  }");
      }
      svg.addLine("]]>");
      svg.addLine("</style>");
      string nc = bool2string(blSettings.signalEnd);
      /* This softens the lines*/
      if (blSettings.smoothSVG == true){
        for (i = 0; i < ngroups; i++){
          point nxt = place(sc, bl[i][0]);
          string cpath = "M " + coord(nxt.x) + " " + coord(nxt.y);

          for (j = 1; j < (bl[i].size() - 2); j++){
            point prev = place(sc, bl[i][j - 1]);
            point curr = place(sc, bl[i][j]);
            point next = place(sc, bl[i][j + 1]);
            point next2 = place(sc, bl[i][j + 2]);
            point ctrlfst = fstCtrlPoint(prev, curr, next);
            point ctrlsec = scndCtrlPoint(curr, next, next2);
            cpath += " C " + coord(ctrlfst.x) + " " + coord(ctrlfst.y) + " " +
                             coord(ctrlsec.x) + " " + coord(ctrlsec.y) + " " +
                             coord(next.x) + " " + coord(next.y);
          }
          svg.addLine("<symbol id=\"bl" + num(i) + "\">");
          svg.addLine("<path class=\"p" + num(i) + " borderLine\" d=\"" + cpath + " Z\" />");
          svg.addLine("</symbol>");
        }
      } else{
        /* This does not */
        for (i = 0; i < ngroups; i++){
          point nxt = place(sc, bl[i][0]);
          string cpath = "M " + coord(nxt.x) + " " + coord(nxt.y);
          for (j = 1; j < bl[i].size(); j++){
            nxt = place(sc, bl[i][j]);
            cpath += " L " + coord(nxt.x) + " " + coord(nxt.y);
          }
          svg.addLine("<symbol id=\"bl" + num(i) + "\">");
          svg.addLine("<path d=\"" + cpath + " Z\" />");
          svg.addLine("</symbol>");
        }
      }
      svg.addLine("</defs>");
      svg.addLine("<!-- isDone: " + nc + " -->");
      svg.addLine("<rect width=\"700\" height=\"500\" style=\"fill:#fff;stroke-width:0\" />");

      // Add fills
      for (i = 0; i < ngroups; i++){
        svg.addLine("<use class=\"p" + num(i) + " borderLine\" xlink:href=\"#bl" + num(i) + "\"/>");
      }
      // Add strokes
      for (i = 0; i < ngroups; i++){
        svg.addLine("<use class=\"q" + num(i) + "\" xlink:href=\"#bl" + num(i) + "\"/>");
      }
      for (i = 0; i < circles.size(); i++){
        //printf("%d\n", i);
        if (circles[i].radius > 0){
          svgtemp = place(sc, circles[i]);
          //printf("%.4f, %.4f, %.4f\n", svgtemp.x, sc.minX, sc.maxX);
          if (svgtemp.x > sc.minX() && svgtemp.x < sc.maxX()){
            sprintf(temp, "<circle onclick=\"fromCircle(%u)\" class=\"circle\" cx=\"%.4f\" cy=\"%.4f\" r=\"%.4f\" />", circles[i].n, svgtemp.x,
                            svgtemp.y, svgtemp.radius);
            tst = temp;
            svg.addLine(tst);
            char addNum[200];
            sprintf(addNum, "<text class=\"nLabel\" x=\"%.2f\" y=\"%.2f\">%g</text>", svgtemp.x, svgtemp.y - fsize/2, circles[i].orig);
            tst = addNum;
            svg.addLine(tst);
            // Belongs to
            vector<int> tb = toBin(circles[i].n, bl.size());
            vector<string> belongs;
            UINT m;
            for (m = 0; m < tb.size(); m++){
              if (tb[m] > 0){
                char t[100];
                sprintf(t, "%d", m + 1);
                belongs.insert(belongs.end(), (string) t);
              }
            }
            string bgs = join(", ", belongs);
            char addBelongs[500];
            sprintf(addBelongs, "<text class=\"belong\" x=\"%.2f\" y=\"%.2f\">(%s)</text>", svgtemp.x, svgtemp.y + fsize / 2, bgs.c_str());
            svg.addLine(addBelongs);
          }
        }
      }
      // Legend
      UINT l;
      float cx = 500.0f;
      float cy = 50.0f;
      float rw = 30.0f;
      float rh = 15.0f;
      float dy = 40.0f;
      float dx = 40.0f;
      for (l = 0; l < ngroups; l++){
        string g = groups[l];
        char myg[50]; sprintf(myg, "p%d", l);
        char addRect[500];
        sprintf(addRect, "<rect class=\"%s borderLine\" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />",
                myg, cx, cy, rw, rh);
        char addLegend[500];
        sprintf(addLegend, "<text class=\"legend\" x=\"%.2f\" y=\"%.2f\">%s</text>", cx + dx, cy + rh, g.c_str());
        svg.addLine(addRect);
        svg.addLine(addLegend);
        cy += dy;
      }
      svg.addLine("</svg>");
      return svg;
    }

    fileText toPS()
    {
        fileText pstext;
        char temp[512];
        string tst;
        UINT i, j;
        point pstemp;
        initPoint(&pstemp);
        scale ps;
        ps.initScale();
        ps.setMinX(72.0f);
        ps.setMinY(72.0f);
        ps.setMaxX(551.0f);
        ps.setMaxY(721.0f);

        // Postscript header
        pstext.addLine(" ");
        pstext.addLine("%!PS-Adobe-2.0");
        pstext.addLine(" ");
        pstext.addLine("/scale 1 def");
        pstext.addLine("/ury 0  def");
        pstext.addLine("/urx 0  def");
        pstext.addLine("/lly 900  def");
        pstext.addLine("/llx 900  def");
        pstext.addLine(" ");
        pstext.addLine("/showline{     %draws a line from an array of points");
        pstext.addLine("topath          %stack: point_array. rgb_color");
        pstext.addLine("setrgbcolor");
        pstext.addLine("1 setlinejoin");
        pstext.addLine("stroke");
        pstext.addLine("} def");
        pstext.addLine(" ");
        pstext.addLine("/topath{        %makes a path from an array of points");
        pstext.addLine("/tmat exch def  ");
        pstext.addLine("newpath");
        pstext.addLine("tmat 0 get scale mul tmat 1 get scale mul moveto");
        pstext.addLine("2 2 tmat length 1 sub");
        pstext.addLine("{");
        pstext.addLine("dup");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul /x exch def");
        pstext.addLine("1 add");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul /y exch def");
        pstext.addLine("x y lineto");
        pstext.addLine("}for");
        pstext.addLine("closepath");
        pstext.addLine("}def");
        pstext.addLine(" ");
        pstext.addLine("/showp{                %draws points from an array");
        pstext.addLine("/tmat exch def         %stack: point_array. rgb_color");
        pstext.addLine("/b exch def");
        pstext.addLine("/g exch def");
        pstext.addLine("/r exch def");
        pstext.addLine("0 2 tmat length 1 sub");
        pstext.addLine("{");
        pstext.addLine("dup");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul /x exch def");
        pstext.addLine("1 add");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul neg /y exch def");
        pstext.addLine("newpath");
        pstext.addLine("x y 1 0 360 arc");
        pstext.addLine("r g b setrgbcolor");
        pstext.addLine("stroke");
        pstext.addLine("}for");
        pstext.addLine("}def");
        pstext.addLine(" ");
        pstext.addLine("/patternshape{");
        pstext.addLine("/size 0.5 def");
        pstext.addLine("moveto");
        pstext.addLine("size 2 div size 2 div rmoveto");
        pstext.addLine("0 size neg rlineto");
        pstext.addLine("size neg 0 rlineto");
        pstext.addLine("0 size rlineto");
        pstext.addLine("size 0 rlineto");
        pstext.addLine("} def");
        pstext.addLine("");
        pstext.addLine("/colorpattern{");
        pstext.addLine("/offsety exch def");
        pstext.addLine("/offsetx exch def");
        pstext.addLine("/b exch def");
        pstext.addLine("/g exch def");
        pstext.addLine("/r exch def");
        char* st = (char*) calloc(100, sizeof(char));
        sprintf(st, "/step %u def", ngroups);
        pstext.addLine(st);
        free(st);
        pstext.addLine("offsetx llx add step urx offsetx add{");
        pstext.addLine("/x exch def");
        pstext.addLine(" offsety lly add step ury offsety add{");
        pstext.addLine(" /y exch def");
        pstext.addLine("  newpath");
        pstext.addLine("  x y patternshape");
        pstext.addLine("  r g b setrgbcolor");
        pstext.addLine("  fill");
        pstext.addLine(" } for");
        pstext.addLine("} for");
        pstext.addLine("} def");
        pstext.addLine("");
        pstext.addLine("");
        pstext.addLine("");
        pstext.addLine("/minmax{");
        pstext.addLine("/tmat exch def");
        pstext.addLine("");
        pstext.addLine("0 2 tmat length 1 sub");
        pstext.addLine("{");
        pstext.addLine(" dup");
        pstext.addLine(" tmat exch get");
        pstext.addLine(" scale mul /x exch def");
        pstext.addLine(" 1 add");
        pstext.addLine(" tmat exch get");
        pstext.addLine(" scale mul /y exch def");
        pstext.addLine(" x urx gt{");
        pstext.addLine(" /urx x def");
        pstext.addLine(" }if");
        pstext.addLine(" x llx lt{");
        pstext.addLine(" /llx x def");
        pstext.addLine(" }if");
        pstext.addLine(" y ury gt{");
        pstext.addLine(" /ury y def");
        pstext.addLine(" }if");
        pstext.addLine(" y lly lt{");
        pstext.addLine(" /lly y def");
        pstext.addLine(" }if");
        pstext.addLine("}for");
        pstext.addLine("}def");

        // Arrays
        for (i = 0; i < ngroups; i++)
        {
            pstext.addLine("[");
            for (j = 0; j < bl[i].size(); j++)
            {
                pstemp = place(ps, bl[i][j]);
                sprintf(temp, "%f %f", pstemp.x, pstemp.y);
                tst = temp;
                pstext.addLine(tst);
            }
            sprintf(temp, "]");
            tst = temp;
            pstext.addLine(tst);
            sprintf(temp, "/set%d exch def", i + 1);
            tst = temp;
            pstext.addLine(tst);
            sprintf(temp, "set%d minmax", i + 1);
            tst = temp;
            pstext.addLine(tst);
            pstext.addLine(" ");
        }
        pstext.addLine(" ");

        // Draw sets
        pstext.addLine("%Begin program");
        pstext.addLine(" ");

        UINT offset = 0;
        for (i = 0; i < ngroups; i++)
        {
            pstext.addLine("gsave");
            sprintf(temp, "set%d topath", i+1);
            tst = temp;
            pstext.addLine(tst);
            pstext.addLine("clip");
            sprintf(temp, "%f %f %f %u %u colorpattern", colors[i].red,
                            colors[i].green, colors[i].blue, offset, offset);
            tst = temp;
            pstext.addLine(tst);
            pstext.addLine("grestore");
            offset++;
        }
        pstext.addLine(" ");

        for (i = 0; i < ngroups; i++)
        {
            sprintf(temp, "%f %f %f set%d showline", colors[i].red,
                            colors[i].green, colors[i].blue, i+1);
            tst = temp;
            pstext.addLine(tst);
        }
        pstext.addLine(" ");

        /****Draw circles*/
        for (i = 0; i < circles.size(); i++){
            if (circles[i].mass > 0){
              pstemp = place(ps, circles[i]);
              pstext.addLine("newpath");
              if (pstemp.x > ps.minX() && pstemp.x < ps.maxX()){
                sprintf(temp, "%f %f %f 0 360 arc", pstemp.x,
                                pstemp.y, pstemp.radius);
                tst = temp;
                pstext.addLine(tst);
                pstext.addLine("0.2 setlinewidth");
                pstext.addLine("1 0 0 setrgbcolor");
                pstext.addLine("stroke");
              }
            }
        }
        /*****/
        pstext.addLine("showpage");
        return pstext;
    }

    void initPoint(point* p){
      p->cancelForce = false;
      p->fx = 0.0f;
      p->fy = 0.0f;
      p->mass = POINT_MASS;
      p->n = 0;
      p->orig = 0.0f;
      p->radius = 0.0f;
      p->vx = 0.0f;
      p->vy = 0.0f;
      p->x = 0.0f;
      p->y = 0.0f;
      p->inContact = false;
      p->softenVel = false;
    }

    void interpolate(UINT npoints)
    {
        UINT i, j, k;
        float dx, dy;
        float perim;
        float segment;
        UINT interpoints;
        point startPoint;
        initPoint(&startPoint);
        point endPoint;
        initPoint(&endPoint);
        point tempPoint;
        initPoint(&tempPoint);
        vector<point> tempv;
        vector<vector<point> > tempbl;
        for (i = 0; i < bl.size(); i++)
        {
            perim = perimeter(bl[i], true);
            startPoint = bl[i][bl[i].size()-1];
            for (j = 0; j < bl[i].size(); j++)
            {
                endPoint = bl[i][j];
                segment = distance(startPoint.x, startPoint.y,
                                   endPoint.x, endPoint.y);
                interpoints = (int)(segment * npoints / perim);
                if (interpoints == 0)
                    interpoints = 1;
                dx = (float)(endPoint.x - startPoint.x);
                dx = dx / interpoints;
                dy = (float)(endPoint.y - startPoint.y);
                dy = dy / interpoints;
                for (k = 0; k < interpoints; k++)
                {
                    tempPoint.x = startPoint.x + (k * dx);
                    tempPoint.y = startPoint.y + (k * dy);
                    tempPoint.mass = POINT_MASS;
                    tempv.insert(tempv.end(), tempPoint);
                }
                startPoint = endPoint;
            }
            tempbl.insert(tempbl.end(), tempv);
            tempv.clear();
        }
        bl.clear();
        bl = tempbl;
        initOlds();
    }

    void writeSVG(){
        fileText tmp = toSVG();
        ofstream result;
        result.open(blSettings.fname.c_str());
        result.write(tmp.getText().c_str(), tmp.getText().size());
        result.close();

    }

    void simulate(int maxRel = 0)
    {
        UINT i;
        UINT it1 = (UINT) 7e3;
        UINT it2 = (UINT) 2e2;
        point minP;
        initPoint(&minP);
        point maxP;
        initPoint(&maxP);
        printf("Starting...\n");

        // This loop is limited to maxRunningTime
        time_t start = time(NULL);
        if (blSettings.ncyclesInterrupted >= it1) blSettings.ncyclesInterrupted = 0;
        for (i = blSettings.ncyclesInterrupted; i < it1; i++){
          setForces1();
          if (refreshScreen.isMax() == true){
            writeSVG();
            time_t now = time(NULL);
            double elapsed = difftime(now, start);
            if (((UINT) elapsed) > blSettings.maxRunningTime){
              blSettings.ncyclesInterrupted = (UINT) i;
              blSettings.signalEnd = false;
              i = it1;
            }
            writeCoords();
          }
          refreshScreen++;
          if (i == (it1 - 1)){
            blSettings.signalEnd = true;
            blSettings.ncyclesInterrupted = it1;
          }
          solve();
        }

        setAsStable();
        for (i = 0; i < 50; i++){
          setForces1();
          if (refreshScreen.isMax() == true){
            writeSVG();
          }
          refreshScreen++;
          solve();
        }
        printf("Refining...\n");
        UINT np = (UINT) (1.5f * (float) startPerim);
        interpolate(np);
        blSettings.margin /= 10;
        setRadii();
        for (i = 0; i < it2; i++){
          setForces2();
          if (refreshScreen.isMax() == true){
            writeSVG();
          }
          solve(true);
        }
        //setForces3();
        UINT counter;
        for (counter = 0; counter < dataDisplay.size(); counter++){
          free(dataDisplay[counter]);
        }
        dataDisplay.clear();
    }
};


#endif // TOPOL_H_INCLUDED
