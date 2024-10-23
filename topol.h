#ifndef TOPOL_H_INCLUDED
#define TOPOL_H_INCLUDED

#include <vector>
#include <cmath>
#include <time.h>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <random>
#include <set>
#include "debug.h"
#include "scene.h"
//#include <windows.h>

#define AIR 2    // polish and embellish multiply maxRad() by this factor

// Flags for points
#define IS_OUTSIDE        0x01  // The circle is in an incorrect space. Set to 0x01 to highlight
#define DO_NOT_EMBELLISH  0X02  // The point has already been corrected
#define TAKEN_OUT         0x04  // The point has been moved outside
#define USED              0x08  // The point has been used in a procedure. Remember to reset at the end
#define DELME             0x10  // The point will be deleted

#ifndef INGRAVID
#define INGRAVID          0x20
#endif // INGRAVID

#ifndef GHOST
#define GHOST             0x40  // Two points with this bit set will not receive cushion
#endif // GHOST

#define DO_NOT_OPTIMIZE   0x80  // The point has already been optimized

using namespace std;


UINT setFlag(UINT flag, UINT mask){
  UINT result = flag | mask;
  return result;
}

UINT unsetFlag(UINT flag, UINT mask){
  UINT result = flag & (~mask);
  return result;
}



/** \brief Fast computing of 2**n
 *
 * \param n UINT
 * \return UINT
 *
 */
UINT twoPow(UINT n)
{
    if (n > 8*sizeof(UINT)-1)
        return 0;
    UINT result = 1;
    result = result << n;
    return result;
}


bool isNAN(float n){
  return (n != n);
}


/** \brief Scalar product of two vectors
 *
 * \param a vector<float>
 * \param b vector<float>
 * \return float
 *
 */
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
        result.push_back(arr[i]);
    }
    return result;
}


/** \brief Converts a number into a vector of integers
 * with the binary representation of the number
 *
 * \param number int
 * \param 0 UINT nBits=
 * \return vector<int>
 *
 */
vector<int> toBin(int number, UINT nBits = 0)
{
    //  Takes an integer and returns a vector containing
    //  its binary representation

    UINT i;                          // iterate over each bit
    int bit;                        // temp storage of each bit
    vector<int> bits;               // return vector
    while (number > 0)
    {
        bit = number & 1;
        number = number >> 1;
        bits.push_back(bit);
    }
    if (nBits > bits.size())
    {
        for (i = bits.size(); i < nBits; i++)
        {
            bits.push_back(0);
        }
    }
    return bits;
}



/** \brief Takes a vector containing the binary representation
 * of an integer and returns that integer
 *
 * \param v vector<int>
 * \return int
 *
 */
int toInt(vector<int> v)
{
    // Takes a vector containing the binary representation
    // of an integer and returns that integer

    int counter = 0;                // increasing iterator
    int result = 0;
    for (UINT i = 0; i < v.size(); i++)
    {
        result = result + v[i] * twoPow(counter);
        counter++;
    }
    return result;
}

/** \brief Manages a text stream
 */
class fileText
{
    ostringstream text;
public:
    void addLine(string t)
    {
        text << t << "\n";
    }
    void addText(string t){
        text << t;
    }
    void clearText()
    {
        text.clear();
    }
    string getText()
    {
        return text.str();
    }
};


enum crossResult{crosses, doesnotcross, cont};

typedef struct blData{
  float sk;  /**< Reference value for spring parameter */
  float dt;  /**< Reference value for cycle time */
  float mindt;  /**< Cycle time cannot be lower than this value */
  float maxdt;  /**< Cycle time cannot be higher than this value */
  float baseBV; /**< Reference friction coefficient */
  float minratio;
  float stepdt;
  bool doCheckTopol;
  bool fixCircles; /**< If true, circles do not move */
  bool signalEnd;
  bool smoothSVG;
  float surfRatio;
  float minSurfRatio;  /**< Possible condition for ending the simulation if the relationship
                        between the total area of the circles and the area of the figure
                        is lower than this value */
  float maxSurfRatio;  /**< SurfRatio cannot be higher than this value */
  float minDx;
  float minDy;
  float startdt;
  float marginScale;
  float margin;
  float totalCircleV;
  float totalLineV;
  bool increasedDT;
  int contacts;
  UINT ncycles;
  UINT maxRunningTime;
  UINT ncyclesInterrupted;
  float maxf;
  float maxv;
  bool softcontact; /**< When in contact, speed is limited */
  float maxvcontact; /**< When in contact, maxv is maxv / maxvcontact */
  string cycleInfo; /**< Debuggin info from last cycle */
  UINT cyclesForStability; /**< After this number of cycles without improvement, finish sim */
  float lineAir; /**< Added to radii so that there is room for lines between circles */
  UINT contactFunction;
  int checkFor; // If any of the previous or following 10 point is sticking to
                           // the surface, the current point will also stick
  string inputFile;
  string fname;
} blData;




class borderLine;

class circleIterator {
  UINT current;
  UINT first;
  UINT sze;
  UINT mask;
  bool finished;
  vector<point> circles;

  void setval(UINT v){
    UINT sanity = v;
    if (v < sze && circles[v].radius > 0){
      current = v;
    }
    else{
      current = v + 1;
      if (v >= sze){
        v = 0;
      }
      while (circles[v].radius == 0 && !finished){
        v++;
        if (v >= sze){
          v = 0;
        }
        if (v == sanity){
          finished = true;
        }
        current = v;
      }
    }
  }

public:
  circleIterator(){}
  circleIterator(vector<point> circs, UINT npoints, UINT starting = 0) {
    circles = circs;
    sze = npoints;
    setval(starting);
    first = current;
    finished = false;
  }
  /*~groupIterator(){
    tolog(toString(__LINE__) + "\n" + "Called destructor\n");
  }*/
  UINT val() {
    setval(current);
    return current;
  }

  void reset(UINT from = 0){
    setval(from);
    finished = false;
  }

  bool isFinished(){
    return finished;
  }

  UINT nxt() {
    if (finished){
      cout << "Nxt past finished in circleIterator\n";
      tolog("Nxt past finished in circleIterator\n");
      exit(1);
    }
    current++;
    setval(current);
    if (current == first){
      current = 0;
      finished = true;
      return current;
    }
    return current;
  }
};


class groupIterator{
  circleIterator ci;
  vector<point> circles;
  UINT mask;
public:
  groupIterator(vector<point> circs, UINT group, UINT nBits, UINT starting = 0) {
    mask = 1 << group;
    circles = circs;
    ci = circleIterator(circs, circs.size(), starting);
  }
  UINT val(){
    UINT r = ci.val();
    if (r < circles.size()){
      while ((circles[r].n & mask) == 0 && !ci.isFinished()){
        ci.nxt();
        r = ci.val();
      }
    }
    else{
      r = 0;
    }
    return r;
  }
  bool isFinished(){
    return ci.isFinished();
  }
  UINT nxt(){
    if (!ci.isFinished()){
      ci.nxt();
    }
    UINT r = val();
    return r;
  }
};

/** \brief Computes the square of the cartesian distance between
 * points (@p0) and (@p1). If the distance itself is not
 * important, this spares a sqrt calculation
 *
 * \param p0 point
 * \param p1 point
 * \return float
 *
 */
float sqDistance(point p0, point p1)
{
    float result = 0;
    float rx = p1.x - p0.x;
    float ry = p1.y - p0.y;
    rx *= rx;
    ry *= ry;
    result = rx + ry;
    return result;
}





class tangent {
  float slope;
  UINT quadrant;
  bool err;

public:
  tangent(float dx, float dy) {
    slope = 0;
    err = false;
    if (dx == 0 && dy == 0){
      //tolog("Incorrect tangent!\n");
      err = true;
    }
    if (dx < 0 && dy <= 0) {
      quadrant = 1;
      slope = dy / dx;
    } else if (dx >= 0 && dy < 0) {
      quadrant = 2;
      slope = -dx / dy;
    } else if (dx > 0 && dy >= 0) {
      quadrant = 3;
      slope = dy / dx;
    } else {
      quadrant = 4;
      slope = -dx / dy;
    }
  }
  tangent(){}
  tangent(point p1, point p2){
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    slope = 0;
    err = false;
    if (dx < 0 && dy <= 0) {
      quadrant = 1;
      slope = dy / dx;
    } else if (dx >= 0 && dy < 0) {
      quadrant = 2;
      slope = -dx / dy;
    } else if (dx > 0 && dy >= 0) {
      quadrant = 3;
      slope = dy / dx;
    } else {
      quadrant = 4;
      slope = -dx / dy;
    }
    if (slope != slope){
      tolog(_L_ + "Incorrect tangent: \n" + p1.croack() + p2.croack());
      err = true;
    }
  }
  bool error(){
    return err;
  }
  UINT getQuadrant(){
    return quadrant;
  }
  float getSlope(){
    return slope;
  }
  point transformPoint(point p, float r){
    point result;
    float sq = slope * slope;
    float dy = r * sqrt(1/(1+sq));
    float dx = slope * dy;
    if (quadrant == 1){
      result.x = p.x - dy;
      result.y = p.y - dx;
    }
    else if (quadrant == 2){
      result.x = p.x + dx;
      result.y = p.y - dy;
    }
    else if (quadrant == 3){
      result.x = p.x + dy;
      result.y = p.y + dx;
    }
    else if (quadrant == 4){
      result.x = p.x - dx;
      result.y = p.y + dy;
    }
    return result;
  }
  //void operator=(tangent t) {
  //  quadrant = t.quadrant;
  //  slope = t.slope;
  //}
  void rotate(tangent t) {
    if (t.quadrant == quadrant && t.slope == slope){
      slope = 0;
      quadrant = 1;
      return;
    }
    float tslope = t.slope;
    float s = slope;
    if (t.slope > 0) {
      tslope = 1 / t.slope;
    } else {
      quadrant++;
    }
    quadrant += 4 - t.quadrant;
    if ((s * tslope) == 1) {
      slope = 0;
      quadrant++;
    } else {
      slope = (s + tslope) / (1 - s * tslope);
      if (slope < 0) {
        quadrant++;
        slope = -1 / slope;
      }
    }
    quadrant = (quadrant - 1) % 4 + 1;
  }

  tangent bisect(tangent t){
    tangent t1 = *this;
    tangent t2 = t;
    tangent d = t1 - t2;
    tangent h(-1, 1);
    tangent half(-1, 0);
    UINT qdiff = d.quadrant - 1;
    for (UINT i = 0; i < qdiff; i++){
      half.rotate(h);
    }
    float s = d.slope;
    float s1 = 0;
    if (s > 0){
      s1 = (sqrt(1 + s*s)  - 1) / s;
    }
    tangent rest = tangent(-1, 0);
    rest.slope = s1;
    tangent r = half + rest;
    tangent result = r + t1;
    return result;
  }

  tangent leftIntermediate(tangent t){
    tangent t1 = *this;
    tangent t2 = t;
    tangent rv(1, 0);
    tangent tt = t2 + rv;
    tangent b = t1.bisect(tt);
    return b;
  }

  tangent operator-(tangent t){
    tangent t1 = *this;
    tangent t2 = t;
    t2.rotate(t1);
    tangent result = t2;
    return result;
  }

  tangent operator+(tangent t){
    tangent result(1, 1);
    result.quadrant = quadrant;
    result.quadrant += t.quadrant - 1;
    float s = slope;
    float tslope = t.slope;
    if ((s * tslope) == 1) {
      result.slope = 0;
      result.quadrant++;
    }
    else{
      result.slope = (s + tslope) / (1 - s * tslope);
    }
    if (result.slope < 0) {
      result.quadrant++;
      result.slope = -1 / result.slope;
    }
    result.quadrant = (result.quadrant - 1) % 4 + 1;
    return result;
  }

  bool operator<(tangent t2) {
    if (quadrant > t2.quadrant) {
      return false;
    } else if (quadrant < t2.quadrant) {
      return true;
    } else if (slope < t2.slope) {
      return true;
    } else {
      return false;
    }
  }
  bool operator==(tangent t2){
    if (slope == t2.slope && quadrant == t2.quadrant){
      return true;
    }
    return false;
  }
  bool operator!=(tangent t2){
    return !(*this == t2);
  }
  bool operator<=(tangent t2){
    if (*this < t2 || *this == t2){
      return true;
    }
    return false;
  }
  string croack() {
    ostringstream r;
    r << "Slope: " << slope << "\tQuadrant: " << quadrant << endl;
    return r.str();
  }
};

class ccwangle {

  tangent t;

public:
  ccwangle(point p0, point p1, point p2) {
    t = tangent(p1, p2);
    tangent u(p1, p0);
    t.rotate(u);
  }

  bool operator<(ccwangle c) {
    //tolog(toString(__LINE__) + "\n" + t->croack() + "\n" + c.t->croack() + "\n\n");
    if (t < c.t){
      return true;
    }
    else{
      return false;
    }
  }
  string croack(){
    return t.croack();
  }
};

class timeMaster{
  vector<float> times;
  vector<UINT> reports;
  UINT icdt;
  UINT unstableCounter;
  float imindt;
  float imaxdt;
  float istepdt;
  public:
  timeMaster(){}
  void init(float mindt = 1e-4, float maxdt = 0.02, float stepdt = 0.5){
    if (stepdt >= 1){
      stepdt = 0.9;
    }
    if (mindt > maxdt){
      float interm = mindt;
      mindt = maxdt;
      maxdt = interm;
    }
    imindt = mindt;
    imaxdt = maxdt;
    istepdt = stepdt;
    icdt = 0;
    unstableCounter = 5;
    for (float i = maxdt; i >= mindt; i *= stepdt){
      times.push_back(i);
      reports.push_back(0);
    }
  }
  void reset(){
    init(imindt, imaxdt, istepdt);
  }
  float cdt(){
    return times[icdt];
  }
  void report(){
    reports[icdt] = reports[icdt] + 1;
    if (icdt < reports.size()){
      icdt++;
    }
  }
  void poke(){
    //toString(_L_ + "Poke: i - " + toString(icdt) + ", Reports: " + toString(reports[icdt - 1]) + "\n");
    if ((icdt > 0) &&(reports[icdt - 1] < unstableCounter)){
      icdt--;
    }
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

    scale(point p1, point p2){
      x = min(p1.x, p2.x);
      X = max(p1.x, p2.x);
      y = min(p1.y, p2.y);
      Y = max(p1.y, p2.y);
    }

    point place(scale s, point m){
      point r;
      scale tempScale;
      tempScale.initScale();
      float offset;
      float Scale;
      if (s.ratio() <= ratio())
      {
          offset = s.xSpan() - s.ySpan() / ratio();
          offset /= 2;
          tempScale.setMinX(offset + s.minX());
          tempScale.setMaxX(s.maxX() - offset);
          tempScale.setMinY(s.minY());
          tempScale.setMaxY(s.maxY());
      }
      else
      {
          offset = s.ySpan() - ratio() * s.xSpan();
          offset /= 2;
          tempScale.setMinX(s.minX());
          tempScale.setMaxX(s.maxX());
          tempScale.setMinY(offset + s.minY());
          tempScale.setMaxY(s.maxY() - offset);
      }
      r.x = m.x - minX();
      r.y = m.y - minY();
      r.x /= xSpan();
      r.y /= ySpan();
      r.x *= tempScale.xSpan();
      r.y *= tempScale.ySpan();
      r.x += tempScale.minX();
      r.y += tempScale.minY();
      Scale = tempScale.xSpan() / xSpan();
      r.radius = Scale * m.radius;
      return r;
    }

    void addToScale(point p){
      if (clear){
        x = p.x - p.radius;
        X = p.x + p.radius;
        y = p.y - p.radius;
        Y = p.y + p.radius;
        clear = false;
      }
      else{
        float possib[4] = {
          p.x - p.radius,
          p.x + p.radius,
          p.y - p.radius,
          p.y + p.radius
        };
        for (UINT i = 0; i < 2; i++){
          if (possib[i] < x){
            x = possib[i];
          }
          else if (possib[i] > X){
            X = possib[i];
          }
        }
        for (UINT i = 2; i < 4; i++){
          if (possib[i] < y){
            y = possib[i];
          }
          else if (possib[i] > Y){
            Y = possib[i];
          }
        }
      }
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
    string croack(){
      string result;
      result += "MinX: " + toString(minX()) + "\n";
      result += "MaxX: " + toString(maxX()) + "\n";
      result += "MinY: " + toString(minY()) + "\n";
      result += "MaxY: " + toString(maxY()) + "\n";
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
void _D_(vector<T> v, string sep=""){
  string r;
  for (UINT i = 0; i < v.size() - 1; i++){
    r += toString((UINT) v[i]) + sep;
  }
  r += v[v.size()-1];
  //tolog(toString(__LINE__) + "\n" + r);
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

typedef struct{
  point outsider;
  float dsena;
  UINT vertex;
  UINT nextVertex;
} outsiderInfo;

void oilog(outsiderInfo oi){
  tolog(_L_ + "Vertex: " + toString(oi.vertex) + "\nNextVertex: " + toString(oi.nextVertex) + "\n");
}

class optimizationStep{
  UINT counter;
  UINT candidate;
  bool ended;
  bool untied;
  float bestComp;
public:
  optimizationStep(float comp){
    bestComp = comp;
    startCycle();
    ended = true;
    untied = false;
  }
  UINT getCounter(){
    return counter;
  }
  void next(){
    counter++;
    ended = false;
  }
  void setOptimus(float comp){
    bestComp = comp;
  }
  void setCandidate(UINT cand){
    candidate = cand;
  }
  void startCycle(){
    counter = 0;
    candidate = 0;
    ended = false;
  }
  void endCycle(){
    ended = true;
  }
  bool hasEnded(){
    return ended;
  }
  void setUntie(){
    untied = true;
  }
  bool hasUntied(){
    bool result = untied;
    untied = false;
    return result;
  }
  UINT getCandidate(){
    return candidate;
  }
  void setBestCompactness(float comp){
    bestComp = comp;
  }
  float getBestCompactness(){
    return bestComp;
  }

};


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

        int counter;
        UINT i, j;
        bool ismatched = false;
        if (v.size() > 0){
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
        }
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
                result.push_back(tempV);
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
        column.push_back(v);
        while (position != -1)
        {
            result[position] = 1;
            column.push_back(result);
            position = firstUnmatched0(result);
        }
        row.push_back(column);
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

bool outsorter(outsiderInfo a, outsiderInfo b){
  if (a.vertex != b.vertex){
    return (a.vertex < b.vertex);
  }
  if (a.dsena != b.dsena){
    return (a.dsena < b.dsena);
  }
  return false;
}

class decider{
  bool finish;
  bool canFinish;
  bool first;
  UINT cyclesWithoutImprovement;
  UINT finishAfter;
  float bestCompactness;
  UINT nAvg;
  UINT counter;
  float totalCompactness;
  float lastCompactness;
  bool keep;
public:
  void setConstants(UINT numberForAverage, UINT numberForStability){
    finish = false;
    canFinish = false;
    cyclesWithoutImprovement = 0;
    nAvg = numberForAverage;
    finishAfter = numberForStability;
    counter = 0;
    totalCompactness = 0;
    first = true;
    keep = false;
  }
  void init(){
    finish = false;
    //canFinish = false;
    first = true;
    cyclesWithoutImprovement = 0;
    counter = 0;
    totalCompactness = 0;
    lastCompactness = 0;
  }
  bool keepState(){
    return keep;
  }
  void add(float comp){
    keep = false;
    if (first){
      first = false;
      counter++;
      bestCompactness = comp;
      lastCompactness = comp;
    }
    if (canFinish){
      if (comp < bestCompactness){
        bestCompactness = comp;
        keep = true;
        cyclesWithoutImprovement = 0;
      }
      else{
        cyclesWithoutImprovement++;
        if (cyclesWithoutImprovement > finishAfter){
          finish = true;
        }
      }
    }
    else{
      totalCompactness += comp;
      counter++;
      if (counter > nAvg){
        float newAvg = totalCompactness / ((float)counter);
        if (newAvg > lastCompactness){
          canFinish = true;
        }
        counter = 1;
        totalCompactness = 0;
        lastCompactness = newAvg;
      }
    }
  }
  UINT viewCounter(){
    return counter;
  }
  float viewLastComp(){
    return lastCompactness;
  }
  UINT viewCanFinish(){
    return (UINT) canFinish;
  }
  UINT viewCyclesWithoutImprovement(){
    return cyclesWithoutImprovement;
  }

  bool finished(){
    bool result = finish;
    return result;
  }

};

typedef struct blst{
  vector<point> bestCircles;
  vector<vector<point> > bestBl; /**< Best result */
  vector<vector<point> > bl_secure; /**< For changes in the number of points */
  vector<vector<point> > bl_old10;
  vector<point> circles_secure;
  vector<point> circles_old10;
  float simTime;
  bool hasBeenSet;
} blState;

class borderLine
{
    friend class glGraphics;
    friend class groupIterator;

    binMap* bm;
    scene tosolve;
    blState savedState;
    float simulationTime;
    UINT internalCounter;

    vector<float> pairDistances;
    vector<string> groups;
    vector<point> p;
    vector<vector<point> > bl; /**< Vector of lines. Each line is a vector of points */
    vector<point> circles;
    vector<point> scircles; /**< Circles sorted according to @n */
    vector<point> debug;
    vector<UINT> relationships;
    UINT ncycles_secure;
    UINT ncyles_old10;
    UINT ngroups;
    UINT startPerim;
    float minCircRadius; /**< Minimum circ radius, to calc nPointsMin */
    float maxRadius;   /**< Radius of the largest circlese */
    UINT nPointsMin;  /**< Minimum number of points per line */
    float avgStartDist; /**< Average distance between points at start */
    float potential; /**< Potential energy, some parameter to minimize */
    decider evaluation;
    lCounter blCounter;
    lCounter deciderCounter;
    lCounter refreshScreen;
    lCounter keepDistCounter;
    vector<float> circRadii;
    vector<float> w;
    vector<string> labels;
    vector<float> origw;
    vector<rgb> colors;
    vector<string> svgcolors;
    vector<point> warn;
    scale internalScale;
    scale svgScale;
    int totalExpectedSurface;
    vector<string> dataDisplay;
    timeMaster udt;
    blData blSettings;
    float minRat;
    float minCircScreenRadius;
    bool showThis;

    void initBlData(blData* b){
      b->sk = 1e3f;
      b->dt = 0.025f;
      b->mindt = b->dt / 100;
      b->maxdt = b->dt;
      b->baseBV = 5.0f;

      b->minratio = 0.005f;
      b->stepdt = 0.6f;
      b->doCheckTopol = true;
      b->fixCircles = false;
      b->signalEnd = false;
      b->smoothSVG = false;
      b->surfRatio = 0;
      b->minSurfRatio = 0;
      b->maxSurfRatio = 10;
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
      b->cyclesForStability = 100;
      b->contactFunction = 0; // contact()
      b->maxRunningTime = 200; // 300 seconds to finish the first part
    }

    UINT leftmostCircle(UINT group){
      groupIterator git(circles, group, bl.size(), 1);
      UINT result = git.val();
      float lx = circles[result].x;
      while(!git.isFinished()){
        UINT n = git.val();
        if (circles[n].radius > 0 && circles[n].x < lx){
          result = n;
          lx = circles[n].x;
        }
        git.nxt();
      }
      return result;
    }

    void resetTimer(){
      udt.reset();
    }

    void attention(float x, float y)
    {
        point temp;
        temp.x = x;
        temp.y = y;
        temp.radius = 1;
        warn.push_back(temp);
    }

    string float2string(float f){
        string result = vformat("%g", f);
        return result;
    }

    string UINT2string(UINT f){
        string result = vformat("%u", f);
        return result;
    }

    string bool2string(bool f){
        string result = f ? "1" : "0";
        return result;
    }

    void displayFloat(string label, float d){
        string dsp = vformat("%s: %g", label.c_str(), d);
        dataDisplay.push_back(dsp);
    }

    void displayUINT(string label, UINT d){
        string dsp = vformat("%s: %u", label.c_str(), d);
        dataDisplay.push_back(dsp);
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
        savedState.bl_old10 = bl;
        ncyles_old10 = blSettings.ncycles;
        savedState.circles_old10 = circles;
    }

    point place(scale s, point m)
    {
        point r;
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
        minRat = 0.02 * minCoord;

        minCircScreenRadius = correctedMinCircRadius();

        for (i = 0; i < circles.size(); i++){
            if (circles[i].radius > 0){
              float trad = circles[i].radius;
              float tr = circRadii[i];
              point t; t.radius = tr; t.x = 0; t.y = 0;
              point test = place(internalScale, t);
              float rat = test.radius / minRat;
              if (rat < 1 && rat > 0){
                trad = tr / rat;
              }
              else if (trad != tr){
                trad = tr;
              }
              circles[i].radius = trad;
              //if (trad > maxRadius){
              //  maxRadius = trad;
              //}
            }
        }

    }

    void setCircles(binMap b, vector<float> o, vector<string> tlabels)
    {
        UINT i, j;
        int n;
        int height;
        int offset;
        float maxw = 0;
        point cpoint;
        height = b.row[0].size();
        scircles.assign((1 << b.ngroups),cpoint);
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
                circRadii.push_back(2 * sqrt(origw[n] / maxw));
                cpoint.orig = o[n];
                circles.push_back(cpoint);
                labels.push_back(tlabels[n]);
                cpoint.fx = 0;
                cpoint.fy = 0;
                cpoint.vx = 0;
                cpoint.vy = 0;
                cpoint.inContact = false;
            }
        }
        for (j = 1; j < circRadii.size(); j++){
          if (circRadii[j] > 0 && circRadii[j] < minCircRadius) minCircRadius = circRadii[j];
          if (circRadii[j] > 0 && circRadii[j] > maxRadius) maxRadius = circRadii[j];
        }
    }

    /** \brief Gets the coordinates of circles from setCircles() and sets them
     *         on a grid pseudorandomly
     *
     * \return void
     *
     */
    void randomizeCircles(){
      UINT gstep = 1;
      UINT gridSize = twoPow(ngroups >> 1);
      if ((ngroups & 1) > 0){ // Odd number of groups
          gridSize = gridSize << 1;
          gstep = 2;
      }

      //cout << "gridSize: " << gridSize << "\n";
      float xstep = internalScale.xSpan() / ((gridSize + 1));
      float ystep = xstep; //internalScale.ySpan() / (gridSize + 1);
      //cout << "xStep: " << internalScale.xSpan() << "\n";
      //cout << "yStep: " << ystep << "\n";
      vector<UINT> order;
      for (UINT i = 0; i < circles.size(); i++){
        order.push_back(i);
      }
      std::random_shuffle(order.begin(), order.end());
      UINT sy = 0; UINT sx = 0;
      for (UINT i = 0; i < circles.size(); i++){
        float cx = (sx + 0.5) * xstep + internalScale.minX();
        float cy = (sy + 0.5) * ystep + internalScale.minY();
        sx += gstep;
        if (sx > gridSize){
          sy++;
          sx = 0;
        }
        circles[order[i]].x = cx;
        circles[order[i]].y = cy;
      }

    }


    void setPoints(binMap b, UINT ngroup)
    {
        int i, counter;
        int cstart;         //placement of first and last point
        //depending on ngroup
        point cpoint;       //point storage
        point fpoint;       //first point
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
        internalScale.addToScale(fpoint);
        p.push_back(fpoint);
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
            internalScale.addToScale(cpoint);
            p.push_back(cpoint);
            cpoint.x = 6 * i + 2;
            internalScale.addToScale(cpoint);
            p.push_back(cpoint);
            // add circles
        }
        //last point
        cpoint.x = 6 * (width + 1 + cstart);
        cpoint.y = 6 * (height + 1 + cstart);
        cpoint.mass = POINT_MASS;
        internalScale.addToScale(cpoint);
        p.push_back(cpoint);
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
        for (i = 0; i < savedState.bl_secure.size(); i++)
        {
            for(j = 0; j < savedState.bl_secure[i].size(); j++)
            {
                savedState.bl_secure[i][j].reset();
            }
        }
        for (i = 0; i < savedState.bl_old10.size(); i++)
        {
            for (j = 0; j < savedState.bl_old10[i].size(); j++)
            {
                savedState.bl_old10[i][j].reset();
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
        if (d > maxrad) fatt = blSettings.sk * kattr * (d - radius);
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

    point selforce(point &p0, point &p1, float kattr = 5e-3f, float radiusFactor = 0)
    {
      if (kattr == 0){
        kattr = 5e-3f;
      }
        if (p1.radius == 0 || p0.radius == 0){
          return p0;
        }
        float repulsion = blSettings.sk * kattr * 5;
        point result;
        float factor = (float) getRelationships(p0.n, p1.n) + 1;
        float radius = radiusFactor * (p0.radius + p1.radius);
        //radius *= 1.2;
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        float d = distance(p0.x, p0.y, p1.x, p1.y);
        float fatt = 0;
        if (d > radius) fatt = (factor*factor) * blSettings.sk * kattr * (d-radius) - repulsion;
        //if (d < radius) fatt = -blSettings.sk * kattr * (d - radius);
        result.fx = dx * fatt/d;
        result.fy = dy * fatt/d;
        potential += fatt;
        p0.fx += result.fx;
        p1.fx -= result.fx;
        p0.fy += result.fy;
        p1.fy -= result.fy;
        return result;
    }

    point selforce2(point &p0, point &p1, float kattr = 5e-4f)
    {
      if (kattr == 0){
        kattr = 5e-4f;
      }
        if (p1.radius == 0 || p0.radius == 0){
          return p0;
        }
        float repulsion = -kattr;
        point result;
        //float factor = (float) getRelationships(p0.n, p1.n);
        float radius = p0.radius + p1.radius + 4 * blSettings.margin;
        //radius *= 1.2;
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        float d = distance(p0.x, p0.y, p1.x, p1.y);
        if (d == 0){
          d = 0.01;
        }
        float fatt = 0;
        if (d > radius) fatt = blSettings.sk * kattr * (d-radius) - repulsion;
        //if (d < radius) fatt = -blSettings.sk * kattr * (d - radius);
        result.fx = dx * fatt/d;
        result.fy = dy * fatt/d;
        potential += fatt;
        p0.fx += result.fx;
        p1.fx -= result.fx;
        p0.fy += result.fy;
        p1.fy -= result.fy;
        return result;
    }

    void listOutsiders(){
      for (UINT i = 0; i < bl.size(); i++){
        for (UINT j = 0; j < circles.size(); j++){
          vector<int> belong = toBin(circles[j].n, bl.size());
          if (circles[j].radius > 0 && belong[i] == 0){
            bool incorrect = circleTopol(circles[j], belong, i);
            if (incorrect){
              tolog(_L_ +  ": " + "Incorrect topology of circle " + toString(circles[j].n) +
                    " with line " + toString(i) + ", " + circles[j].croack());
            }
          }
        }
      }
    }


    void embellishTopology(bool logit = false){
      tangent lft(0, -1);
      tangent rgh(0, 1);
      for (UINT k = 0; k < circles.size(); k++){
        circles[k].flags = unsetFlag(circles[k].flags, USED);
      }
      for (UINT i = 0; i < bl.size(); i++){
        bool again = true;
        while (again){
          again = false;
          vector<point> newbl;
          for (UINT j = 0; j < bl[i].size(); j++){
            if (!again){
              point nxt = bl[i][nextPoint(i, j)];
              point current = bl[i][j];
              newbl.push_back(current);
              float dsq3 = sqDistance(current, nxt);
              if (dsq3 > 0){
                float sq3 = sqrt(dsq3);
                UINT fcirc = 0;
                for (UINT k = fcirc; k < circles.size(); k++){
                  point c = circles[k];
                  if (c.radius > 0){
                    float lrad = maxRad();
                    if (!again && ((c.flags & USED) == 0)){
                      bool inside = (c.n & twoPow(i)) > 0 ? true : false;
                      float dsq1 = sqDistance(c, current);
                      float dsq2 = sqDistance(c, nxt);
                      float rsq = c.radius * c.radius;
                      if (dsq3 > dsq1 && dsq3 > dsq2 && dsq1 > rsq && dsq2 > rsq){
                        float h = sqrt(-(dsq1 * dsq1 + dsq2 * dsq2 + dsq3 * dsq3) +
                                       2 * dsq1 * dsq2 + 2 * dsq1 * dsq3 + 2 * dsq2 * dsq3) / (2 * sq3);
                        //tolog(toString(__LINE__) + "\n" + toString(h) + "\t" + toString(dsq1) + "\t" + toString(dsq2) + "\t" + toString(dsq3) + "\t" + toString(sq3) + "\n");
                        if (h > 0 && h < c.radius){
                          again = true;
                          fcirc = k + 1;
                          tangent tp(nxt.x - current.x, nxt.y - current.y);
                          float t = sqrt(lrad * lrad - h * h);
                          float d1 = sqrt(dsq1 - h * h);
                          float x1 = d1 - t;
                          point pst1 = tp.transformPoint(current, x1);
                          pst1.flags = setFlag(pst1.flags, DO_NOT_EMBELLISH);
                          pst1.flags = setFlag(pst1.flags, DELME);
                          pst1.n = k;
                          point pst2 = tp.transformPoint(current, d1 + t) ;
                          pst2.flags = setFlag(pst2.flags, DO_NOT_EMBELLISH);
                          pst2.flags = setFlag(pst2.flags, DELME);
                          pst2.n = k;
                          newbl.push_back(pst1);
                          tangent tr = tp + rgh;
                          //tolog(toString(__LINE__) + "\n" + "Outside: \n" + tp.croack() + tr.croack());
                          if (inside || (!inside && ((c.flags & USED) > 0))){
                            tr = tp + lft;
                            //tolog(toString(__LINE__) + "\n" + "Inside: \n" + tp.croack() + tr.croack());
                          }
                          c.flags = setFlag(c.flags, USED);
                          point newp = tr.transformPoint(c, lrad);
                          newp.flags = setFlag(newp.flags, DO_NOT_EMBELLISH);
                          newp.flags = setFlag(newp.flags, DELME);
                          newp.n = k;
                          newbl.push_back(newp);
                          newbl.push_back(pst2);
                          // Close the bl for the next cycle
                          for (UINT l = j+1; l < bl[i].size(); l++){
                            newbl.push_back(bl[i][l]);
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          bl[i].clear();
          bl[i] = newbl;
          for (UINT k = 0; k < circles.size(); k++){
            circles[k].flags = unsetFlag(circles[k].flags, USED);
          }
        }
      }
      /* */
    }


    /** \brief Fix topology by excluding circles from improper borderlines
     *
     * \param false bool logit= Whether the function should log operations.
     *        Ignored if DEBUG is undef.
     * \return void
     *
     */
    void fixTopology(bool logit = false){
      addLines();
      polishLines();
      for (UINT i = 0; i < circles.size(); i++){
        circles[i].flags = unsetFlag(circles[i].flags, USED);
      }
      for (UINT k = 0; k < bl.size(); k++){
        bool goon = true;
        while (goon){
          goon = false;
          for (UINT j = 0; j < circles.size(); j++){
            if (circles[j].radius > 0 && ((circles[j].flags & USED) == 0) && !goon){
              circles[j].flags = setFlag(circles[j].flags, USED);
              point p3 = circles[j];
              UINT p3n = p3.n;
              vector<int> belong = toBin(p3n, bl.size());
              bool incorrect = circleTopol(circles[j], belong, k);
              if (incorrect){
                if (logit){
                  tolog(_L_ +  "Fixing circle " + toString(circles[j].n) +
                        " with line " + toString(k) + "\n");
                }
                outsiderInfo oi;
                float minV = -1;
                for (UINT i = 0; i < bl[k].size(); i++){
                  UINT tvertx = i;
                  point p1 = bl[k][tvertx];
                  UINT tvertnx = nextPoint(k, tvertx);
                  point p2 = bl[k][tvertnx];
                  if (p1 != p3 && p2 != p3 && p1 != p2 &&
                      !(p1.flags & DO_NOT_EMBELLISH) &&
                      !(p2.flags & DO_NOT_EMBELLISH)){
                    tangent t1(p1, p2);
                    tangent t2(p1, p3);
                    tangent t3(p2, p3);
                    //tolog(toString(__LINE__) + "\n" + t2.croack() + t3.croack());
                    t2.rotate(t1);
                    t3.rotate(t1);
                    //tolog(toString(__LINE__) + "\n" + t2.croack() + t3.croack() + "Line " + toString(k) + ", circle " + toString(circles[j].n) + "\n");
                    if ((t2.getQuadrant() == 4 && t3.getQuadrant() == 3)){
                      //tolog(toString(__LINE__) + "\n" + "Line " + toString(k) + ", circle " + toString(j) + " " + toString(circles[j].n) + "\n");
                      float d1sq = (p3.x - p1.x) * (p3.x - p1.x) +
                                   (p3.y - p1.y) * (p3.y - p1.y);
                      float d2sq = (p2.x - p3.x) * (p2.x - p3.x) +
                                   (p2.y - p3.y) * (p2.y - p3.y);
                      float dsq = (p2.x - p1.x) * (p2.x - p1.x) +
                                  (p2.y - p1.y) * (p2.y - p1.y);
                      if (true){ //dsq > 0){
                        float d = sqrt(dsq);
                        float dsena = (dsq + d1sq - d2sq) / (2*d);
                        float ddsq = d1sq - dsena * dsena;
                        if ((minV < 0 || (ddsq < minV))){
                          minV = ddsq;
                          //tolog(toString(__LINE__) + "\tminV " + toString(minV) + ", ddsq " + toString(ddsq) + "\n");
                          oi.vertex = tvertx;
                          oi.nextVertex = tvertnx;
                          oi.dsena = dsena;
                          oi.outsider = p3;
                          goon = true;
                        }
                      }
                      else{
                        tolog(_L_ + "DSQ failure: " + toString(dsq));
                      }
                    }
                  }
                }
                if (goon){
                  //oilog(oi);
                  vector<point> newpoints;
                  for (UINT i = 0; i <= oi.vertex; i++){
                    newpoints.push_back(bl[k][i]);
                  }
                  UINT prevVertex = oi.vertex;
                  UINT currentVertex = oi.nextVertex;
                  float dsena = oi.dsena;
                  point pt = oi.outsider;
                  //if (dsena > pt.radius){
                    float lrad = maxRad() * AIR;
                    tangent mn(0, 1);
                    tangent rv(1, 0);
                    point v = bl[k][prevVertex];
                    point vn = bl[k][currentVertex];
                    tangent t(v, vn);
                    point q = t.transformPoint(v, dsena);
                    tangent toc1(q, pt);
                    float d = distance(q.x, q.y, pt.x, pt.y);
                    point c1 = toc1.transformPoint(q, d - lrad);
                    tangent toc2 = toc1 + mn;
                    point c2 = toc2.transformPoint(pt, lrad);
                    point c3 = toc1.transformPoint(pt, lrad);
                    tangent toc4 = toc2 + rv;
                    point c4 = toc4.transformPoint(pt, lrad);
                    q.n = j; c1.n = j; c2.n = j;
                    c3.n = j; c4.n = j;
                    newpoints.push_back(q);
                    newpoints.push_back(c1);
                    newpoints.push_back(c2);
                    newpoints.push_back(c3);
                    newpoints.push_back(c4);
                    newpoints.push_back(c1);
                    newpoints.push_back(q);
                    //tolog(_L_ + t.croack() + toc1.croack() + toc2.croack() + toc4.croack());
                  //}
                  if (oi.nextVertex > 0){
                    for (UINT i = oi.nextVertex; i < bl[k].size(); i++){
                      newpoints.push_back(bl[k][i]);
                    }
                  }
                  bl[k].clear();
                  bl[k] = newpoints;
                  //tolog(_L_ +  toString(circleTopol(p3, belong, k)) + "\n");
                  //writeSVG("delme.svg");
                  //exit(0);
                  //tolog(toString(circleTopol(p3, belong, k)) + "\n");
                }
                else{
                  //tolog(_L_ + "Somehow, this one did not make it.\n");
                  //tolog(_L_ + "Line " + toString(k) + ", segment " + toString(oi.vertex) + "\n");
                  //tolog(_L_ + oi.outsider.croack());
                  //writeSVG("delme.svg");
                  //exit(1);
                }
                incorrect = circleTopol(circles[j], belong, k); //Was it fixed?
                if (incorrect){
                  //writeSVG("error.svg");
                  //tolog(_L_ + "Could not fix circle " + toString(circles[j].n) + " with line " + toString(k) + ".\n");
                  //exit(1);
                }
              }
            }
          }
        }
        for (UINT i = 0; i < circles.size(); i++){
          circles[i].flags = unsetFlag(circles[i].flags, USED);
        }
      }
      embellishTopology(logit);
    }


    /** \brief Fix topology by excluding circles from improper borderlines
     *
     * \param false bool logit= Whether the function should log operations.
     *        Ignored if DEBUG is undef.
     * \return void
     *
     */
    void fTopology(bool logit = false){
      addLines();
      //writeSVG("addlines.svg");
      polishLines();
      writeSVG("polishlines.svg");
      //embellishTopology();
      writeSVG("embellish.svg");
      for (UINT k = 0; k < bl.size(); k++){
        bool goon = true;
        while (goon){
          goon = false;
          for (UINT j = 0; j < circles.size(); j++){
            if (circles[j].radius > 0 && ((circles[j].flags & USED) == 0)){
              circles[j].flags = setFlag(circles[j].flags, USED);
              point p3 = circles[j];
              vector<int> belong = toBin(p3.n, bl.size());
              bool incorrect = circleTopol(circles[j], belong, k);
              bool safe = false;
              if (incorrect){
                if (logit){
                  tolog(_L_ +  "Fixing circle " + toString(circles[j].n) +
                        " with line " + toString(k) + "\n");
                }
                outsiderInfo oi;
                float minV = -1;
                for (UINT i = 0; i < bl[k].size(); i++){
                  UINT tvertx = i;
                  point p1 = bl[k][tvertx];
                  UINT tvertnx = 0;
                  if (i < (bl[k].size() - 1)){
                    tvertnx = i + 1;
                  }
                  point p2 = bl[k][tvertnx];
                  if (p1 != p3 && p2 != p3 && p1 != p2){
                    tangent t1(p1, p2);
                    tangent t2(p1, p3);
                    tangent t3(p2, p3);
                    //tolog(toString(__LINE__) + "\n" + t2.croack() + t3.croack());
                    t2.rotate(t1);
                    t3.rotate(t1);
                    //tolog(toString(__LINE__) + "\n" + t2.croack() + t3.croack() + "Line " + toString(k) + ", circle " + toString(circles[j].n) + "\n");
                    if (t2.getQuadrant() == 4 && t3.getQuadrant() == 3){
                      //tolog(toString(__LINE__) + "\n" + "Line " + toString(k) + ", circle " + toString(j) + " " + toString(circles[j].n) + "\n");
                      float d1sq = (p3.x - p1.x) * (p3.x - p1.x) +
                                   (p3.y - p1.y) * (p3.y - p1.y);
                      float d2sq = (p2.x - p3.x) * (p2.x - p3.x) +
                                   (p2.y - p3.y) * (p2.y - p3.y);
                      float dsq = (p2.x - p1.x) * (p2.x - p1.x) +
                                  (p2.y - p1.y) * (p2.y - p1.y);
                      if (dsq > 0){
                        float d = sqrt(dsq);
                        float dsena = (dsq + d1sq - d2sq) / (2*d);
                        float ddsq = d1sq - dsena * dsena;
                        tolog(_L_ +  "\nLine: " + toString(k) + ", segment" + toString(i) + ", circle " + toString(circles[j].n) + "\n");
                        tolog(_L_ + "minV " + toString(minV) + ", ddsq " + toString(ddsq) + "\n");
                        if ((minV < 0 || (ddsq < minV)) || !safe){
                          minV = ddsq;
                          //tolog(toString(__LINE__) + "\tminV " + toString(minV) + ", ddsq " + toString(ddsq) + "\n");
                          oi.vertex = tvertx;
                          oi.nextVertex = tvertnx;
                          oi.dsena = dsena;
                          oi.outsider = p3;
                          goon = true;
                          safe = true;
                        }
                      }
                    }
                  }
                }
                if (goon){
                  vector<point> newpoints;
                  for (UINT i = 0; i <= oi.vertex; i++){
                    newpoints.push_back(bl[k][i]);
                  }
                  UINT prevVertex = oi.vertex;
                  UINT currentVertex = oi.nextVertex;
                  float dsena = oi.dsena;
                  point pt = oi.outsider;
                  if (dsena > pt.radius){
                    tangent mn(0, 1);
                    tangent rv(1, 0);
                    point v = bl[k][prevVertex];
                    point vn = bl[k][currentVertex];
                    tangent t(v, vn);
                    point q = t.transformPoint(v, dsena);
                    tangent toc1(q, pt);
                    float d = distance(q.x, q.y, pt.x, pt.y);
                    point c1 = toc1.transformPoint(q, d - pt.radius);
                    tangent toc2 = toc1 + mn;
                    point c2 = toc2.transformPoint(pt, pt.radius);
                    point c3 = toc1.transformPoint(pt, pt.radius);
                    tangent toc4 = toc2 + rv;
                    point c4 = toc4.transformPoint(pt, pt.radius);
                    newpoints.push_back(q);
                    newpoints.push_back(c1);
                    newpoints.push_back(c2);
                    newpoints.push_back(c3);
                    newpoints.push_back(c4);
                    newpoints.push_back(c1);
                    newpoints.push_back(q);
                    //tolog(_L_ + t.croack() + toc1.croack() + toc2.croack() + toc4.croack());
                  }
                  for (UINT i = oi.nextVertex; i < bl[k].size(); i++){
                    newpoints.push_back(bl[k][i]);
                  }
                  bl[k] = newpoints;
                  tolog(_L_ + toString(circleTopol(p3, belong, k)) + "\n");
                  //writeSVG("delme.svg");
                  //exit(0);
                  //tolog(toString(circleTopol(p3, belong, k)) + "\n");
                }
                else{
                  tolog(_L_ + "Somehow, this one did not make it.\n");
                  tolog(_L_ + "Line " + toString(k) + ", segment " + toString(oi.vertex) + "\n");
                  tolog(_L_ + oi.outsider.croack());
                  writeSVG("error.svg");
                  exit(1);
                }
                incorrect = circleTopol(circles[j], belong, k); //Was it fixed?
                if (incorrect){
                  writeSVG("error.svg");
                  tolog(_L_ + "Could not fix circle " + toString(circles[j].n) + " with line " + toString(k) + ".\n");
                  exit(1);
                }
              }
            }
          }
        }
        for (UINT i = 0; i < circles.size(); i++){
          circles[i].flags = unsetFlag(circles[i].flags, USED);
        }
      }
      embellishTopology(logit);
    }

    float compactness(){
      float result = 0;
      for (UINT i = 0; i < circles.size() - 1; i++){
        if (circles[i].radius > 0){
          for (UINT j = i + 1; j < circles.size(); j++){
            if (circles[j].radius > 0){
              UINT n = getRelationships(i, j);
              if (n > 0){
                float sqd = sqDistance(circles[i], circles[j]);
                result += log(sqd) + log(n);
              }
            }
          }
        }
      }
      return result;
    }

    UINT nextLeftmostPoint(UINT ngroup, UINT n1, UINT n2){
      point anchor = circles[n1];
      if (n1 == n2){
        anchor.x -= 1;
      }
      groupIterator git(circles, ngroup, bl.size(), n2);
      git.nxt();
      bool first = true;
      ccwangle best(anchor, circles[n2], anchor);
      UINT result = git.val();
      while (!git.isFinished()){
        UINT n3 = git.val();
        if (n3 != n2){
          ccwangle test(anchor, circles[n2], circles[n3]);
          if (first || best < test){
            first = false;
            best = test;
            result = n3;
          }
        }
        git.nxt();
      }
      return result;
    }

    /** \brief Adds group lines
     *
     * \return void
     *
     */
    void addLines(bool logit = false){
      /*for (UINT i = 0; i < circles.size(); i++){
        scircles[circles[i].n] = circles[i];
      }*/
      for (UINT i = 0; i < bl.size(); i++){
        //cout << "---" << i << "----\n";
        UINT lm = leftmostCircle(i);
        bl[i].clear();
        point toadd = circles[lm];
        toadd.n = lm;
        bl[i].push_back(toadd);
        //cout << "-> " << circles[lm].n << endl;
        UINT an = lm;
        UINT st = lm;
        UINT np = nextLeftmostPoint(i, an, st);
        while (np != lm){
          point toadd = circles[np];
          toadd.n = np;
          bl[i].push_back(toadd);
          //cout << circles[np].n << endl;
          an = st;
          st = np;
          np = nextLeftmostPoint(i, an, st);
        }
      }
    }


    /** \brief Take the lines generated with @addLines and set the points to the
     *         outside of each circle.
     *
     * \return void
     *
     */
    void polishLines(){
      tangent rev(1, 0);
      for (UINT i = 0; i < bl.size(); i++){
        UINT sz = bl[i].size();
        vector<point> newpoints;
        if (sz < 2){
          tangent dwn(0, -1);
          tangent lft(-1, 0);
          tangent up(0, 1);
          point current = bl[i][0];
          UINT n = current.n;
          point p1 = rev.transformPoint(current, current.radius);
          p1.flags = setFlag(p1.flags, DO_NOT_EMBELLISH);
          p1.n = n;
          point p2 = dwn.transformPoint(current, current.radius);
          p2.flags = setFlag(p2.flags, DO_NOT_EMBELLISH);
          p2.n = n;
          point p3 = lft.transformPoint(current, current.radius);
          p3.flags = setFlag(p3.flags, DO_NOT_EMBELLISH);
          p3.n = n;
          point p4 = up.transformPoint(current, current.radius);
          p4.flags = setFlag(p4.flags, DO_NOT_EMBELLISH);
          p4.n = n;
          newpoints.push_back(p1);
          newpoints.push_back(p2);
          newpoints.push_back(p3);
          newpoints.push_back(p4);
        }
        else{
            point prev = bl[i][sz - 1];

            for (UINT j = 0; j < sz; j++){
              point current = bl[i][j];
              UINT n = current.n;
              int nj = nextPoint(i, j);
              point next = bl[i][nj];
              if (current.radius > 0){
                // Point 1
                float r = maxRadius;// current.radius;
                float d = distance(current.x, current.y, prev.x, prev.y);
                if (d > 0){
                  float ndx = (current.x - prev.x) * (1 - r/d);
                  float ndy = (current.y - prev.y) * (1 - r/d);
                  point np;
                  np.x = prev.x + ndx;
                  np.y = prev.y + ndy;
                  np.flags = setFlag(np.flags, DELME);
                  np.n = n;
                  newpoints.push_back(np);
                  tangent t1(current.x - prev.x, current.y - prev.y);
                  tangent t2(next.x - current.x, next.y - current.y);
                  tangent t3 = t1.leftIntermediate(t2);
                  tangent rt3 = t3 + rev;
                  tangent t4 = t1.leftIntermediate(t3);
                  tangent t5 = rt3.leftIntermediate(t2);
                  // Point 2
                  point tt2 = t4.transformPoint(current, r);
                  tt2.flags = setFlag(tt2.flags, DO_NOT_EMBELLISH);
                  tt2.flags = setFlag(tt2.flags, DELME);
                  tt2.n = n;

                  // Point 3
                  point tt3 = t3.transformPoint(current, r);
                  tt3.flags = setFlag(tt3.flags, DO_NOT_EMBELLISH);
                  tt3.flags = setFlag(tt3.flags, DELME);
                  tt3.n = n;

                  // Point 4
                  point tt4 = t5.transformPoint(current, r);
                  tt4.flags = setFlag(tt4.flags, DO_NOT_EMBELLISH);
                  tt4.flags = setFlag(tt4.flags, DELME);
                  tt4.n = n;

                  /*if ((current.flags & RIGHT_SIDE) > 0){
                    newpoints.push_back(tt4);
                    newpoints.push_back(tt3);
                    newpoints.push_back(tt2);
                    tolog(toString(__LINE__) + ": Right-side byte set wrong\n");
                  }
                  else{*/
                    newpoints.push_back(tt2);
                    newpoints.push_back(tt3);
                    newpoints.push_back(tt4);
                  //  }


                }
                // Point 5
                d = distance(next.x, next.y, current.x, current.y);
                if (d > 0){
                  float ndx = (next.x - current.x) * (r/d);
                  float ndy = (next.y - current.y) * (r/d);
                  point np;
                  np.x = current.x + ndx;
                  np.y = current.y + ndy;
                  np.flags = setFlag(np.flags, DELME);
                  np.n = n;
                  newpoints.push_back(np);
                }
              }
            else{
              newpoints.push_back(current);
            }
            prev = current;
          }
        }
        bl[i] = newpoints;
      }
      //setCheckTopol(true);
      //writeSVG(); exit(0);
    }

    void swapCoords(UINT i, UINT j){
      point interm = circles[i];
      circles[i].x = circles[j].x;
      circles[i].y = circles[j].y;
      circles[j].x = interm.x;
      circles[j].y = interm.y;
    }


    vector<point> getSetBoundaries(UINT groups, float air = 0, bool onlyCircles = false){
      point ul;
      point lr;
      bool fst = true;
      vector<point> result;
      //i = ci.nxt();
      for(UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          if (circles[i].n & groups){
            if (fst){
              fst = false;
              ul.x = circles[i].x; ul.y = circles[i].y;
              lr.x = circles[i].x; lr.y = circles[i].y;
              result.push_back(ul); result.push_back(lr);
            }
            else{
              if (circles[i].x < result[0].x){
                result[0].x = circles[i].x;
              }
              if (circles[i].y < result[0].y){
                result[0].y = circles[i].y;
              }
              if (circles[i].x > result[1].x){
                result[1].x = circles[i].x;
              }
              if (circles[i].y > result[1].y){
                result[1].y = circles[i].y;
              }
            }
          }
        }
      }
      if (blSettings.doCheckTopol && !onlyCircles){
        for (UINT i = 0; i < bl.size(); i++){
          if (i & groups){
            for (UINT j = 0; j < bl[i].size(); j++){
              if (bl[i][j].x < result[0].x){
                result[0].x = bl[i][j].x;
              }
              if (bl[i][j].y < result[0].y){
                result[0].y = bl[i][j].y;
              }
              if (bl[i][j].x > result[1].x){
                result[1].x = bl[i][j].x;
              }
              if (bl[i][j].y > result[1].y){
                result[1].y = bl[i][j].y;
              }
            }
          }
        }
      }
      result[0].x -= air;
      result[0].y -= air;
      result[1].x += air;
      result[1].y += air;
      return result;
    }
    vector<point> getBoundaries(float air = 0, bool onlyCircles = false){
      UINT n = twoPow(ngroups) - 1;
      vector<point> result = getSetBoundaries(n, air, onlyCircles);
      return result;
    }

    vector<point> getOutsidePoints(bool logit = false){
      vector<point> p = getBoundaries(2 * maxRad());
      vector<point> q;
      point r;
      r.x = p[0].x;
      r.y = p[0].y;
      q.push_back(r);
      r.x = p[1].x;
      r.y = p[0].y;
      q.push_back(r);
      r.x = p[1].x;
      r.y = p[1].y;
      q.push_back(r);
      r.x = p[0].x;
      r.y = p[1].y;
      q.push_back(r);
      return q;
    }

    UINT chooseCombination(bool logit = false){
      vector<point> backup = circles;
      vector<point> best   = circles;
      addLines();
      polishLines();
      UINT bestout = countOutsiders();
      UINT bestcrs = countCrossings();
      if (logit){
        tolog(_L_ +  "Starting with " + toString(bestout) + " outsiders.\n");
      }
      for (UINT i = 0; i < circles.size() - 1; i++){
        if (circles[i].radius > 0){
          //vector<int> b = toBin(circles[i].n, twoPow(ngroups));
          //if (!isTopolIncorrect(circles[i], b)){
            for (UINT j = i + 1; j < circles.size(); j++){
              if (circles[j].radius > 0){
                swapCoords(i, j);
                addLines();
                polishLines();
                UINT out = countOutsiders();
                if (out <= bestout){
                  UINT crs = countCrossings();
                  if (out == bestout){
                    if (crs < bestcrs){
                      best = circles;
                      bestout = out;
                      bestcrs = crs;
                    }
                  }
                  else{
                    best = circles;
                    bestout = out;
                    bestcrs = crs;
                  }
                  if (logit){
                    tolog(_L_ +  "i: " + toString(i) + ", " +
                          "j: " + toString(j) + ", " + "out: " + toString(out) + "\n");
                  }
                  /*if (level < 2){
                    level++;
                    UINT tout = chooseCombination(level);
                    if (tout < bestout){
                      bestout = tout;
                    }
                  }*/
                }
                else{
                  swapCoords(i, j);
                }
              }
            //}
          }
        }
      }

      return bestout;
    }


    /***********************/
    /* Extensive search    */
    /***********************/

    void chooseCrossings(bool logit = false){
      fixTopology();
      UINT bc = countCrossings();
      tolog("Best crossings: " + toString(bc) + "\n");
      UINT nbc = setCrossings();
      while (nbc < bc){
        if (logit){
          tolog("New best crossings: " + toString(nbc) + "\n");
        }
        bc = nbc;
        nbc = setCrossings();
      }
    }

    UINT setCrossings(bool logit = false){
      fixTopology();
      UINT bestcross = countCrossings();
      float bestComp = compactness();
      //UINT bestout = countOutsiders();
      for (UINT i = 0; i < circles.size() - 1; i++){
        if (circles[i].radius > 0){
          for (UINT j = i + 1; j < circles.size(); j++){
            if (circles[j].radius > 0){
              swapCoords(i, j);
              fixTopology();
              float comp = compactness();
              UINT newcross = countCrossings();
              if (newcross < bestcross){
                bestcross = newcross;
                //bestout = newout;
              }
              else if (newcross == bestcross && comp < bestComp){
                bestcross = newcross;
                bestComp = comp;
              }
              else{
                swapCoords(i, j);
              }
            }
          }
        }
      }
      return bestcross;
    }

    void chooseCompact(bool logit = false){
      float best = compactness();
      float newc = setCompact();
      while (newc < best){
        if (logit){
          tolog("Better compact: " + toString(best) + " to " + toString(newc) + "\n");
        }
        best = newc;
        newc = compactness();
      }
    }

    float setCompact(){
      float bestComp = compactness();
      for (UINT i = 0; i < circles.size() - 1; i++){
        if (circles[i].radius > 0){
          for (UINT j = i + 1; j < circles.size(); j++){
            if (circles[j].radius > 0){
              swapCoords(i, j);
              fixTopology();
              float comp = compactness();
              if (comp < bestComp){
                bestComp = comp;
                //bestout = newout;
              }
              else{
                swapCoords(i, j);
              }
            }
          }
        }
      }
      return bestComp;
    }

    /***********************/
    /* /Extensive search   */
    /***********************/


    vector<UINT> sampleUINT(vector<UINT> from, UINT n, std::mt19937 *gen){
      UINT nc = from.size() - 1;
      vector<UINT> cp = from;
      vector<UINT> result;
      std::set<UINT> offsets;
      for (UINT i = 0; i < n; i++){
        std::uniform_int_distribution<UINT> distrib(0, nc);
        UINT ni = distrib(*gen);
        std::set<UINT>::iterator it;
        for (it = offsets.begin(); it != offsets.end(); it++){
          UINT v = *it;
          if (v <= ni){
            ni++;
          }
        }
        offsets.insert(ni);
        result.push_back(from[ni]);
        nc--;
      }
      return result;
    }

    /***********************/
    /* Metropolis-Hastings */
    /***********************/


    /** \brief Look for the best combination compactness
     *  using a Metropolis-Hastings-like algorithm
     *
     *
     * \return float Best compactness achieved
     *
     */
    float MHCompact(){
      UINT nstep = 2; // Number of circles exchanged in each step
      UINT maxSteps = 100000;
      std::random_device rd;  // Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
      std::uniform_real_distribution<float> alpha(0.0, 1.0);
      UINT cyclesWithoutImprovement = 0;
      float bestCompactness = compactness();
      vector<UINT> existingCircles = ncircles();
      UINT counter = 0;
      while (cyclesWithoutImprovement < 1000 && counter < maxSteps){
        vector<UINT> exch = sampleUINT(existingCircles, nstep, &gen);
        UINT ci = exch[0];
        UINT cj = exch[1];
        tolog("Exchanging " + toString(ci) + " with " + toString(cj) + "\n");
        swapCoords(ci, cj);
        float newCompactness = compactness();
        tolog("Old comp: " + toString(bestCompactness) + "\tNew comp: " + toString(newCompactness) + "\n");
        if (newCompactness < bestCompactness){
          cyclesWithoutImprovement = 0;
          bestCompactness = newCompactness;
        }
        else{
          float decd = alpha(gen);
          float rtio = bestCompactness / newCompactness;
          tolog("Decider: " + toString(decd) + ", " + "Ration: " + toString(rtio) + "\n");
          if (decd > rtio){
            swapCoords(ci, cj);
            tolog("Bailed\n");
          }
          cyclesWithoutImprovement++;
        }
        counter++;
      }
      return bestCompactness;
    }

    float MHCrosses(){
      UINT nstep = 2; // Number of circles exchanged in each step
      UINT maxSteps = 10000;
      std::random_device rd;  // Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
      std::uniform_real_distribution<float> alpha(0.0, 1.0);
      UINT cyclesWithoutImprovement = 0;
      fixTopology();
      float bestCrossings   = countCrossings();
      vector<UINT> existingCircles = ncircles();
      UINT counter = 0;
      while (cyclesWithoutImprovement < 500 && counter < maxSteps){
        vector<UINT> exch = sampleUINT(existingCircles, nstep, &gen);
        UINT ci = exch[0];
        UINT cj = exch[1];
        tolog("Exchanging " + toString(ci) + " with " + toString(cj) + "\n");
        swapCoords(ci, cj);
        fixTopology();
        if (checkTopol()){
          swapCoords(ci, cj);
        }
        else{
          float newCrossings = countCrossings();
          tolog("Old cross: " + toString(bestCrossings) + "\tNew cross: " + toString(newCrossings) + "\n");
          if (newCrossings < bestCrossings){
            cyclesWithoutImprovement = 0;
            bestCrossings = newCrossings;
          }
          else{
            float decd = alpha(gen);
            float rtio = bestCrossings / newCrossings;
            tolog("Decider: " + toString(decd) + ", " + "Ration: " + toString(rtio) + "\n");
            if (decd > rtio){
              swapCoords(ci, cj);
              tolog("Bailed\n");
            }
            cyclesWithoutImprovement++;
          }
          counter++;
        }
      }
      return bestCrossings;
    }

    /***********************/
    /* /Metropolis-Hastings*/
    /***********************/


    /***********************/
    /* Restrict to outliers*/
    /***********************/

    void resetOptimize(){
      for (UINT i = 0; i < circles.size(); i++){
        circles[i].flags = unsetFlag(circles[i].flags, DO_NOT_OPTIMIZE);
      }
    }

    UINT furthestPoint(){
      UINT result = 0;
      float best = 0;
      bool first = true;
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          float val = 0;
          if ((circles[i].flags & DO_NOT_OPTIMIZE) == 0){
            for (UINT j = 0; j < circles.size(); j++){
              if (i != j){
                UINT rel = getRelationships(i, j);
                if (rel > 0){
                  float dsq = sqDistance(circles[i], circles[j]);
                  val += log(rel) + log(dsq);
                  //tolog(toString(val) + "\n");
                }
              }
            }
            if ((first) || (val > best)){
              result = i;
              best = val;
              first = false;
            }
          }
        }
      }
      //tolog(_L_ + circles[result].croack());
      circles[result].flags = setFlag(circles[result].flags, DO_NOT_OPTIMIZE);
      if (result == 0){
        tolog("Error in getting furthest point\n");
      }
      tolog("Next: " + toString(result) + "\n");
      return result;
    }

    /** \brief Gets (one of) the circle that belongs to the least number of sets
     * The result must be higher than 0 and must have radius > 0. Never returns the same circle twice
     * until resetOptimize() is run.
     *
     * \return UINT
     *
     */
    UINT crossestPoint(){
      UINT result = 0;
      UINT crossest = 0;
      UINT mask = (1 << (ngroups)) - 1;
      bool first = true;
      for (UINT i = 1; i < circles.size(); i++){
        if (circles[i].radius > 0 && !(circles[i].flags & DO_NOT_OPTIMIZE)){
          UINT val = ones(circles[i].n & mask);
          //tolog(toString(circles[i].n) + ": " + toString(val) + "\n");
          if (first || val < crossest){
            result = i;
            crossest = val;
            first = false;
          }
        }
      }
      circles[result].flags = setFlag(circles[result].flags, DO_NOT_OPTIMIZE);
      //tolog("CNext: " + toString(circles[result].n) + "\n");
      return result;
    }

    float doNothing(){
      return 0;
    }

    float outCompactness(optimizationStep *opt, UINT (borderLine::*chooseCandidate)(),
                         float (borderLine::*countFunct)(), float (borderLine::*untieFunct)(),
                         bool logit = false){
      float untie = (this->*untieFunct)();
      if (opt->hasEnded()){
        opt->startCycle();
        opt->setCandidate((this->*chooseCandidate)());
      }
      else{
        UINT candidate = opt->getCandidate();
        if (candidate > 0){
          UINT i = opt->getCounter();
          if (i != candidate && circles[i].radius > 0){
            swapCoords(i, candidate);
            fixTopology();
            if (checkTopol()){
              swapCoords(i, candidate);
            }
            else{
              float newComp = (this->*countFunct)();
              if (newComp < opt->getBestCompactness()){
                opt->setBestCompactness(newComp);
                tolog("Swapping " + toString(i) + " with " + toString(candidate) + " -> " + toString(newComp) + "\n");
              }
              else if (newComp == opt->getBestCompactness()){
                float newUntie = (this->*untieFunct)();
                tolog("Prev untie: " + toString(untie) + ", new untie: " + toString(newUntie) + "\n");
                opt->setUntie();
                if (newUntie > untie){
                  swapCoords(i, candidate);
                  tolog("Did not swap on untie\n");
                }
              }
              else{
                swapCoords(i, candidate);
              }
            }
          }
        }
        opt->next();
        if (opt->getCounter() >= circles.size()){
          opt->endCycle();
        }
      }
      return opt->getBestCompactness();
    }

    UINT outCrosses(optimizationStep *opt, bool logit = false){
      UINT candidate = furthestPoint();
      fixTopology();
      UINT bestCross = countCrossings();
      for (UINT i = 0; i < circles.size(); i++){
        if (i != candidate){
          swapCoords(i, candidate);
          fixTopology();
          if (checkTopol()){
            swapCoords(i, candidate);
          }
          else{
            UINT newCross = countCrossings();
            if (newCross < bestCross){
              bestCross = newCross;
              tolog("Swapping " + toString(i) + " with " + toString(candidate) + "\n");
            }
            else{
              swapCoords(i, candidate);
            }
          }
        }
      }
      return bestCross;
    }

    /***********************/
    /*/Restrict to outliers*/
    /***********************/

    UINT setCrossingsBack(bool logit = false){
      addLines();
      polishLines();
      fixTopology();
      UINT bestcross = countCrossings();
      UINT bestout = countOutsiders();
      for (UINT i = 0; i < circles.size(); i++){
        /*UINT n = circles[i].n;
        while ((n > 0) && ((n & 0x1) == 0)){
          n = n >> 1;
        }*/
        if (/*n == */1){
          vector<point> q = getOutsidePoints();
          for (UINT j = 0; j < q.size(); j++){
            point u; u.x = circles[i].x; u.y = circles[i].y;
            circles[i].x = q[j].x;
            circles[i].y = q[j].y;
            addLines();
            polishLines();
            fixTopology();
            UINT newcross = countCrossings();
            UINT newout = countOutsiders();
            if ((newout <= bestout) && (newcross < bestcross)){
              bestout = newout;
              bestcross = newcross;
            }
            else{
              //tolog(toString(__LINE__) + "\n" + toString(i) + " with " + toString(j) + " was no better\n");
              circles[i].x = u.x;
              circles[i].y = u.y;
            }
          }
        }
      }
      return bestcross;
    }

    point spring(point &p0, point &p1, float dampen = 0, float springK = -1)
    {
        if (springK < 0){
          springK = blSettings.sk;
        }
        point result;
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
            result.fx = hardness * blSettings.sk * kx;
            result.fy = hardness * blSettings.sk * ky;
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

    void setSecureState(){
      if (minRat < 10){
        savedState.bl_secure = bl;
        ncycles_secure = blSettings.ncycles;
        resetOld();
        savedState.circles_secure = circles;
      }
    }

    void restoreSecureState(){
      bl = savedState.bl_secure;
      circles = savedState.circles_secure;
      deciderCounter = 0;
      keepDistCounter = 0;
      blSettings.ncycles = ncycles_secure;
      evaluation.init();
    }

    void setPrevState(){
      savedState.bl_old10 = bl;
      ncyles_old10 = blSettings.ncycles;
      resetOld();
      savedState.circles_old10 = circles;
    }

    void restorePrevState(){
      bl = savedState.bl_old10;
      circles = savedState.circles_old10;
      for (UINT i = 0; i < circles.size(); i++){
        circles[i].resetv();
      }
      deciderCounter = 0;
      keepDistCounter = 0;
      blSettings.ncycles = ncyles_old10;
    }

    void contact2(point &p0, point &p1){
      float trueRadius = p0.radius + p1.radius;
      float dx = p1.x - p0.x;
      float dy = p1.y - p0.y;
      float d = distance(p0.x, p0.y, p1.x, p1.y);
      if (d <= trueRadius){
        float fx = p0.fx + p1.fx;
        float fy = p0.fy + p1.fy;
        float modf = fx * dx + fy * dy;
        if (modf > 0){
          attention(p0.x, p0.y);
          attention(p1.x, p1.y);
        }
      }
    }

    point contact(point &p0, point &p1, float hardness = 5e1f, float radius = 0, float air = 0)
    {
      point result;
      if (blSettings.contactFunction == 1){
        contact2(p0, p1);
      }
      if (blSettings.contactFunction == 0){
        if (hardness == 0){
          hardness = 5e1f;
        }
          point zero;
          float d;
          float dx;
          float dy;
          float kx, ky;
          float crat;
          zero.fx = 0;
          zero.fy = 0;
          dx = p1.x - p0.x;
          dy = p1.y - p0.y;
          float trueRadius = p0.radius + p1.radius + air;
          if (radius == 0){
            radius = (trueRadius) * AIR;
          }
          //radius += blSettings.margin;
          //check contact
          d = distance(p0.x, p0.y, p1.x, p1.y);
          if (d <= (radius))
          {
              if (d <= radius){
                  blSettings.contacts++;
                  crat = (d - radius)/(radius);
                  kx = dx * (crat);
                  ky = dy * (crat);
                  result.fx = hardness * blSettings.sk * kx;
                  result.fy = hardness * blSettings.sk * ky;
                  p0.fx += result.fx;
                  p0.fy += result.fy;
                  p1.fx -= result.fx;
                  p1.fy -= result.fy;
                  p0.inContact = true;
                  p1.inContact = true;
                  attention(p1.x, p1.y);
                  if (blSettings.softcontact){
                    float dvx = p0.vx - p1.vx;
                    float dvy = p0.vy - p1.vy;
                    float dv = dvx * dvx + dvy * dvy;
                    float mdv = blSettings.maxv / blSettings.maxvcontact;
                    if (dv > mdv){
                      float rv = blSettings.maxv / (blSettings.maxvcontact * dv);
                      p0.vx *= rv;
                      p0.vy *= rv;
                      p1.vx *= rv;
                      p1.vy *= rv;
                    }
                  }

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
            return result;
          }
      }
      return result;
    }

    point rope(point &circ, point &p0, point &p1)
    {
        float a, b;
        float d;
        point result;
        point zero;
        point virt;
        float d0, d1;
        a = (p1.y - p0.y)/(p1.x - p0.x);
        b = p0.y - a * p0.x;
        virt.x = ((circ.x + a*circ.y - a * b)/(a*a + 1));
        virt.y = (a * circ.x + a*a*circ.y + b)/(a*a + 1);
        d0 = distance(p0.x, p0.y, virt.x, virt.y) /
             distance(p0.x, p0.y, p1.x, p1.y);
        d1 = distance(p1.x, p1.y, virt.x, virt.y) /
             distance(p0.x, p0.y, p1.x, p1.y);
        float td = d0 + d1;
        float rd = d0 / td;
        virt.vx = p0.vx + (p1.vx-p0.vx) * rd;
        virt.fx = p0.fx + (p1.fx-p0.fx) * rd;
        virt.vy = p0.vy + (p1.vy-p0.vy) * rd;
        virt.fy = p0.fy + (p1.fy-p0.fy) * rd;
        virt.mass = p0.mass;
        d = distance(circ.x, circ.y, virt.x, virt.y);
        if (d < circ.radius && isInside(p0, p1, virt))
        {
            result = contact(circ, virt);
            p0.fx = d1 * virt.fx * rd;
            p0.fy = d1 * virt.fy * rd;
            p1.fx = d0 * virt.fx * rd;
            p1.fy = d0 * virt.fy * rd;
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
      if (blSettings.dt < blSettings.maxdt){
        blSettings.dt *= blSettings.stepdt;
        deciderCounter.setAsStable();
      }
    }

    void setForces1()
    {
        UINT i, j;
        UINT size;
        point f;
        //line points
        /*******/
        float damp = blSettings.sk / 20;
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

    void setCircleAttraction(float G = 1e-5){
      float minDist = 4 * maxRad(); //  * sqrt((float) circles.size());
      for (UINT i = 0; i < (circles.size() - 1); i++){
        for (UINT j = i + 1; j < circles.size(); j++){
          if (distance(circles[i].x, circles[i].y, circles[j].x, circles[j].y) > minDist){
            selforce2(circles[i], circles[j], G);
          }
        }
      }
    }

    void setGravityForces(){
      for (UINT i = 0; i < bl.size(); i++){
        for (UINT j = 0; j < bl[i].size(); j++){
          for(UINT k = 0; k < circles.size(); k++){
            selforce(bl[i][j], circles[k]);
          }
        }
      }
    }

    void setForcesFirstStep(){
      UINT i, j;
      point f;
      /** Circle-circle **/
        for (i = 0; i < circles.size() - 1; i++){
          //float tmp = circles[i].radius;
          //circles[i].radius += 2 * ngroups * blSettings.marginScale;
          if (circles[i].radius > 0){

              for (j = i + 1; j < circles.size(); j++){
                 if (circles[j].radius > 0){ //float tmp2 = circles[j].radius;
                  //circles[j].radius += 2 * ngroups * blSettings.marginScale;
                  f = selforce(circles[i], circles[j]);
                  //circles[j].radius = tmp2;
                }
              }
          }
          //circles[i].radius = tmp;
        }
    }

    void setForcesSecondStep(){
      UINT i, j;
      point f;
      /** Circle-circle **/
        for (i = 0; i < circles.size() - 1; i++){
          //float tmp = circles[i].radius;
          //circles[i].radius += 2 * ngroups * blSettings.marginScale;
          if (circles[i].radius > 0){

              for (j = i + 1; j < circles.size(); j++){
                 if (circles[j].radius > 0){ //float tmp2 = circles[j].radius;
                  //circles[j].radius += 2 * ngroups * blSettings.marginScale;
                  f = selforce2(circles[i], circles[j]);
                  //circles[j].radius = tmp2;
                }
              }
          }
          //circles[i].radius = tmp;
        }
    }

    void setContacts(bool line_circle = true, bool circle_circle = true, float radius = 0)
    {
        UINT i, j, k;
        int size;
        point f;

        //contact forces
        /*******/
        //line-circle

        if (line_circle){
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
                          f = contact(circles[j], bl[k][i], 0, radius);
                          /******/
                      }

                  }
              }
          }
        }
        /*******/
        //circle-circle
        if (circle_circle){
          for (i = 0; i < circles.size() - 1; i++)
          {
              for (j = i + 1; j < circles.size(); j++)
              {
                  //f = eqforce(circles[i], circles[j]);
                  f = contact(circles[i], circles[j], 0, radius, 4* blSettings.margin);
              }
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

    int prevPoint(UINT i, UINT j){
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
          result = circles[k].n;
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
        point previous;
        point next;
        string temp;
        blSettings.fixCircles = true;
        float dampen = blSettings.sk / 2;
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
                        f = eqforce(circles[j], bl[k][i], 1e-5f * blSettings.sk);
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
            f = contact(circles[i], circles[j], 1e15f * blSettings->sk);
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

    void setBestSoFar(){
      savedState.bestCircles = circles;
      savedState.bestBl = bl;
      savedState.simTime = simulationTime;
      savedState.hasBeenSet = true;
    }

    void getBestSoFar(){
      if (savedState.hasBeenSet){
        circles = savedState.bestCircles;
        bl = savedState.bestBl;
        simulationTime = savedState.simTime;
        attachScene();
      }
    }

    void solve(bool resetVelocity = false, bool breakOnTopol = false)
    {
        float kb = blSettings.baseBV;
        warn.clear();
        dataDisplay.clear();

        blSettings.totalCircleV = 0;
        blSettings.totalLineV = 0;

        //Init the scale for the new frame
        internalScale.setClear(true);
        updPos(kb, resetVelocity);
        clearForces();
//Show dt
        displayFloat("SIMTIME", simulationTime);
        displayFloat("DT", blSettings.dt);
        displayUINT("CYCLES", blSettings.ncycles);
        displayUINT("NPOINTS", bl[0].size());
        displayFloat("POTENTIAL", log10(potential));

        potential = 0;

        for (UINT j = 0; j < circles.size(); j++){
          circles[j].flags = unsetFlag(circles[j].flags, IS_OUTSIDE);
        }
        if (blSettings.doCheckTopol == true && (checkTopol()))
        {
          if (breakOnTopol){
              tolog(_L_ + "Break on topol\n");
              writeSVG("error.svg");
              for (UINT j = 0; j < circles.size(); j++){
                if (circles[j].radius > 0){
                  point P = circles[j];
                  vector<int> b = toBin(circles[j].n, bl.size());
                  for (UINT i = 0; i < bl.size(); i++){
                    bool incorrect = circleTopol(P, b, i);
                    if (incorrect){
                      tolog(_L_ + "Circle " + toString(circles[j].n) + " is incorrect with line " + toString(i) + "\n");
                      tolog(_L_ + blSettings.cycleInfo + "\n");
                    }
                  }
                }
              }
            }
            if (udt.cdt() < blSettings.mindt){
              restoreSecureState();
              blCounter = 0;
            }
            else{
              restorePrevState();
              udt.report();
            }
        }
        else {
          if (blCounter == 0)
          {
              setPrevState();
              //udt.reset();
          }
          if (deciderCounter.isMax())
          {
              udt.poke();
          }
          if (keepDistCounter.isMax()){
              keepDist(0);
              if (checkTopol()){
                writeSVG("error.svg");
                listOutsiders();
                tolog(_L_ + "Break on KeepDist\n");
                restorePrevState();
              }
              else{
                setSecureState();
              }
          }
        }
        blSettings.dt = udt.cdt();
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
        keepDistCounter++;
        blSettings.ncycles++;
    }

    void updPos(float kb, bool resetVelocity = false)
    {
        UINT i, j;
        //Apply each force to each point
        if (blSettings.doCheckTopol == true){
          for (i = 0; i < bl.size(); i++)
          {
              for (j = 0; j < bl[i].size(); j++)
              {
                  bl[i][j].fx -= kb * bl[i][j].vx;
                  bl[i][j].fy -= kb * bl[i][j].vy;

                  //Limit force to avoid artifacts
                  //if (bl[i][j].softenVel == true){
                    limitVel(bl[i][j], blSettings.maxv);
                    //bl[i][j].softenVel = false;
                  //}
                  //

                  bl[i][j].vx += bl[i][j].fx * blSettings.dt / bl[i][j].mass;
                  bl[i][j].vy += bl[i][j].fy * blSettings.dt / bl[i][j].mass;
                  blSettings.totalLineV += bl[i][j].vx * bl[i][j].vx + bl[i][j].vy * bl[i][j].vy;

                  bl[i][j].x += bl[i][j].vx * blSettings.dt;
                  bl[i][j].y += bl[i][j].vy * blSettings.dt;
                  savedState.simTime += blSettings.dt;
                  /*******/
                  //attention(bl[i][j].x, bl[i][j].y);
                  /*******/
                  //limitVel(bl[i][j], maxv);
                  //Prepare the scale for the new frame


                  internalScale.addToScale(bl[i][j]);


                  if (resetVelocity)
                  {
                      bl[i][j].vx = 0;
                      bl[i][j].vy = 0;
                  }
              }

              //float p = perimeter(bl[i], true);
              //char* t = (char*) calloc(100, sizeof(char));
              //sprintf(t, "P%u: %.4f", i, p);
              //dataDisplay.push_back(t);
          }
        }
        for (i = 0; i < circles.size(); i++)
        {
          if (circles[i].radius > 0){
              if (isNAN(circles[i].fx)){
                tolog(_L_ + "Bad circle: " + circles[i].croack());
                writeSVG("error.svg");
                exit(0);
              }
              if (blSettings.doCheckTopol){
                  limitForce(circles[i], blSettings.maxf);
                  limitVel(circles[i], blSettings.maxv);
              }

              /*float sx = 0;
              if (circles[i].vx != 0){
                  sx = circles[i].vx / abs(circles[i].vx);
              }
              float sy = 0;
              if (circles[i].vy != 0){
                  sy = circles[i].vy / abs(circles[i].vy);
              }*/
              circles[i].fx -= kb * 10 * circles[i].vx;
              circles[i].fy -= kb * 10 * circles[i].vy;
              //Limit force to avoid artifacts
              if (circles[i].softenVel && blSettings.softcontact){
                limitVel(circles[i], blSettings.maxv / blSettings.maxvcontact);
                limitForce(circles[i], blSettings.maxf / blSettings.maxvcontact);
                circles[i].softenVel = false;
              }
              //
              circles[i].vx += circles[i].fx * blSettings.dt / (CIRCLE_MASS);
              circles[i].vy += circles[i].fy * blSettings.dt / (CIRCLE_MASS);
              blSettings.totalCircleV += circles[i].vx * circles[i].vx + circles[i].vy * circles[i].vy;
              //limitVel(circles[i], maxv);

              circles[i].x += circles[i].vx * blSettings.dt;
              circles[i].y += circles[i].vy * blSettings.dt;
              if (resetVelocity)
              {
                  circles[i].vx = 0;
                  circles[i].vy = 0;
              }
              internalScale.addToScale(circles[i]);
          }
        }
        displayFloat("LINEV", log(blSettings.totalLineV));
        displayFloat("MINRAT", minRat);
        displayFloat("CIRCLEV", log10(blSettings.totalCircleV)/(2*log10(ngroups)));
        displayFloat("SURFRATIO", estSurf());
    }

    float estSurf(int nPoints = 100){
      float tsurf = (float) (internalScale.xSpan() * internalScale.ySpan());
      tsurf /= (float) totalExpectedSurface;
      return tsurf;
    }

    bool circleTopol(point P, vector<int> belong, UINT j){
      bool mustBeIn = (belong[j] == 1);  //Must the circle be inside the curve?
      bool isIn = false;      //Is the circle inside the curve?
      vector<point> bnd = getBoundaries();
      float xmax = max(bnd[0].x, bnd[1].x) + 2 * maxRad() +  1;
      point p1 = P;
      point p2;
      p2.x = xmax;
      p2.y = P.y;
      blSettings.cycleInfo = "";
      for (UINT i = 0; i < bl[j].size(); i++){
        point p3 = bl[j][i];
        UINT np = nextPoint(j, i);
        point p4 = bl[j][np];
        crossResult cr = cross(p1, p2, p3, p4); //cont;
        //cout << i << "\t" << j << "\t" << np << endl;
        while (cr == cont){
          cr = cross(p1, p2, p3, p4);
          np = nextPoint(j, np);
          p4 = bl[j][np];
          //cout << "--" <<  i << "\t" << j << "\t" << np << endl;
        }

        if (cr == crosses){
          isIn = !isIn;
          //blSettings.cycleInfo += "Cross: \n" + p1.croack() + p2.croack() + p3.croack() + p4.croack() + "\n";
          //blSettings.cycleInfo += "i: " + toString(i) + ", j: " + toString(j) + "\n";
        }
      }
      if (isIn ^ mustBeIn)
      {
          //attention(circles[j].x, circles[j].y);
          /******DEBUG*/
          if (blSettings.doCheckTopol){
            attention(p1.x, p1.y);
            attention(p2.x, p2.y);
            for (UINT i = 0; i < bl[j].size(); i++){
              point p3 = bl[j][i];
              crossResult cr = cont;
              UINT np = nextPoint(j, i);
              point p4 = bl[j][np];
              while (cr == cont){
                cr = cross(p1, p2, p3, p4);
                np = nextPoint(j, np);
                p4 = bl[j][np];
              }
              if (cr == crosses){
                //isIn = !isIn;
                attention(p3.x, p3.y);
                attention(p4.x, p4.y);
                //blSettings.cycleInfo += "Crossing with: \n" + p1.croack() + p2.croack() + p3.croack() + p4.croack();
              }
            }
          }
          /*******
          tolog(toString(__LINE__) + "\n" + "Line " + toString(j) + ", " + P.croack() + "isIn: " + toString(isIn) + " mustBeIn: " +
                toString(mustBeIn) + "\n\n");
          *******/
          return true;
      }
      return false;
    }


    bool isTopolIncorrect(point P, vector<int> belong){
      if (!(P.radius > 0)){
        tolog(_L_ + "Called isTopoloCorrect with radius 0\n"); exit(1);
      }
      for (UINT j = 0; j < bl.size(); j++)
      {
          bool ct = circleTopol(P, belong, j);
          if (ct){
            if (blSettings.doCheckTopol == true){
              //tolog(toString(__LINE__) + "\n" + toString(P.n) + "\n");
            }
            return ct;
          }
      }
      return false;
    }

    bool checkTopol()
    {
      if (!blSettings.doCheckTopol){
        return false;
      }
      UINT i;
      for (i = 0; i < circles.size(); i++)
      {
          if (circles[i].radius > 0)
          { /* Circle has radius */
              vector<int> belong = toBin(circles[i].n, bl.size());
              bool result = isTopolIncorrect(circles[i], belong);
              if (result){
                return result;
              }
          } /* Circle has radius */
      }
      return false;
    }

    point getCross(point p1, point p2, point p3, point p4){
      point result;
      if (max(p1.x, p2.x) >= min(p3.x, p4.x) &&
          max(p1.y, p2.y) >= min(p3.y, p4.y) &&
          min(p1.x, p2.x) <= max(p3.x, p4.x) &&
          min(p1.y, p2.y) <= max(p3.y, p4.y)){
        if (p2.x == p1.x){
          result.x = p1.x;
          result.y = p3.y + (result.x - p3.x) * (p4.y - p3.y) / (p4.x - p3.x);
        }
        else if (p4.x == p3.x){
          result.x = p3.x;
          result.y = p1.y + (result.x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
        }
        else{
          float m1 = (p2.y - p1.y) / (p2.x - p1.x);
          float m3 = (p4.y - p3.y) / (p4.x - p3.x);
          result.x = (p3.y - m3 * p3.x - p1.y + m1 * p1.x) / (m1 - m3);
          result.y = p1.y + m1 * (result.x - p1.x);
        }
      }
      return result;
    }

    /** \brief Do segments p1-p2 and p3-p4 cross?
     *
     * \param p1 point
     * \param p2 point
     * \param p3 point
     * \param p4 point
     * \return crossResult Three possible results: cross, doesnotcross
     * or cont. The last one means that the answer is ambiguous
     * and must be deferred.
     *
     */
    crossResult cross(point p1, point p2, point p3, point p4){
      crossResult result = doesnotcross;
      if (max(p1.x, p2.x) >= min(p3.x, p4.x) &&
          max(p1.y, p2.y) >= min(p3.y, p4.y) &&
          min(p1.x, p2.x) <= max(p3.x, p4.x) &&
          min(p1.y, p2.y) <= max(p3.y, p4.y)){
        if (p1 == p3 || p2 == p3){
          return crosses;
        }
        if (p2 == p4 || p1 == p4){
          return doesnotcross;
        }
        tangent t1 = tangent(p1, p3);
        tangent t2 = tangent(p3, p2);
        tangent t3 = tangent(p2, p4);
        tangent t4 = tangent(p4, p1);
        t2.rotate(t1);
        t3.rotate(t1);
        t4.rotate(t1);
        tangent tz = tangent(-1, 0);
        if (t2 == tz){
          result = doesnotcross;
        }
        else if (t3 == t4){
          result = cont;
        }
        else{
          if (((t2 <= t3) && (t3 <= t4)) ||
              ((t3 <= t2) && (t4 <= t3))){
            result = crosses;
          }
        }
        //blSettings.cycleInfo += "Tangents: \n" + t1.croack() + t2.croack() + t3.croack() + t4.croack();
      }
      return result;
    }


    float countCrossings(){
      float result = 0;
      debug.clear();
      vector<vector<point>> useme = bl;
      // Unembellish
      for (UINT i = 0; i < bl.size(); i++){
        UINT cunem = 0;
        useme[i].clear();
        for (UINT j = 0; j < bl[i].size(); j++){
          if ((bl[i][j].flags & DELME) == 0){
            useme[i].push_back(bl[i][j]);
          }
          else if (bl[i][j].n != cunem){
            useme[i].push_back(circles[bl[i][j].n]);
            cunem = bl[i][j].n;
          }
        }
      }
      //
      for (UINT i = 0; i < useme.size()-1; i++){
        for (UINT ii = 0; ii < useme[i].size(); ii++){
          point p1 = useme[i][ii];
          UINT npi =  0;
          if (ii < (useme[i].size() - 1)){
            npi = ii + 1;
          }
          point p2 = useme[i][npi];
          for (UINT j = i + 1; j < useme.size(); j++){
            for (UINT jj = 0; jj < useme[j].size(); jj++){
              point p3 = useme[j][jj];
              UINT np =  0;
              if (jj < (useme[j].size() - 1)){
                np = jj + 1;
              }
              point p4 = useme[j][np];
              if (p1 != p3 && p1 != p4 && p2 != p3 && p2 != p4){
                crossResult cr = cont;
                while (cr == cont){
                  p4 = useme[j][np];
                  cr = cross(p1, p2, p3, p4);
                  np =  0;
                  if (jj < (useme[j].size() - 1)){
                    np = jj + 1;
                  }
                }
                if (cr == crosses){
                  result = result + 1;
                  /*point crss = getCross(p1, p2, p3, p4);
                  tolog("----------\n");
                  tolog(p1.croack() + p2.croack() + p3.croack()+p4.croack() + crss.croack());
                  tolog(toString(result) + "\n");
                  tolog("----------");
                  debug.push_back(crss);*/
                }
              }
            }
          }
        }
      }

      return result;
    }

    void showCrossings(){
      debug.clear();
      vector<vector<point>> useme = bl;
      // Unembellish
      for (UINT i = 0; i < bl.size(); i++){
        UINT cunem = 0;
        useme[i].clear();
        for (UINT j = 0; j < bl[i].size(); j++){
          if ((bl[i][j].flags & DELME) == 0){
            useme[i].push_back(bl[i][j]);
          }
          else if (bl[i][j].n != cunem){
            useme[i].push_back(circles[bl[i][j].n]);
            cunem = bl[i][j].n;
          }
        }
      }for (UINT i = 0; i < useme.size()-1; i++){
        for (UINT ii = 0; ii < useme[i].size(); ii++){
          point p1 = useme[i][ii];
          UINT npi =  0;
          if (ii < (useme[i].size() - 1)){
            npi = ii + 1;
          }
          point p2 = useme[i][npi];
          for (UINT j = i + 1; j < useme.size(); j++){
            for (UINT jj = 0; jj < useme[j].size(); jj++){
              point p3 = useme[j][jj];
              UINT np =  0;
              if (jj < (useme[j].size() - 1)){
                np = jj + 1;
              }
              point p4 = useme[j][np];
              if (p1 != p3 && p1 != p4 && p2 != p3 && p2 != p4){
                crossResult cr = cont;
                while (cr == cont){
                  p4 = useme[j][np];
                  cr = cross(p1, p2, p3, p4);
                  np =  0;
                  if (jj < (useme[j].size() - 1)){
                    np = jj + 1;
                  }
                }
                if (cr == crosses){
                  point crss = getCross(p1, p2, p3, p4);
                  debug.push_back(crss);
                }
              }
            }
          }
        }
      }
      vector<vector<point>> delme = bl;
      bl = useme;
      writeSVG("starting.svg");
      bl = delme;
    }

    UINT countOutsiders(){
      UINT result = 0;
      for (UINT i = 0; i < circles.size(); i++)
      {
          if (circles[i].radius > 0)
          { /* Circle has radius */
              vector<int> belong = toBin(circles[i].n, bl.size());
              bool incorrect = isTopolIncorrect(circles[i], belong);
              if (incorrect){
                circles[i].flags = circles[i].flags | IS_OUTSIDE;
                //tolog(toString(__LINE__) + "\n" + "Circle " + toString(circles[i].n)+ "\n");
                for (UINT j = 0; j < bl.size(); j++){
                  if (circleTopol(circles[i], belong, j)){
                    result++;
                  }
                }
              }
              else{
                circles[i].flags = circles[i].flags & (~IS_OUTSIDE);
              }
          } /* Circle has radius */
      }
      //tolog(toString(__LINE__) + "\n" + "---\n");
      return result;
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


/** \brief Gets the relationship as calculated with setRelationships()
 *
 * \param o UINT Number of circle 1
 * \param t UINT Number of circle 2
 * \return UINT How many bytes are 1 in both numbers associated with the circles
 *
 */
UINT getRelationships(UINT o, UINT t){
  UINT i = min(o, t);
  UINT j = max(o, t);
  if (i == j){
    return 0;
  }
  UINT r = 0;
  UINT N = circles.size();
  i++;
  UINT row = ((2*i-2) * N - i * i + i) / 2;
  r = row + j - i;
  return relationships[r];
}


/** \brief Count the number of ones in the binary representation of a number
 *
 * \param n UINT Number to examine
 * \return UINT Number of 1s in the binary representation of @n
 *
 */
UINT ones(UINT n){
  UINT r = 0;
  while (n > 0){
    if (n & 1){
      r++;
    }
    n = n >> 1;
  }
  return r;
}

/** \brief Set relationships (number of shared bytes) between circles
 *
 * \return void
 *
 */
void setRelationships(){
  UINT i;
  UINT j;
  for (i = 0; i < (circles.size() - 1); i++){
    for (j = i + 1; j < circles.size(); j++){
      UINT r = 0;
      UINT bits = circles[i].n & circles[j].n;
      r = ones(bits);
      relationships.push_back(r);
      //cout << i << ", " << j << " - " << r << "\n";
    }
  }
}

public:
    borderLine(){}
    borderLine(binMap* b, vector<string> g, vector<float> tw, vector<string> tlabels, string inputFile = "venn.txt", string outputFile = "result.svg") /// aqui
    {
        UINT i;
        bm = b;
        groups = g;
        ngroups = bm->ngroups;
        minCircRadius = 1.0f;
        nPointsMin = 10;
        potential = 0;
        maxRadius = 0;
        internalScale.initScale();
        svgScale.initScale();
        svgScale.setMinX(10.0f);
        svgScale.setMinY(10.0f);
        svgScale.setMaxX(490.0f);
        svgScale.setMaxY(490.0f);
        initBlData(&blSettings);
        minRat = 0;
        showThis = true;
        blSettings.signalEnd = false;
        blSettings.contacts = 0;
        blSettings.fixCircles = false;
        blSettings.minratio = 0.1f * (ngroups * ngroups * ngroups)/ (4 * 4 * 4);
        blSettings.totalCircleV = 0;
        blSettings.totalLineV   = 0;
        blSettings.minSurfRatio = 0;
        blSettings.maxf = 5e1f;
        blSettings.maxv = 5e1f;
        blSettings.softcontact = false;
        blSettings.maxvcontact = 50;
        blSettings.startdt = blSettings.dt;
        blSettings.stepdt = 0.04f;
        blSettings.inputFile = inputFile;
        blSettings.fname = outputFile;
        blSettings.ncycles = 0;
        blSettings.cycleInfo = "";
        blSettings.lineAir = ngroups;
        simulationTime = 0;
        srand(time(0));
        w = tw;         //keep a copy of the weights
        for (i = 0; i < tw.size(); i++){
          origw.push_back(tw[i]);
        }
        wlimit();


        //init circles
        setCircles(*bm, origw, tlabels);
        setRelationships(); /* How many bits does each pair of circles share */

        /**/
        // Set the width of a line
        point labs(1 / svgScale.xSpan(), 0);
        point lsvg = place(internalScale, labs);
        blSettings.marginScale = lsvg.x;
        blSettings.margin = 1.2 * ngroups * blSettings.marginScale;
        /**/
        ostringstream l;
        l << "\t";
        for (UINT i = 0; i < circles.size(); i++){
          l << '"' << circles[i].n << '"' << "\t";
        }
        l << "\n";
        for (UINT i = 0; i < circles.size(); i++){
          l << '"' << circles[i].n << '"' << "\t";
          for (UINT j = 0; j < circles.size(); j++){
            l << getRelationships(i, j) << "\t";
          }
          l << "\n";
        }
        //tolog(toString(__LINE__) + "\n" + l.str());
        /**/

        totalExpectedSurface = 0;
        for (i = 0; i < w.size(); i++){
          totalExpectedSurface += (int) (circles[i].radius * circles[i].radius);
        }

        //init counters
        blCounter.setLimits(0, 50u);
        deciderCounter.setLimits(0, 50u);
        keepDistCounter.setLimits(0, 149u);
        refreshScreen.setLimits(1, 5);

        //init internal scale
        internalScale.setClear(true);

        //init time parameters
        udt.init();

        //init points
        for (i = 0; i < ngroups; i++)
        {
            p.clear();
            setPoints(*bm, i);
            bl.push_back(p);
        }
        startPerim = (UINT) perimeter(bl[0]);
        UINT np = (UINT) (0.5f * (float) startPerim);
        interpolate(np);

        evaluation.setConstants(100, 50);
        setPrevState();
        setSecureState();
        savedState.hasBeenSet = false;
        randomizeCircles();



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
          string c = vformat("#%06x", arr[i]);
          svgcolors.push_back(c.c_str());
        }
        //init colors
        for (i = 0; i < ngroups; i++)
        {
          colors.push_back(toRGB(arr[i], 1));
        }
        /*writeSVG()*/
    }

    float correctedMinCircRadius(){
      float result = minCircRadius;
      float minCoord = internalScale.xSpan();
      if (internalScale.ySpan() < minCoord) minCoord = internalScale.ySpan();
      minRat = 0.02 * minCoord;
      point t; t.radius = minCircRadius; t.x = 0; t.y = 0;
      point test = place(internalScale, t);
      float rat = test.radius / minRat;
      if (rat < 1){
        result = minCircRadius / rat;
      }
      return result;
    }

    /** \brief Minimal distance between two circles. Used to
     *         finish the second step
     *
     * \return float
     *
     */
    float minCircDist(){
      circleIterator ci(circles, circles.size());
      UINT fst = ci.val();
      UINT scnd = ci.nxt();
      float r = sqDistance(circles[fst], circles[scnd]);
      ci.reset();
      circleIterator ci2(circles, circles.size());
      while (!ci.isFinished()){
        UINT v = ci.val();
        ci2.reset(v);
        UINT w = ci2.nxt();
        while (!ci2.isFinished()){
          w = ci2.val();
          float nr = sqDistance(circles[v], circles[w]);
          if (nr < r){
            r = nr;
          }
          ci2.nxt();
        }
        ci.nxt();
      }

      float result = sqrt(r);
      return result;
    }

    bool isSimulationComplete(){
      bool result = evaluation.finished();
      if (result){
        getBestSoFar();
        evaluation.init();
      }
      return result;
    }

    void attachScene(float springK = 5e3){
      UINT cnt = 0;
      tosolve.clearScene();
      pairDistances.clear();
      UINT level = 1;
      float cushion = 0.05;
      for (UINT i = 0; i < bl.size(); i++){
        UINT lp = cnt;
        for (UINT j = 0; j < bl[i].size(); j++){
          //bl[i][j].flags = bl[i][j].flags | INGRAVID;
          bl[i][j].flags = bl[i][j].flags | GHOST;
          tosolve.addPointP(&(bl[i][j]));
          tosolve.addLink(cnt + lp, cnt + j, springK);
          lp = j;
          pairDistances.push_back(cushion * (float) level);
        }
        tosolve.addLink(cnt + lp, cnt, springK);
        cnt += lp + 1;
        level++;
      }
      level++;
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          tosolve.addPointP(&(circles[i]));
          pairDistances.push_back(cushion * (float) level);
        }
      }
      tosolve.setCushions(pairDistances);
    }

    void resetScale(){
      internalScale.setClear(true);
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          internalScale.addToScale(circles[i]);
        }
      }
      if (checkTopol()){
        for (UINT i = 0; i < bl.size(); i++){
          for (UINT j = 0; j < bl[i].size(); j++){
              internalScale.addToScale(bl[i][j]);
          }
        }
      }
    }

    void scFriction(float f = 50){
      tosolve.setFriction(f);
    }
    void scG(float G = 0){
      tosolve.setG(G);
    }
    void scGhostGrav(bool ghostGrav = true){
      tosolve.setGhostGravity(ghostGrav);
    }
    void scD(float D = 0){
      tosolve.setDampingConstant(D);
    }
    void scSpringK(float k = 1e3){
      tosolve.setSpringK(k);
    }
    void scSolve(){
      dataDisplay.clear();
      blSettings.dt = tosolve.solve(blSettings.dt);
      bool incorrect = checkTopol();
      while (incorrect){
        restorePrevState();
        //tolog(_L_ + "Bad topol\n");
        udt.report();
        if (blSettings.dt < blSettings.mindt){
          tolog(_L_ + "Cannot solve topol problems\n");
          toSVG();
          exit(0);
        }
        blSettings.dt = udt.cdt();
        //tolog(_L_ + "Bad topol: " + toString(udt.cdt()) + "\n");
        tosolve.solve(blSettings.dt);
        incorrect = checkTopol();
      }
      if (blCounter == 0){
          setPrevState();
      }
      if (deciderCounter.isMax()){
          setPrevState();
          udt.poke();
          blSettings.dt = udt.cdt();
      }
      if (keepDistCounter.isMax()){
          keepDist(avgStartDist);
          if (checkTopol()){
            writeSVG("error.svg");
            listOutsiders();
            tolog(_L_ + "Break on KeepDist\n");
            restorePrevState();
          }
          else{
            setSecureState();
          }
      }
      resetScale();
      resetCircleRadius();
      float d = compactness();
      evaluation.add(d);
      if (evaluation.keepState()){
        setBestSoFar();
      }
      displayFloat("DT", blSettings.dt);
      displayFloat("SIMTIME", tosolve.simTime());
      displayFloat("COMPACTNESS", d);
      displayFloat("LASTCOMPACTNESS", evaluation.viewLastComp());
      displayUINT("COUNTER", evaluation.viewCounter());
      displayUINT("CANFINISH", evaluation.viewCanFinish());
      displayUINT("CWI", evaluation.viewCyclesWithoutImprovement());
      //displayUINT("DECIDER", deciderCounter);
      blCounter++;
      deciderCounter++;
      keepDistCounter++;
      blSettings.ncycles++;
    }
    void scSave(string fname = ""){
      tosolve.saveScene();
    }

    string scCroack(){
      return tosolve.croack();
    }

    /** \brief Set baseBV (Reference friction coefficient)
     *
     * \param bv float New value (5.0 by default)
     * \return void
     *
     */
    void setBV(float bv){
      blSettings.baseBV = bv;
    }
    /** \brief Set the function to compute contacts
     *
     * \param f UINT 0 means contact(); 1 means contact2()
     * \return void
     *
     */
    void setContactFunction(UINT f){
      blSettings.contactFunction = f;
    }
    vector<UINT> ncircles(){
      vector<UINT> result;
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          result.push_back(i);
        }
      }
      return result;
    }
    UINT nregions(){
      UINT result = 0;
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          result++;
        }
      }
      return result;
    }
    void setSoftContact(bool soft = false){
      blSettings.softcontact = soft;
    }
    float getTotalCircleV(){
      return blSettings.totalCircleV;
    }

    void setFixedCircles(bool fixedCircles = true){
      blSettings.fixCircles = fixedCircles;
    }

    void setCheckTopol(bool doCheck = true){
      blSettings.doCheckTopol = doCheck;
    }

    float maxRad(){
      return maxRadius;
    }

    bool isThisTheEnd(){
      return blSettings.signalEnd;
    }

    vector<vector<point> > getPoints(){
      return bl;
    }

    /** \brief Sets the starting coordinates from a previous execution
     *
     * \param dataFile string File with coordinates from borderLine::saveFigure
     * \return void
     *
     */
    void setCoords(string dataFile){
        ifstream vFile;
        vFile.open(dataFile.c_str());
        UINT ncI = 0;
        if (vFile.good() == true){
            bl.clear();
            savedState.bl_secure.clear();
            savedState.bl_old10.clear();
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
                    point p;
                    p.x = x; p.y = y;
                    internalScale.addToScale(p);
                    thisline.push_back(p);
                    getline(vFile, line);
                }
                bl.push_back(thisline);
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

    void showInfo(){
      cout << "Number of groups: " << ngroups << "\n";
      cout << "Number of circles: " << circles.size() << "\n";
      for (UINT i = 0; i < circles.size(); i++){
        cout << "\t" << "circle" << i << " - (" << circles[i].x << ", " << circles[i].y << ")\n";
        cout << "\t" << "radius - " << circles[i].radius << "\n";
      }
      cout << "Internal scale - " << "(" << internalScale.minX() << ", " << internalScale.minY() << ") to (" <<
                                            internalScale.maxX() << ", " << internalScale.maxY() << ") \n";
      cout << "Spans: " << "(" << internalScale.xSpan() << ", " << internalScale.ySpan() << ")\n";
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

    const std::string vformat(const char * const zcFormat, ...) {

      // initialize use of the variable argument array
      va_list vaArgs;
      va_start(vaArgs, zcFormat);

      // reliably acquire the size
      // from a copy of the variable argument array
      // and a functionally reliable call to mock the formatting
      va_list vaArgsCopy;
      va_copy(vaArgsCopy, vaArgs);
      const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
      va_end(vaArgsCopy);

      // return a formatted string without risking memory mismanagement
      // and without assuming any compiler or platform specific behavior
      std::vector<char> zc(iLen + 1);
      std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
      va_end(vaArgs);
      return std::string(zc.data(), iLen);
    }

    string croack(){
      string result = internalScale.croack();
      for (UINT i = 0; i < circles.size(); i++){
        result += circles[i].croack();
      }
      return result;
    }

    void clearWarnings(){
      warn.clear();
    }

    vector<point> getWarnings(){
      vector<point> result = warn;
      return result;
    }

    string coord(float c){
      string result = vformat("%.2f", c);
      return result;
    }

    string num(int c){
      string result = vformat("%d", c);

      return result;
    }

    point fstCtrlPoint(point prev, point start, point nxt, float sc = 0.5f){
      point result;
      result.x = start.x + sc * (nxt.x - prev.x);
      result.y = start.y + sc * (nxt.y - prev.y);
      return result;
    }

    point scndCtrlPoint(point prev, point start, point nxt, float sc = 0.5f){
      point result;
      result.x = start.x - sc * (nxt.x - prev.x);
      result.y = start.y - sc * (nxt.y - prev.y);
      return result;
    }

    string join(string interm, vector<string> arr) {
      int i;
      string result = "";
      if (arr.size() == 1) {
        result = arr[0];
      } else if (arr.size() > 1) {
        int j = arr.size() - 1;
        if (j > 0) {
          for (i = 0; i < j; i++) {
            result += arr[i] + interm;
          }
          result += arr[j];
        }
      }
      return result;
    }

    fileText toSVG(){
      getBestSoFar();
      fileText svg;
      int fsize = 10;
      UINT i, j;
      string tst;
      point svgtemp;
      svg.addLine("<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"700\" height=\"500\">");
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
      svg.addLine("  .spcircle {");
      svg.addLine("	   stroke: #FF2222;");
      svg.addLine("	   stroke-width: 1;");
      svg.addLine("	   fill: none;");
      svg.addLine("    pointer-events: all;");
      svg.addLine("  }");
      svg.addLine("  .tLabel {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      svg.addText("	   font-size: ");
      svg.addText(num(fsize+4));
      svg.addLine("px;");
      svg.addLine("	   text-anchor: middle;");
      svg.addLine("	   alignment-baseline: central;");
      svg.addLine("  }");
      svg.addLine("  .nLabel {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      svg.addText("	   font-size: ");
      svg.addText(num(fsize));
      svg.addLine("px;");
      svg.addLine("	   text-anchor: middle;");
      svg.addLine("	   alignment-baseline: central;");
      svg.addLine("  }");
      svg.addLine("  .belong {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      svg.addText("	   font-size: ");
      svg.addText(num((int) fsize / 2));
      svg.addLine("px;");
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
      if (blSettings.doCheckTopol){
        if (blSettings.smoothSVG == true){
          for (i = 0; i < ngroups; i++){
            point nxt = place(svgScale, bl[i][0]);
            string cpath = "M " + coord(nxt.x) + " " + coord(nxt.y);

            for (j = 1; j < (bl[i].size() - 2); j++){
              point prev = place(svgScale, bl[i][j - 1]);
              point curr = place(svgScale, bl[i][j]);
              point next = place(svgScale, bl[i][j + 1]);
              point next2 = place(svgScale, bl[i][j + 2]);
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
            point nxt = place(svgScale, bl[i][0]);
            string cpath = "M " + coord(nxt.x) + " " + coord(nxt.y);
            for (j = 1; j < bl[i].size(); j++){
              nxt = place(svgScale, bl[i][j]);
              cpath += " L " + coord(nxt.x) + " " + coord(nxt.y);
            }
            svg.addLine("<symbol id=\"bl" + num(i) + "\">");
            svg.addLine("<path d=\"" + cpath + " Z\" />");
            svg.addLine("</symbol>");
          }
        }
      }
      svg.addLine("</defs>");
      svg.addLine("<!-- isDone: " + nc + " -->");
      svg.addLine("<rect width=\"700\" height=\"500\" style=\"fill:#fff;stroke-width:0\" />");

      // Add fills
      if (blSettings.doCheckTopol){
        for (i = 0; i < ngroups; i++){
          svg.addLine("<use class=\"p" + num(i) + " borderLine\" xlink:href=\"#bl" + num(i) + "\"/>");
        }
        // Add strokes
        for (i = 0; i < ngroups; i++){
          svg.addLine("<use class=\"q" + num(i) + " outLine\" xlink:href=\"#bl" + num(i) + "\"/>");
        }
      }
      if (showThis){
        for (UINT i = 0; i < bl.size(); i++){
          for (UINT j = 0; j < bl[i].size(); j++){
            point nxt = place(svgScale, bl[i][j]);
            if ((bl[i][j].flags & DELME) > 0){
              string tmp = vformat("<circle class=\"%s\" cx=\"%.4f\" cy=\"%.4f\" r=\"2\" />", "spcircle", nxt.x, nxt.y);
              svg.addLine(tmp);
            }
          }
        }
        for (UINT i = 0; i < debug.size(); i++){
          point t = place(svgScale, debug[i]);
          string tmp = vformat("<circle class=\"%s\" cx=\"%.4f\" cy=\"%.4f\" r=\"2\" />", "spcircle", t.x, t.y);
          svg.addLine(tmp);
        }
      }
      for (i = 1; i < circles.size(); i++){
        //printf("%d\n", i);
        if (circles[i].radius > 0){
          svgtemp = place(svgScale, circles[i]);
          //printf("%.4f, %.4f, %.4f\n", svgtemp.x, sc.minX, sc.maxX);
          if (svgtemp.x > svgScale.minX() && svgtemp.x < svgScale.maxX()){
            string clss = "circle";
            if ((circles[i].flags & IS_OUTSIDE) > 0){
              clss = "spcircle";
            }
            tst = vformat("<circle onclick=\"fromCircle(%u)\" class=\"%s\" cx=\"%.4f\" cy=\"%.4f\" r=\"%.4f\" />", circles[i].n, clss.c_str(), svgtemp.x,
                            svgtemp.y, svgtemp.radius);
            svg.addLine(tst);
            tst = vformat("<text class=\"tLabel\" x=\"%.2f\" y=\"%.2f\">%s</text>", svgtemp.x, svgtemp.y - 4*fsize/2, labels[i].c_str());
            svg.addLine(tst);
            tst = vformat("<text class=\"nLabel\" x=\"%.2f\" y=\"%.2f\">%g</text>", svgtemp.x, svgtemp.y - fsize/2, circles[i].orig);
            svg.addLine(tst);
            // Belongs to
            vector<int> tb = toBin(circles[i].n, bl.size());
            vector<string> blongs;
            UINT m;
            for (m = 0; m < tb.size(); m++){
              if (tb[m] > 0){
                string t = vformat("%d", m + 1);
                blongs.push_back(t);
              }
            }
            string bgs = join(", ", blongs);
            string t = vformat("<text class=\"belong\" x=\"%.2f\" y=\"%.2f\">(%s)</text>", svgtemp.x, svgtemp.y + fsize / 2, bgs.c_str());
            svg.addLine(t);
          }
        }
      }
      // Legend
      float cx = 500.0f;
      float cy = 50.0f;
      float rw = 30.0f;
      float rh = 15.0f;
      float dy = 40.0f;
      float dx = 40.0f;
      for (UINT l = 0; l < ngroups; l++){
        string g = groups[l];
        string myg = vformat("p%d", l);
        string myq = vformat("q%d", l);
        string addRect = vformat("<rect class=\"%s borderLine\" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />",
                myg.c_str(), cx, cy, rw, rh);
        string addOut = vformat("<rect class=\"%s\" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />",
                myq.c_str(), cx, cy, rw, rh);
        string addLegend = vformat("<text class=\"legend\" x=\"%.2f\" y=\"%.2f\">%s</text>", cx + dx, cy + rh, g.c_str());
        svg.addLine(addRect);
        svg.addLine(addOut);
        svg.addLine(addLegend);
        cy += dy;
      }
      svg.addLine("</svg>");
      return svg;
    }

    fileText toPS()
    {
        fileText pstext;
        string tst;
        UINT i, j;
        point pstemp;
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
        string st = vformat("/step %u def", ngroups);
        pstext.addLine(st);
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
                tst = vformat("%f %f", pstemp.x, pstemp.y);
                pstext.addLine(tst);
            }
            tst = (string) "]";
            pstext.addLine(tst);
            tst = vformat("/set%d exch def", i + 1);
            pstext.addLine(tst);
            tst = vformat("set%d minmax", i + 1);
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
            tst = vformat("set%d topath", i+1);
            pstext.addLine(tst);
            pstext.addLine("clip");
            tst = vformat("%f %f %f %u %u colorpattern", colors[i].red,
                            colors[i].green, colors[i].blue, offset, offset);
            pstext.addLine(tst);
            pstext.addLine("grestore");
            offset++;
        }
        pstext.addLine(" ");

        for (i = 0; i < ngroups; i++)
        {
            tst = vformat("%f %f %f set%d showline", colors[i].red,
                            colors[i].green, colors[i].blue, i+1);
            pstext.addLine(tst);
        }
        pstext.addLine(" ");

        /****Draw circles*/
        for (i = 0; i < circles.size(); i++){
            if (circles[i].mass > 0){
              pstemp = place(ps, circles[i]);
              pstext.addLine("newpath");
              if (pstemp.x > ps.minX() && pstemp.x < ps.maxX()){
                tst = vformat("%f %f %f 0 360 arc", pstemp.x,
                                pstemp.y, pstemp.radius);
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



    void keepDist(float minDist){
      if (blSettings.doCheckTopol){
        //float md = minDist;
        //if (md < minCircScreenRadius){
          float md = minCircScreenRadius;
        //}
        for (UINT i = 0; i < bl.size(); i++){
          UINT n = twoPow(i);
          vector<point> q = getSetBoundaries(n, 2*maxRadius*AIR, true);
          for (UINT j = 0; j < bl[i].size(); j++){
            if (bl[i][j].x < q[0].x ){
              bl[i][j].x = q[0].x;
            }
            else if (bl[i][j].x > q[1].x){
              bl[i][j].x = q[1].x;
            }
            if (bl[i][j].y < q[0].y){
              bl[i][j].y = q[0].y;
            }
            else if (bl[i][j].y > q[1].y){
              bl[i][j].y = q[1].y;
            }
          }
          UINT counter = 0;
          UINT lastConserved = 0;
          for (UINT j = 0; j < bl[i].size() - 2; j++){
            float d = distance(bl[i][lastConserved].x, bl[i][lastConserved].y, bl[i][j+1].x, bl[i][j+1].y);
            if (d < (md)){
              bl[i][j].flags = bl[i][j].flags | DELME;
              counter++;
              //tolog("Distance between " + toString(lastConserved) + " and " +
              //      toString(j+1) + " is " + toString(d) + "\n");
              //bl[i].erase(bl[i].begin() + j+ 1);
            }
            else{
              lastConserved = j + 1;
            }
          }
          //if (counter > 0){
            //tolog("Took " + toString(counter) + " points from line " + toString(i + 1) + "\n");
            //tolog("Using minCircScreenRadius " + toString(md) + "\n");
          //}
          vector<point> rb;
          vector<point> backup = bl[i];
          for (UINT j = 0; j < bl[i].size(); j++){
            if (!(bl[i][j].flags & DELME)){
              rb.push_back(bl[i][j]);
            }
          }
          bl[i] = rb;
          if (checkTopol()){
            writeSVG("error.svg");
            tolog("Backup after keepDist\n");
            bl[i] = backup;
          }
        }
        //writeSVG("error.svg");
        //exit(0);
      }
      attachScene();
    }

    void interpolateToDist(float pointDist){
      if (pointDist <= 0){
        pointDist = 1;
      }

      for (UINT i = 0; i < bl.size(); i++){
        vector<point>tempbl;
        for(UINT j = 0; j < bl[i].size(); j++){
          point current = bl[i][j];
          point nxt = bl[i][nextPoint(i, j)];
          float d = distance(current.x, current.y, nxt.x, nxt.y);
          UINT interpoints = ceil(d / pointDist);
          if (interpoints == 0)
              interpoints = 1;
          float dx = (float)(nxt.x - current.x);
          dx = dx / interpoints;
          float dy = (float)(nxt.y - current.y);
          dy = dy / interpoints;
          //tolog(toString(__LINE__) + "\n" + "Current: " + current.croack());
          for (UINT k = 0; k < interpoints; k++){
              point tempPoint;
              tempPoint.x = current.x + (k * dx);
              tempPoint.y = current.y + (k * dy);
              tempPoint.mass = POINT_MASS;
              //tolog(toString(__LINE__) + "\n" + "Interp: " + tempPoint.croack());
              tempbl.push_back(tempPoint);
          }
          //tolog(toString(__LINE__) + "\n" + "Next: " + nxt.croack());
        }
        bl[i] = tempbl;
      }
      avgStartDist = pointDist;
      attachScene();
    }

    void interpolate(UINT npoints)
    {
        UINT i, j, k;
        float dx, dy;
        float perim;
        float segment;
        UINT interpoints;
        point startPoint;
        point endPoint;
        point tempPoint;
        vector<point> tempv;
        vector<vector<point> > tempbl;
        npoints += (UINT) bl[0].size();
        for (i = 0; i < bl.size(); i++)
        {
            perim = perimeter(bl[i], true);
            nPointsMin = (UINT) (perim / bl.size());
            startPoint = bl[i][bl[i].size()-1];
            avgStartDist = perim / npoints;
            for (j = 0; j < bl[i].size(); j++)
            {
                endPoint = bl[i][j];
                segment = distance(startPoint.x, startPoint.y,
                                   endPoint.x, endPoint.y);
                interpoints = (int)(segment / avgStartDist);
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
                    tempv.push_back(tempPoint);
                }
                startPoint = endPoint;
            }
            tempbl.push_back(tempv);
            tempv.clear();
        }
        bl.clear();
        bl = tempbl;
        initOlds();
        attachScene();
    }

    void writeSVG(string fname = ""){
        fileText tmp = toSVG();
        resetScale();
        ofstream result;
        if (fname == ""){
          fname = blSettings.fname.c_str();
        }
        result.open(fname);
        result.write(tmp.getText().c_str(), tmp.getText().size());
        result.close();

    }

    bool simulate(int maxRel = 0){
      cout << "Starting...\n";
      refreshScreen.setLimits(1,10);
      bool bQuit = false;
      setCheckTopol(false);
      float tc = 0;
      cout << "First step...\n";
      //setBestSoFar();
      while (!bQuit){
        setForcesFirstStep();
        solve();
        tc = getTotalCircleV();
        if (refreshScreen.isMax() == true) writeSVG();
        refreshScreen++;
        if (tc > 0 && tc < (1e-3*ngroups / 5)){
          bQuit = true;
        }
      }
      cout << "Second step...\n";
      bQuit = false;
      while (!bQuit){
        setForcesSecondStep();
        setContacts(false, true, 3 * maxRad() * AIR);
        solve(true);
        if (minCircDist() > (2 * maxRad()*AIR)){
          bQuit = true;
        }
        //cout << croack() << endl; exit(0);
        if (refreshScreen.isMax() == true) writeSVG();
        refreshScreen++;
      }
      bQuit = false;
      cout << "Third step (finding best geometry)...\n";
      // Compactness
      fixTopology();
      float bestOut = compactness();
      optimizationStep opt(bestOut);
      bestOut = outCompactness(&opt, &borderLine::furthestPoint, &borderLine::compactness, &borderLine::countCrossings);
      UINT outCount = 0;
      UINT maxOutCount = 10;

      while (!bQuit){
        float thisOut = outCompactness(&opt, &borderLine::furthestPoint, &borderLine::compactness, &borderLine::countCrossings);
        if (opt.hasEnded()){
          if (thisOut < bestOut || opt.hasUntied()){
            bestOut = thisOut;
            outCount = 0;
            showCrossings();
          }
          else{
            outCount++;
          }
        }
        //**********//
        fixTopology();
        //toOGL(bl, hDC);
        if (outCount > maxOutCount){
          bQuit = true;
        }
      }

      //Crossings
      resetOptimize();
      fixTopology(false);
      float bestCross = countCrossings();
      optimizationStep cropt(bestCross);
      bestCross = outCompactness(&cropt, &borderLine::furthestPoint, &borderLine::countCrossings, &borderLine::compactness);
      tolog("New bestCross: " + toString(bestCross) + "\n");
      UINT crossCount = 0;
      bQuit = false;
      while (!bQuit){
        float thisCross = outCompactness(&cropt, &borderLine::furthestPoint, &borderLine::countCrossings, &borderLine::compactness);
        if (cropt.hasEnded()){
          if (thisCross < bestCross || opt.hasUntied()){
            bestCross = thisCross;
            crossCount = 0;
            tolog("New new bestCross: " + toString(bestCross) + "\n");
          }
          else{
            crossCount++;
            tolog("-> " + toString(crossCount));
          }
        }

        fixTopology(false);
        //toOGL(bl, hDC);
        if (crossCount > maxOutCount){
          bQuit = true;
        }
      }

      if (checkTopol() == false){
        interpolateToDist(5 * minCircRadius * AIR);
        setPrevState();
        setSecureState();
      }
      else{
       // cout << "Could not fix topology. Starting again...\n");
        return false;
      }
      cout << "Fourth step (Simulation)...\n";
      bQuit = false;
      resetTimer();
      setCheckTopol(true);
      attachScene();
      scFriction(25);
      scD(1e1);
      scG(1e-1);
      scGhostGrav(false);
      while (!bQuit){
      //for (UINT i = 0; i < 10; i++){
        if (refreshScreen.isMax()) {
            writeSVG();
        }
        refreshScreen++;
        scSolve();
        bool bq = isSimulationComplete();
        if (bq){
          bQuit = true;
        }
      }
      cout << "Fifth step (Refining)...\n";
      bQuit = false;
      resetTimer();
      interpolateToDist(2 * correctedMinCircRadius());
      scSpringK(1e2);
      scFriction(70);
      scG(1e-2);
      scD(1e2);
      scGhostGrav(true);
      while (!bQuit){
        if (refreshScreen.isMax()) {
            writeSVG();
        }
        refreshScreen++;
        scSolve();
        bool bq = isSimulationComplete();
        if (bq){
          bQuit = true;
        }
      }
      return true;
    }

};

string getFile(string prompt, string errorPrompt)
{
    string fname;
    cout << prompt << endl;
    cin >> fname;
    ifstream isfile;
    isfile.open(fname.c_str());
    if (!isfile.is_open())
    {
        isfile.close();
        cout << errorPrompt << endl;
        fname = getFile(prompt, errorPrompt);
    }
    isfile.close();
    return fname;
}


//--------------------------------------------
void printv(vector<int> v)
{
    UINT i;
    for (i = 0; i < v.size(); i++)
    {
        cout << v[i];
    }
    cout << " ";
}
//--------------------------------------------


borderLine getFileInfo(string fname, string outputFile){
    ifstream vFile;
    string header;
    vector<string> groupNames;
    vector<int> temp;
    vector<float> weights;
    vector<string> labels;
    int i;
    vFile.open(fname.c_str());
    getline(vFile, header);
    cout << header;
    getline(vFile, header);
    int number = atoi(header.c_str());
    cout << endl << number << " groups:" << endl;
    for (i = 0; i < number; i++){
        getline(vFile, header);
        groupNames.insert(groupNames.end(), header);
        cout << header << endl;
    }
    int n = twoPow(number);
    for (i = 0; i < n; i++){
        getline(vFile, header); //  get the whole line
        string w = header.substr(0,header.find_first_of(" "));
        weights.insert (weights.end(), atoi(w.c_str())); // it take the first number
        string label;
        try
        {
            label = header.substr(header.find_first_of(" "));
        }
        catch (const std::exception& e)
        {
            cout << i << endl;
            label = "";
        }
        labels.insert (labels.end(), label);
        cout << "w=" << w << "  label:" << label << endl;
        //getline(vFile, header, ' '); /// get the number
        //weights.insert (weights.end(), atoi(header.c_str()));
        //getline(vFile, header) ;  ///  get the rest of the line with the labels
        //labels.insert (labels.end(), header);
        temp = toBin(i, number);
        printv(temp);
        cout << ".- " << weights[i] << " : " << labels[i] << endl;
    }
    vFile.close();
    binMap mymap(number);
    string dataFile = outputFile + ".data";
    borderLine lines(&mymap, groupNames, weights, labels, fname, outputFile);
    vFile.open(dataFile.c_str());
    if (false){ //vFile.good() == true){ // Unfinished
        vFile.close();
        lines.setCoords(dataFile);
    }


    return lines;
}



#endif // TOPOL_H_INCLUDED
