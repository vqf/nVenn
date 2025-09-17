#ifndef TOPOL_H_INCLUDED
#define TOPOL_H_INCLUDED

#include <vector>
#include <cmath>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <string>

#include "elements.h"
#include "debug.h"
#include "scene.h"
#include "strFuncts.h"
#include "palettes.h"

//#include <windows.h>

#define AIR 2    // polish and embellish multiply maxRad() by this factor

// Flags for points




std::string buildTw(std::string signat){
  std::string r;
  std::vector<std::string> rgroups;
  splitString nums(signat, ';');
  std::string nxt;
  nxt = purgeLetters(nums.next());
  r += "nVenn\n";
  nxt = purgeLetters(nums.next());
  UINT ng = atoi(nxt.c_str());
  if (ng > 0){
    r += toString(ng) + "\n";
    for (UINT j = 0; j < ng; j++){
      nxt = nums.next();
      rgroups.push_back(nxt);
      //bl.push_back({});
    }
    r += join("\n", rgroups);
    r += "0\n";
    std::vector<float> tw;
    while (!nums.finished()){
      nxt = purgeLetters(nums.next());
      float n = stof(nxt);
      tw.push_back(n);
    }
    r += join("\n", tw);
  }
  return r;
}




//--------------------------------------------
void printv(std::vector<UINT> v)
{
    UINT i;
    for (i = 0; i < v.size(); i++)
    {
        std::cout << v[i] << ", ";
    }
    std::cout << std::endl;
}

void vlog(std::vector<UINT> v){
  if (v.size() > 0){
    for (UINT i = 0; i < (v.size() - 1); i++){
      tolog(toString(v[i]) + ", ");
    }
    tolog(toString(v[v.size() - 1]) + "\n");
  }
  else{
    tolog("\n");
  }
}
//--------------------------------------------

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
 * \param a std::vector<float>
 * \param b std::vector<float>
 * \return float
 *
 */
float sprod(std::vector<float> a, std::vector<float> b)
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

std::vector<float> arr2vec(float arr[], int n)
{
    int i;
    std::vector<float> result;
    for (i = 0; i < n; i++)
    {
        result.push_back(arr[i]);
    }
    return result;
}


/** \brief Converts a number into a std::vector of integers
 * with the binary representation of the number
 *
 * \param number int
 * \param 0 UINT nBits=
 * \return std::vector<int>
 *
 */
std::vector<int> toBin(int number, UINT nBits = 0)
{
    //  Takes an integer and returns a std::vector containing
    //  its binary representation

    UINT i;                          // iterate over each bit
    int bit;                        // temp storage of each bit
    std::vector<int> bits;               // return vector
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



/** \brief Takes a std::vector containing the binary representation
 * of an integer and returns that integer
 *
 * \param v std::vector<int>
 * \return int
 *
 */
int toInt(std::vector<int> v)
{
    // Takes a std::vector containing the binary representation
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


enum crossResult{crosses, doesnotcross, cont};
enum steps{
  noaction,
  attract,
  disperse,
  minimizeCompactness,
  minimizeCrossings,
  contract,
  refineCircles,
  embellishLines
};

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
  bool optimize;
  bool part;
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
  std::string cycleInfo; /**< Debuggin info from last cycle */
  UINT cyclesForStability; /**< After this number of cycles without improvement, finish sim */
  float lineAir; /**< Added to radii so that there is room for lines between circles */
  UINT contactFunction;
  int checkFor; // If any of the previous or following 10 point is sticking to
                           // the surface, the current point will also stick
  std::string inputFile;
  std::string fname;
} blData;




class borderLine;



class groupIterator{
  //circleIterator ci;
  std::vector<UINT> translator;
  std::vector<point> circles;
  UINT mask;
  UINT first;
  UINT cval;
  UINT ival;
  bool finish;

  void init(UINT v){
    cval = v;
    first = v;
    ival = 0;
    setVal(v);
    first = val();
    finish = false;
    if (translator.size() < 1){
      finish = true;
    }
  }
public:
  groupIterator(std::vector<point> circs, UINT group, UINT starting = 0, UINT msk = 0) {
    if (msk > 0){
      mask = msk;
    }
    else{
      mask = 1 << group;
    }
    circles.clear();
    for (UINT i = 0; i < circs.size(); i++){
      if (((circs[i].n & mask) > 0) && circs[i].radius > 0){
        circles.push_back(circs[i]);
        translator.push_back(i);
      }
    }
    init(starting);
  }
  void reset(UINT v){
    init(v);
  }
  void setVal(UINT v = 0){
    UINT i = 0;
    bool found = false;
    if (translator.size() > 0){
      while (!found && translator[i] < v){
        i++;
        if (i >= circles.size()){
          i = 0;
          found = true;
        }
      }
      ival = i;
      cval = translator[i];
      if (cval == first){
        finish = true;
      }
    }
    else{
      cval = 0;
    }
  }
  UINT val(){
    return cval;
  }
  std::string croak(){
    std::ostringstream r;
    r << "Mask: " << mask << std::endl;
    for (UINT i = 0; i < circles.size(); i++){
      r << circles[i].croack();
    }
    return r.str();
  }
  bool isFinished(){
    return finish;
  }
  UINT nxt(){
    ival++;
    if (ival >= circles.size()){
      ival = 0;
    }
    UINT v = translator[ival];
    setVal(v);
    return cval;
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
    init(dx, dy);
  }
  void init(float dx, float dy){
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
  void init(point p1, point p2){
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    slope = 0;
    err = false;
    init(dx, dy);
    if (slope != slope){
      tolog(_L_ + "Incorrect tangent: \n" + p1.croack() + p2.croack());
      err = true;
    }
  }
  tangent(){}
  tangent(point p1, point p2){
    init(p1, p2);
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
  std::string croack() {
    std::ostringstream r;
    r << "Slope: " << slope << "\tQuadrant: " << quadrant << std::endl;
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
  std::string croack(){
    return t.croack();
  }
};

class timeMaster{
  std::vector<float> times;
  std::vector<UINT> reports;
  UINT icdt;
  UINT unstableCounter;
  float imindt;
  float imaxdt;
  float istepdt;
  public:
  timeMaster(){}
  void init(float mindt = 1e-4, float maxdt = 0.02, float stepdt = 0.5){
    times.clear();
    reports.clear();
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
  void setDt(UINT level){
    if (level < times.size()){
      reset();
      icdt = level;
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
      x = std::min(p1.x, p2.x);
      X = std::max(p1.x, p2.x);
      y = std::min(p1.y, p2.y);
      Y = std::max(p1.y, p2.y);
    }

    void setScale(scale tocopy){
      point p1;
      p1.x = tocopy.minX();
      p1.y = tocopy.minY();
      point p2;
      p2.x = tocopy.maxX();
      p2.y = tocopy.maxY();
      scale(p1, p2);
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
    float minSpan(){
      float minCoord = xSpan();
      if (ySpan() < minCoord) minCoord = ySpan();
      return minCoord;
    }
    float ratio()
    {
        float result;
        if (xSpan() == 0)
            return 1.0f;
        result = ySpan() / xSpan();
        return result;
    }
    std::string croack(){
      std::string result;
      result += "MinX: " + toString(minX()) + "\n";
      result += "MaxX: " + toString(maxX()) + "\n";
      result += "MinY: " + toString(minY()) + "\n";
      result += "MaxY: " + toString(maxY()) + "\n";
      return result;
    }
};



template<typename T>
void _D_(std::vector<T> v, std::string sep=""){
  std::string r;
  for (UINT i = 0; i < v.size() - 1; i++){
    r += toString((UINT) v[i]) + sep;
  }
  r += v[v.size()-1];
  //tolog(toString(__LINE__) + "\n" + r);
}


float perimeter(std::vector<point> v, bool close = false)
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
  optimizationStep(){}
  optimizationStep(float comp){
    init(comp);
  }
  void init(float comp){
    bestComp = comp;
    counter = 0;
    candidate = 0;
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
    std::vector<std::vector<int> > column;
    std::vector<std::vector<std::vector<int> > > row;

    signed int firstUnmatched1(std::vector<int> v)
    {
        /***********************************************************

            Returns an integer with the position of the first
            unmatched 1. If none is found, the function
            returns -1.

            @v is the input std::vector to be searched for unmatched 1s

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

    signed int firstUnmatched0(std::vector<int> v)
    {
        /***********************************************************

            Returns an integer with the position of the first
            unmatched 0. If none is found, the function
            returns -1.

            @v is the input std::vector to be searched for unmatched 0s

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
    std::vector<std::vector<int> > getBranches(std::vector<int> v)
    {
        /***********************************************************

            Returns a std::vector containing all of the branches derived
            from the input vector. These are obtained by changing to
            1 each 0 to the right of the last 1 if the resulting
            std::vector has no unmatched 1s. If there are no branches,
            the return std::vector is empty.

            @v input binary vector

        ***********************************************************/

        UINT i;
        int last1=0;
        std::vector<int> tempV;
        std::vector<std::vector<int> > result;
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

    void fillColumn(std::vector<int> v)
    {
        std::vector<int> result;
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

    void fillRow(std::vector<std::vector<int> > c)
    {
        UINT i;
        std::vector<std::vector<int> > tempc;
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
        std::vector<int> currentV;
        std::vector<std::vector<int> > currentC;
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
                    std::cout << row[i][j][k];
                }
                std::cout << " ";
            }
            std::cout << std::endl;
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
  UINT ncycles;
  UINT mincycles;
  float totalCompactness;
  float lastCompactness;
  float bestUntie;
  float wRoom;
  bool keep;
public:
  void setConstants(UINT numberForAverage, UINT numberForStability, float wiggleRoom = 1, UINT minNumberOfCycles = 0){
    finish = false;
    canFinish = false;
    cyclesWithoutImprovement = 0;
    nAvg = numberForAverage;
    finishAfter = numberForStability;
    counter = 0;
    totalCompactness = 0;
    first = true;
    keep = false;
    wRoom = wiggleRoom;
    mincycles = minNumberOfCycles;
  }
  void init(){
    finish = false;
    canFinish = false;
    first = true;
    cyclesWithoutImprovement = 0;
    counter = 0;
    ncycles = 0;
    totalCompactness = 0;
    lastCompactness = 0;
  }
  void setCanFinish(){
    canFinish = true;
    if (finishAfter == 0){
      finish = true;
    }
  }
  bool keepState(){
    return keep;
  }
  void add(float comp){
    if (comp == 0){
        finish = true;
    }
    keep = false;
    ncycles++;
    if (first){
      first = false;
      counter++;
      bestCompactness = comp;
      lastCompactness = comp;
    }
    if (canFinish){
      if (finishAfter == 0){
        finish = true;
      }
      if (comp < (bestCompactness)){
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
        float newAvg = totalCompactness / ((float)(nAvg));
        if (newAvg > (lastCompactness / wRoom) && ncycles > mincycles){
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
  std::vector<point> bestCircles;
  std::vector<std::vector<point> > bestBl; /**< Best result */
  std::vector<std::vector<point> > bl_secure; /**< For changes in the number of points */
  std::vector<std::vector<point> > bl_old10;
  std::vector<point> circles_secure;
  std::vector<point> circles_old10;
  float simTime;
  bool hasBeenSet;
} blState;

typedef struct outc{
  UINT outCount;
  UINT maxOutCount;
  float optVal;
} outCounters;

typedef struct csts{
  float K;  // Spring constant
  float G;  // Gravitational constant
  float D;  // Spring damping constant
  float B;  // Newtonian resistance coefficient
} constants;

typedef struct svgOpts{
    std::vector<std::string> svgColors;
    float svgOpacity;
    float svgLineWidth;
    bool showNumbers;
    bool showRegionNumbers;
    UINT svgFontSize;
} svgOptions;

class borderLine
{
    friend class glGraphics;
    friend class groupIterator;
    friend void toOGL();


    nvenn setElements;
    scene tosolve;
    float wmax;
    float cushion; /* Distance between lines */
    bool error;
    bool fromSignature;
    bool resetV;
    float lastV;
    UINT seed;
    std::string errorMessage;
    std::string signature;
    std::vector<UINT> sceneTranslator;
    blState savedState;
    float simulationTime;
    float maxLineVsq;
    float maxCircleVsq;
    constants scConstants;
    UINT internalCounter;
    UINT currentStep;
    UINT doublings;
    UINT maxdoublings;
    outCounters oc;
    optimizationStep optStep;
    float (borderLine::*currentMeasure)();

    std::vector<float> pairDistances;
    std::vector<std::string> groups;
    std::vector<point> p;
    std::vector<std::vector<point> > bl; /**< Vector of lines. Each line is a vector of points */
    std::vector<std::vector<point> > blCross; /**< Vector of lines. Keeps unembellished lines */
    std::vector<point> circles;
    std::vector<point> debug;
    std::vector<UINT> relationships;
    UINT ncycles_secure;
    UINT ncyles_old10;
    UINT ngroups;
    UINT startPerim;
    float minCircRadius; /**< Minimum circ radius, to calc nPointsMin */
    float maxRadius;   /**< Radius of the largest circle */
    UINT nPointsMin;  /**< Minimum number of points per line */
    float avgStartDist; /**< Average distance between points at start */
    float potential; /**< Potential energy, some parameter to minimize */
    decider evaluation;
    lCounter blCounter;
    lCounter deciderCounter;
    lCounter refreshScreen;
    lCounter keepDistCounter;
    std::vector<float> circRadii;
    std::vector<float> w;
    std::vector<std::string> labels;
    std::vector<std::vector<UINT>> origw;
    std::vector<rgb> colors;
    palettes svgPalettes;
    svgOptions svgParams;
    std::vector<point> warn;
    scale internalScale;
    scale limits;
    scale svgScale;
    int totalExpectedSurface;
    std::vector<std::string> dataDisplay;
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
      b->doCheckTopol = false;
      b->fixCircles = false;
      b->signalEnd = false;
      b->smoothSVG = false;
      b->part = false;
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

    void init(){
      error = false;
      resetV = false;
      cushion = 0.02;
      doublings = 0;
      maxdoublings = 2;
      svgParams.svgOpacity = 0.4;
      svgParams.svgLineWidth = 1;
      svgParams.showNumbers = true;
      svgParams.showRegionNumbers = true;
      svgParams.svgFontSize = 10;
      palettes svgPalettes;
      errorMessage = "";
      minCircRadius = 1.0f;
      nPointsMin = 10;
      potential = 0;
      maxRadius = 0;
      scConstants.B = 0;
      scConstants.D = 0;
      scConstants.G = 0;
      scConstants.K = 0;
      internalScale.initScale();
      svgScale.initScale();
      svgScale.setMinX(20.0f);
      svgScale.setMinY(20.0f);
      svgScale.setMaxX(480.0f);
      svgScale.setMaxY(480.0f);
      initBlData(&blSettings);
      minRat = 0;
      showThis = false;
      blSettings.signalEnd = false;
      blSettings.contacts = 0;
      blSettings.fixCircles = false;
      blSettings.totalCircleV = 0;
      blSettings.totalLineV   = 0;
      blSettings.minSurfRatio = 0;
      blSettings.maxf = 5e1f;
      blSettings.maxv = 5e1f;
      blSettings.softcontact = false;
      blSettings.maxvcontact = 50;
      blSettings.startdt = blSettings.dt;
      blSettings.stepdt = 0.04f;
      blSettings.ncycles = 0;
      blSettings.cycleInfo = "";
      blSettings.optimize = true;
      simulationTime = 0;
      maxLineVsq = 0;
      maxCircleVsq = 0;
      oc.maxOutCount = 70;
      oc.outCount = 0;
      oc.optVal = 0;
      borderLine::currentMeasure = &borderLine::compactness;
      // Set the width of a line
      point labs(1 / svgScale.xSpan(), 0);
      point lsvg = place(internalScale, labs);
      blSettings.marginScale = lsvg.x;
      blSettings.margin = 1.2 * ngroups * blSettings.marginScale;
      /**/
    //init counters
      initCounters();

      //init internal scale
      internalScale.setClear(true);

      //init time parameters
      udt.init();
      evaluation.init();
      evaluation.setConstants(100, 50);
      //Set starting palette and load svg and postscript colors
      loadPalette(0);
    }


    void init(std::vector<std::string> g, std::vector<std::vector<UINT>> tw, std::string inputFile = "venn.txt", std::string outputFile = "result.svg"){
      UINT i;
      setElements = nvenn();
      origw.clear();
      circRadii.clear();
      groups = g;
      currentStep = attract;
      ngroups = g.size();
      blSettings.inputFile = inputFile;
      blSettings.fname = outputFile;
      blSettings.minratio = 0.1f * (ngroups * ngroups * ngroups)/ (4 * 4 * 4);
      blSettings.lineAir = ngroups;
      /**/
      init();
      for (UINT i = 0; i < tw.size(); i++){
        w.push_back(tw[i][1]);
      }
      wlimit();
      for (i = 0; i < tw.size(); i++){
        origw.push_back(tw[i]);
      }
      //init circles
      setCircles(origw);
      setRelationships(); /* How many bits does each pair of circles share */


      std::ostringstream l;
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

      //totalExpectedSurface = 0;
      //for (i = 0; i < w.size(); i++){
      //  totalExpectedSurface += (int) (circles[i].radius * circles[i].radius);
      //}



      //init points
      for (i = 0; i < ngroups; i++){
      //{
      //    p.clear();
          //setPoints(i);
          bl.push_back({});
      }
      addLines();
      startPerim = (UINT) perimeter(bl[0]);
      //UINT np = (UINT) (0.5f * (float) startPerim);
      //interpolate(np);

      setPrevState();
      setSecureState();
      savedState.hasBeenSet = false;
      randomizeCircles();
    }


    UINT leftmostCircle(UINT group){
      groupIterator git(circles, group, 0);
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

    std::string float2string(float f){
        std::string result = vformat("%g", f);
        return result;
    }

    std::string UINT2string(UINT f){
        std::string result = vformat("%u", f);
        return result;
    }

    std::string bool2string(bool f){
        std::string result = f ? "1" : "0";
        return result;
    }

    void displayFloat(std::string label, float d){
        std::string dsp = vformat("%s: %g", label.c_str(), d);
        dataDisplay.push_back(dsp);
    }

    void displayUINT(std::string label, UINT d){
        std::string dsp = vformat("%s: %u", label.c_str(), d);
        dataDisplay.push_back(dsp);
    }




    void initOlds()
    {
        savedState.bl_old10 = bl;
        ncyles_old10 = blSettings.ncycles;
        savedState.circles_old10 = circles;
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


    void setCircles(std::vector<std::vector<UINT>> weights){
        UINT maxw = 0;
        for (UINT i = 0; i < weights.size(); i++){
          UINT wgt = weights[i][1];
          if (wgt > maxw){
            maxw = wgt;
          }
        }
        for (UINT i = 0; i < weights.size(); i++){
          UINT reg = weights[i][0];
          UINT sz  = weights[i][1];
          float r = 2 * sqrt(((float) sz) / ((float) maxw));
          point cpoint;
          cpoint.n = reg;
          cpoint.radius = 2 * sqrt(w[i] / maxw);;
          cpoint.setCustom(i);
          circRadii.push_back(r);
          cpoint.orig = sz;
          circles.push_back(cpoint);
        }
        for (UINT j = 0; j < circRadii.size(); j++){
          if (circRadii[j] > 0 && circRadii[j] < minCircRadius) minCircRadius = circRadii[j];
          if (circRadii[j] > 0 && circRadii[j] > maxRadius) maxRadius = circRadii[j];
        }
    }

/*
    void setCirclesOld(binMap b, std::vector<float> o, std::vector<std::string> tlabels)
    {
        UINT i, j;
        int n;
        int height;
        int offset;
        float maxw = 0;
        point cpoint;
        height = b.row[0].size();
        for (i = 0; i < w.size(); i++)
        {
            if (w[i] > maxw)
                maxw = w[i];
        }
        for (i = 0; i < b.row.size(); i++)
        {
            offset = (height - b.row[i].size()) / 2;
            for (j = 0; j < b.row[i].scirclessize(); j++)
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
        //circles[0].radius = 0;
    }*/

    /** \brief Gets the coordinates of circles from setCircles() and sets them
     *         on a grid pseudorandomly
     *
     * \return void
     *
     */
    void randomizeCircles(){
      UINT gstep = 1;
      UINT gridSize = (UINT) (sqrt(nregions()) + 2);
      if ((ngroups & 1) > 0){ // Odd number of groups
          gridSize = gridSize << 1;
          gstep = 2;
      }

      //std::cout << "gridSize: " << gridSize << "\n";
      float xstep = internalScale.xSpan() / ((gridSize + 1));
      float ystep = xstep; //internalScale.ySpan() / (gridSize + 1);
      //std::cout << "xStep: " << internalScale.xSpan() << "\n";
      //std::cout << "yStep: " << ystep << "\n";
      std::vector<UINT> order;
      for (UINT i = 0; i < circles.size(); i++){
        order.push_back(i);
      }
      std::random_device rd;
      if (!fromSignature) seed = rd();
      std::mt19937 g(seed);
      shuffle(order.begin(), order.end(), g);
      UINT sy = 0; UINT sx = 0;
      for (UINT i = 0; i < 2; i++){
          float cx = (sx + 0.5) * xstep + internalScale.minX();
          float cy = (sy + 0.5) * ystep + internalScale.minY();
          sx += 1;
          sy += 1;
          circles[order[i]].x = cx;
          circles[order[i]].y = cy;
      }
      for (UINT i = 3; i < circles.size(); i++){
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

/*
    void setPoints(UINT ngroup)
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
    }*/

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
        UINT n1 = p0.custom;
        UINT n2 = p1.custom;
        float factor = (float) getRelationships(n1, n2) + 1;
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
          std::vector<int> belong = toBin(circles[j].n, bl.size());
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


    void embellishTopology(float rad = 0, bool logit = false){
      tangent lft(0, -1);
      tangent rgh(0, 1);
      for (UINT k = 0; k < circles.size(); k++){
        circles[k].flags = unsetFlag(circles[k].flags, USED);
      }
      for (UINT i = 0; i < bl.size(); i++){
        bool again = true;
        while (again){
          again = false;
          std::vector<point> newbl;
          std::vector<point> newblAlt;
          for (UINT j = 0; j < bl[i].size(); j++){
            if (!again){ // sanity check
              point nxt = bl[i][nextPoint(i, j)];
              point current = bl[i][j];
              newbl.push_back(current);
              newblAlt.push_back(current);
              float dsq3 = sqDistance(current, nxt);
              if (dsq3 > 0){
                float sq3 = sqrt(dsq3);
                UINT fcirc = 0;
                for (UINT k = fcirc; k < circles.size(); k++){
                  point c = circles[k];
                  if (c.radius > 0){
                    float lrad = rad;
                    if (lrad == 0){
                      lrad = maxRad();
                    }
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
                          c.flags = setFlag(c.flags, USED);
                          point pst1 = tp.transformPoint(current, x1);
                          pst1.flags = setFlag(pst1.flags, DO_NOT_EMBELLISH);
                          pst1.flags = setFlag(pst1.flags, DELME);
                          pst1.n = k;
                          point pst2 = tp.transformPoint(current, d1 + t) ;
                          pst2.flags = setFlag(pst2.flags, DO_NOT_EMBELLISH);
                          pst2.flags = setFlag(pst2.flags, DELME);
                          pst2.n = k;
                          newbl.push_back(pst1);
                          newblAlt.push_back(pst1);
                          // Option 1
                          tangent tr = tp + rgh;
                          point newp = tr.transformPoint(c, lrad);
                          newp.flags = setFlag(newp.flags, DO_NOT_EMBELLISH);
                          newp.flags = setFlag(newp.flags, DELME);
                          newp.n = k;
                          //Option 2
                          tr = tp + lft;
                          point newpa = tr.transformPoint(c, lrad);
                          newpa.flags = setFlag(newpa.flags, DO_NOT_EMBELLISH);
                          newpa.flags = setFlag(newpa.flags, DELME);
                          newpa.n = k;

                          if (inside){
                            newbl.push_back(newpa);
                            newblAlt.push_back(newp);
                          }
                          else{
                            newbl.push_back(newp);
                            newblAlt.push_back(newpa);
                          }

                          newbl.push_back(pst2);
                          newblAlt.push_back(pst2);
                          // Close the bl for the next cycle
                          for (UINT l = j+1; l < bl[i].size(); l++){
                            newbl.push_back(bl[i][l]);
                            newblAlt.push_back(bl[i][l]);
                          }
                          bl[i].clear();
                          bl[i] = newbl;
                          UINT cn = c.n;
                          std::vector<int> belong = toBin(cn, bl.size());
                          bool incorrect = circleTopol(c, belong, i);
                          if (incorrect){
                            //writeSVG("/home/vqf/proyectos/nVenn2/error.svg");
                            bl[i].clear();
                            bl[i] = newblAlt;
                            //std::cout << c.n << "\t" << i + 1 << std::endl;
                            //std::cout << join(", ", belong) << std::endl;
                            //writeSVG("/home/vqf/proyectos/nVenn2/other.svg"); exit(0);
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          //bl[i].clear();
          //bl[i] = newbl;
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
    void fixTopology(float rad = 0, bool logit = false){
      addLines();
      polishLines(rad);
      //embellishTopology();
      //writeSVG("embellish.svg");
      //exit(0);
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
              std::vector<int> belong = toBin(p3n, bl.size());
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
                  std::vector<point> newpoints;
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
      embellishTopology(rad, logit);
      //writeSVG("embellish.svg");
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
      //writeSVG("polishlines.svg");
      //embellishTopology();
      //writeSVG("embellish.svg");
      for (UINT k = 0; k < bl.size(); k++){
        bool goon = true;
        while (goon){
          goon = false;
          for (UINT j = 0; j < circles.size(); j++){
            if (circles[j].radius > 0 && ((circles[j].flags & USED) == 0)){
              circles[j].flags = setFlag(circles[j].flags, USED);
              point p3 = circles[j];
              std::vector<int> belong = toBin(p3.n, bl.size());
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
                  std::vector<point> newpoints;
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
                  error = true;
                }
                incorrect = circleTopol(circles[j], belong, k); //Was it fixed?
                if (incorrect){
                  writeSVG("error.svg");
                  tolog(_L_ + "Could not fix circle " + toString(circles[j].n) + " with line " + toString(k) + ".\n");
                  error = true;
                  errorMessage = "Could not fix circle " + toString(circles[j].n) + " with line " + toString(k) + ".\n";
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

    float getArea(){
      float area = 0;
      for (UINT i = 0; i < bl.size(); i++){
        for (UINT j = 0; j < bl[i].size(); j++){
          point p1 = bl[i][j];
          UINT np2 = nextPoint(i, j);
          point p2 = bl[i][np2];
          float dx = p2.x - p1.x;
          float dy = p2.y - p1.y;
          area += p1.y * dx + dx * dy / 2;
        }
      }
      return area;
    }

    float getMaxDsq(){
      return tosolve.getMaxDsq();
    }
    float getMaxFsq(){
      return tosolve.getMaxFsq();
    }

    float getMaxVsq(){
      return tosolve.getMaxVsq();
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
      groupIterator git(circles, ngroup, n2);
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

    float getEmbellishDist(float rf = 0.2){
      float mc = correctedMinCircRadius();
      float result = rf * minCircRadius * 2;
      float minRat = rf * mc * 2;
      if (result < minRat){
        result = minRat;
        tolog("Mc: " + toString(mc) + ", minc: " + toString(minCircRadius) + ", result: " + toString(result) + "\n");
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

      for (UINT i = 0; i < ngroups; i++){
        bl[i].clear();
        //tolog("Val: " + toString(i) + "\n");
        UINT lm = leftmostCircle(i);
        //tolog(toString(circles[lm].n) + "\n");
        //if (lm > 0){
          point toadd = circles[lm];
          toadd.n = lm;
          bl[i].push_back(toadd);
          UINT an = lm;
          UINT st = lm;
          UINT np = nextLeftmostPoint(i, an, st);
          //tolog(" - "+toString(circles[np].n) + "\n");
          //std::cout << circles[lm].n << "\t" << circles[np].n << std::endl;
          //std::cout << circles[lm].radius << "\t" << circles[np].radius << std::endl;
          //if (lm > 0){
            while (np != lm){
              point toadd = circles[np];
              toadd.n = np;
              bl[i].push_back(toadd);
              an = st;
              st = np;
              np = nextLeftmostPoint(i, an, st);
              if (np == toadd.n){
                std::cout << "Error in group " << i << std::endl;
                std::cout << "The set is empty\n";
                tolog("Error in group " + toString(i) + "\nThe set is empty\n");
                error = true;
                errorMessage = "Error in group " + toString(i) + "\nThe set is empty\n";
              }
              //tolog(toString(circles[np].n) + "\n");
            }
          //}
        //}
      }
    }


    /** \brief Take the lines generated with @addLines and set the points to the
     *         outside of each circle.
     *
     * \return void
     *
     */
    void polishLines(float rad = 0){
      tangent rev(1, 0);
      for (UINT i = 0; i < bl.size(); i++){
        UINT sz = bl[i].size();
        std::vector<point> newpoints;
        if (sz < 2){
          tangent dwn(0, -1);
          tangent lft(-1, 0);
          tangent up(0, 1);
          point current = bl[i][0];
          UINT n = current.n;
          float lrad = rad;
          if (rad == 0){
            lrad = current.radius;
          }
          point p1 = rev.transformPoint(current, lrad);
          p1.flags = setFlag(p1.flags, DO_NOT_EMBELLISH);
          p1.n = n;
          point p2 = dwn.transformPoint(current, lrad);
          p2.flags = setFlag(p2.flags, DO_NOT_EMBELLISH);
          p2.n = n;
          point p3 = lft.transformPoint(current, lrad);
          p3.flags = setFlag(p3.flags, DO_NOT_EMBELLISH);
          p3.n = n;
          point p4 = up.transformPoint(current, lrad);
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
                float r = rad;
                if (rad == 0){
                  r = maxRadius;// current.radius;
                }
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
      fixTopology();
    }


    std::vector<point> getSetBoundaries(UINT groups, float air = 0, bool onlyCircles = false){
      point ul;
      point lr;
      bool fst = true;
      std::vector<point> result;
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
        for (UINT k = 0; k < bl.size(); k++){
          if (k & groups){
            for (UINT j = 0; j < bl[k].size(); j++){
              if (bl[k][j].x < result[0].x){
                result[0].x = bl[k][j].x;
              }
              if (bl[k][j].y < result[0].y){
                result[0].y = bl[k][j].y;
              }
              if (bl[k][j].x > result[1].x){
                result[1].x = bl[k][j].x;
              }
              if (bl[k][j].y > result[1].y){
                result[1].y = bl[k][j].y;
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
    std::vector<point> getBoundaries(float air = 0, bool onlyCircles = false){
      UINT n = twoPow(ngroups) - 1;
      std::vector<point> result = getSetBoundaries(n, air, onlyCircles);
      return result;
    }

    std::vector<point> getOutsidePoints(bool logit = false){
      std::vector<point> p = getBoundaries(2 * maxRad());
      std::vector<point> q;
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
      std::vector<point> backup = circles;
      std::vector<point> best   = circles;
      addLines();
      polishLines();
      UINT bestout = countOutsiders();
      UINT bestcrs = countCrossings();
      if (logit){
        tolog(_L_ +  "Starting with " + toString(bestout) + " outsiders.\n");
      }
      for (UINT i = 0; i < circles.size() - 1; i++){
        if (circles[i].radius > 0){
          //std::vector<int> b = toBin(circles[i].n, twoPow(ngroups));
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


    std::vector<UINT> sampleUINT(std::vector<UINT> from, UINT n, std::mt19937 *gen){
      UINT nc = from.size() - 1;
      std::vector<UINT> cp = from;
      std::vector<UINT> result;
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
      std::vector<UINT> existingCircles = ncircles();
      UINT counter = 0;
      while (cyclesWithoutImprovement < 1000 && counter < maxSteps){
        std::vector<UINT> exch = sampleUINT(existingCircles, nstep, &gen);
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
      std::vector<UINT> existingCircles = ncircles();
      UINT counter = 0;
      while (cyclesWithoutImprovement < 500 && counter < maxSteps){
        std::vector<UINT> exch = sampleUINT(existingCircles, nstep, &gen);
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
      bool resetOpt = true;
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          float val = 0;
          if ((circles[i].flags & DO_NOT_OPTIMIZE) == 0){
            resetOpt = false;
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
      if (resetOpt){
        resetOptimize();
      }
      circles[result].flags = setFlag(circles[result].flags, DO_NOT_OPTIMIZE);
      return result;
    }

    /** \brief Gets (one of) the circle that belongs to the least number of sets.
     * Must have radius > 0. Never returns the same circle twice
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
      bool resetOpt = true;
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0 && !(circles[i].flags & DO_NOT_OPTIMIZE)){
          UINT val = ones(circles[i].n & mask);
          resetOpt = false;
          //tolog(toString(circles[i].n) + ": " + toString(val) + "\n");
          if (first || val < crossest){
            result = i;
            crossest = val;
            first = false;
          }
        }
      }
      if (resetOpt){
        resetOptimize();
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
      //float startComp = (this->*countFunct)();
      //opt->setBestCompactness(startComp);
      displayFloat("BEST", opt->getBestCompactness());
      if (opt->hasEnded()){
        opt->startCycle();
        opt->setCandidate((this->*chooseCandidate)());
        //UINT n = opt->getCandidate();
        tolog("Next candidate: " + toString(circles[n].n) + "\n");
        //if (n == 0){
        //  opt->endCycle();
        //  resetOptimize();
        //  opt->startCycle();
        //}
        //std::cout << circles[n].n << std::endl;
      }
      else{
        UINT candidate = opt->getCandidate();
        displayUINT("CANDIDATE", candidate);
        //if (candidate > 0){
          UINT i = opt->getCounter();
          debug.push_back(circles[i]);
          debug.push_back(circles[candidate]);
          if (i != candidate && circles[i].radius > 0){
            swapCoords(i, candidate);
            if (checkTopol()){
              tolog("Undo on checkTopol\n");
              swapCoords(i, candidate);
            }
            else{
              float newComp = (this->*countFunct)();
              if (newComp < opt->getBestCompactness()){
                opt->setBestCompactness(newComp);
                tolog("Swapping " + toString(i) + " with " + toString(candidate) + " -> " + toString(newComp) + "\n");
                //std::cout << "Swapping " << i << " with " << candidate << " -> " << newComp << std::endl;
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
        //}
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
          std::vector<point> q = getOutsidePoints();
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
        std::ofstream result;
        std::string outputFigData = blSettings.fname + ".data";
        std::string datafile = saveBl();
        result.open(outputFigData.c_str());
        result.write(datafile.c_str(), datafile.size());
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

    bool sensible(){
      bool result = true;
      float minext = cushion * (ngroups + 1);
      for (UINT i = 0; i < (circles.size() - 1); i++){
        if (circles[i].radius > 0){
          for (UINT j = (i+1); j < circles.size(); j++){
            if (circles[j].radius > 0){
              float r0 = circles[i].radius;
              float r1 = circles[j].radius;
              float rd = distance(circles[i].x, circles[i].y, circles[j].x, circles[j].y);
              if (rd < (r0 + r1 + minext)){
                result = false;
              }
            }
          }
        }
      }
      return result;
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
        std::string temp;
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

        blSettings.totalCircleV = 0;
        blSettings.totalLineV = 0;
        maxLineVsq = 0;
        maxCircleVsq = 0;

        //Init the scale for the new frame
        internalScale.setClear(true);
        updPos(kb, resetVelocity);
        clearForces();
//Show dt
        //displayFloat("SIMTIME", simulationTime);
        displayFloat("DT", blSettings.dt);
        //displayUINT("CYCLES", blSettings.ncycles);
        //displayUINT("NPOINTS", bl[0].size());
        //displayFloat("POTENTIAL", std::log10(potential));

        potential = 0;

        /*for (UINT j = 0; j < circles.size(); j++){
          circles[j].flags = unsetFlag(circles[j].flags, IS_OUTSIDE);
        }*/
        if (blSettings.doCheckTopol == true && (checkTopol()))
        {
          if (breakOnTopol){
              tolog(_L_ + "Break on topol\n");
              writeSVG("error.svg");
              for (UINT j = 0; j < circles.size(); j++){
                if (circles[j].radius > 0){
                  point P = circles[j];
                  std::vector<int> b = toBin(circles[j].n, bl.size());
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
              interpolateToDist(minCircRadius / 4);
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
                  float cvelsq = bl[i][j].vx * bl[i][j].vx + bl[i][j].vy * bl[i][j].vy;
                  blSettings.totalLineV += cvelsq;
                  if (cvelsq > maxLineVsq){
                    maxLineVsq = cvelsq;
                  }

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
                error = true;
                errorMessage = "Bad circle: " + circles[i].croack();
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
              float cvelsq = circles[i].vx * circles[i].vx + circles[i].vy * circles[i].vy;
              blSettings.totalCircleV += cvelsq;
              if (cvelsq > maxCircleVsq){
                maxCircleVsq = cvelsq;
              }
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
        //displayFloat("MAXLINEV", log(maxLineVsq));
        //displayFloat("MINRAT", minRat);
        displayFloat("MAXCIRCLEV", std::log10(maxCircleVsq));
        //displayFloat("SURFRATIO", estSurf());
    }

    float estSurf(int nPoints = 100){
      float tsurf = (float) (internalScale.xSpan() * internalScale.ySpan());
      tsurf /= (float) totalExpectedSurface;
      return tsurf;
    }

    bool circleTopol(point P, std::vector<int> belong, UINT j){
      bool mustBeIn = (belong[j] == 1);  //Must the circle be inside the curve?
      bool isIn = false;      //Is the circle inside the curve?
      std::vector<point> bnd = getBoundaries();
      float xmax = std::max(bnd[0].x, bnd[1].x) + 2 * maxRad() +  1;
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
        //std::cout << i << "\t" << j << "\t" << np << std::endl;
        while (cr == cont){
          cr = cross(p1, p2, p3, p4);
          np = nextPoint(j, np);
          p4 = bl[j][np];
          //std::cout << "--" <<  i << "\t" << j << "\t" << np << std::endl;
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


    bool isTopolIncorrect(point P, std::vector<int> belong){
      if (!(P.radius > 0)){
        tolog(_L_ + "Called isTopoloCorrect with radius 0\n");
        error = true;
        errorMessage = "Called isTopoloCorrect with radius 0\n";
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
              std::vector<int> belong = toBin(circles[i].n, bl.size());
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
      if (std::max(p1.x, p2.x) >= std::min(p3.x, p4.x) &&
          std::max(p1.y, p2.y) >= std::min(p3.y, p4.y) &&
          std::min(p1.x, p2.x) <= std::max(p3.x, p4.x) &&
          std::min(p1.y, p2.y) <= std::max(p3.y, p4.y)){
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
      if (std::max(p1.x, p2.x) >= std::min(p3.x, p4.x) &&
          std::max(p1.y, p2.y) >= std::min(p3.y, p4.y) &&
          std::min(p1.x, p2.x) <= std::max(p3.x, p4.x) &&
          std::min(p1.y, p2.y) <= std::max(p3.y, p4.y)){
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
      std::vector<std::vector<point>> useme = bl;
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
                UINT refn = np;
                do{
                  p4 = useme[j][np];
                  cr = cross(p1, p2, p3, p4);
                  np =  0;
                  if (jj < (useme[j].size() - 1)){
                    np = jj + 1;
                  }
                } while (cr == cont && np != refn);
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
      std::vector<std::vector<point>> useme = bl;
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
                  point crss = getCross(p1, p2, p3, p4);
                  debug.push_back(crss);
                }
              }
            }
          }
        }
      }
      std::vector<std::vector<point>> delme = bl;
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
              std::vector<int> belong = toBin(circles[i].n, bl.size());
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
      wmax = 0;
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
  UINT i = std::min(o, t);
  UINT j = std::max(o, t);
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
  for (UINT i = 0; i < (circles.size() - 1); i++){
    for (UINT j = i + 1; j < circles.size(); j++){
      UINT r = 0;
      UINT bits = circles[i].n & circles[j].n;
      r = ones(bits);
      relationships.push_back(r);
      //std::cout << i << ", " << j << " - " << r << "\n";
    }
  }
}

void writeFileText(fileText* tmp, std::string fname = ""){
    resetScale();
    std::ofstream result;
    if (fname == ""){
      fname = blSettings.fname.c_str();
    }
    result.open(fname);
    result.write(tmp->getText().c_str(), tmp->getText().size());
    result.close();
}

public:
    borderLine(){}
    borderLine(std::string description, UINT bycol = 0, const char lineSep = 0x00, std::string fname = "nvenn.txt", std::string outputFile = "result.svg"){
        fromSignature = false;
        setElements = nvenn(description, lineSep, bycol);
        std::stringstream vFile;
        vFile << setElements.getCode();
        std::string header;
        std::vector<std::string> groupNames;
        std::vector<std::vector<UINT>> weights;
        std::vector<std::string> labels;
        getline(vFile, header);
        //std::cout << header << std::endl;
        getline(vFile, header);
        std::string ng = purgeLetters(header);
        UINT number = (UINT) atoi(ng.c_str());
        if (number > 0 && number < 20){
            for (UINT i = 0; i < number; i++){
                getline(vFile, header);
                std::string cl = cleanString(header);
                groupNames.insert(groupNames.end(), cl);
                //std::cout << header << std::endl;
            }
            while (!vFile.eof()){
              getline(vFile, header);
              std::vector<UINT> l = getIntegers(header);
              if (l.size() > 1){
                weights.push_back(l);
              }
            }
            init(groupNames, weights);
            setElements = nvenn(description, lineSep, bycol);
        }
        else{
            init();
            setError("Malformed input string. No less than 2 and no more than 20 groups are allowed");
        }
    }
    borderLine(std::vector<std::string> g, std::vector<float> tw, std::vector<std::string> tlabels, std::string inputFile = "venn.txt", std::string outputFile = "result.svg") /// aqui
    {
        fromSignature = false;
        //init(g, tw, tlabels, inputFile, outputFile);
        limits.setClear(true);

        /*writeSVG()*/
    }

    UINT getCurrentStep(){
        return currentStep;
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

    void initCounters(){
      blCounter.setLimits(0, 50u);
      deciderCounter.setLimits(0, 50u);
      keepDistCounter.setLimits(0, 149u);
      refreshScreen.setLimits(1, 5);
    }


    float correctedMinCircRadius(){
      float result = minCircRadius;
      float minCoord = internalScale.minSpan();
      minRat = 0.02 * minCoord;
      if (result < minRat){
        result = minRat;
      }
      return result;
    }

    void doOptimize(bool opt = false){
      blSettings.optimize = opt;
    }

    /** \brief From here on, every step is recorded independently of
     *         optimizations
     * \return void
     *
     */
    void startRefiningSteps(){
      getBestSoFar();
      setFixedCircles();
      setSecureState();
      setPrevState();
      tosolve.solve();
      doOptimize(false);
      udt.init(1e-4, 0.01);
    }

    /** \brief Minimal distance between two circles. Used to
     *         finish the second step
     *
     * \return float
     *
     */
    float minCircDist(){
      UINT flg = twoPow(ngroups) - 1;
      groupIterator ci(circles, 0, 0, flg);
      UINT fst = ci.val();
      UINT scnd = ci.nxt();
      float r = sqDistance(circles[fst], circles[scnd]);
      ci = groupIterator(circles, 0, 0, flg);
      groupIterator ci2(circles, 0, 0, flg);
      while (!ci.isFinished()){
        UINT v = ci.val();
        ci2.reset(v);
        while (!ci2.isFinished()){
          UINT w = ci2.nxt();
          if (v != w){
            float nr = sqDistance(circles[v], circles[w]);
            if (nr < r){
              r = nr;
            }
          }
        }
        ci.nxt();
      }

      float result = sqrt(r);
      return result;
    }

    bool isSimulationComplete(){
      bool result = evaluation.finished();
      if (result){
        if (blSettings.optimize){
          getBestSoFar();
        }
        evaluation.init();
      }
      return result;
    }

    void setGravityPartitions(){
      std::vector<std::vector<UINT>> gp;
      std::vector<std::vector<UINT>> cs;
      UINT nels = circles.size();
      for (UINT i = 0; i < bl.size(); i++){
        nels += bl[i].size();
      }
      for (UINT i = 0; i < sceneTranslator.size(); i++){
        if (sceneTranslator[i] > 0){
          std::vector<UINT> t;
          t.push_back(sceneTranslator[i]);
          t.push_back(i);
          cs.push_back(t);
        }
      }
      gp.clear();
      UINT counter = 0;
      for (UINT i = 0; i < bl.size(); i++){
        for (UINT k = 0; k < bl[i].size(); k++){
          for (UINT j = 0; j < circles.size(); j++){
            UINT mask = 1 << i;
            if (circles[j].radius > 0 && ((circles[j].n & mask) > 0)){
              UINT tn = 0;
              UINT counter = 0;
              bool found = false;
              while (!found && counter < cs.size()){
                std::vector<UINT> tmp = cs[counter];
                if (tmp[0] == circles[j].n){
                  found = true;
                  tn = tmp[1];
                }
                counter++;
              }
              if (!found){
                setError("Error in scene translation");
              }
              gp.push_back({counter, tn});
            }
          }
          counter++;
        }
      }
      /*for (UINT i = 0; i < gp.size(); i++){
        vlog(gp[i]);
      }
      exit(0);*/
      blSettings.part = true;
      tosolve.setGravityPartitions(gp);
    }

    void centerScene(){
      float xmax = internalScale.maxX();
      float xmin = internalScale.minX();
      float ymax = internalScale.maxY();
      float ymin = internalScale.minY();
      float deltax = (xmax + xmin) / 2;
      float deltay = (ymax + ymin) / 2;
      for (UINT i = 0; i < bl.size(); i++){
        for (UINT j = 0; j < bl[i].size(); j++){
          bl[i][j].x -= deltax;
          bl[i][j].y -= deltay;
          internalScale.addToScale(bl[i][j]);
        }
      }
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          circles[i].x -= deltax;
          circles[i].y -= deltay;
          internalScale.addToScale(circles[i]);
        }
      }
      resetScale();
    }

    void attachScene(){
      float springK = scConstants.K;
      UINT cnt = 0;
      tosolve.clearScene();
      tosolve.setBackgroundGravity();
      tosolve.setRodStiffness(1e5);
      sceneTranslator.clear();
      pairDistances.clear();
      UINT level = 1;
      for (UINT i = 0; i < bl.size(); i++){
        UINT lp = cnt;
        for (UINT j = 0; j < bl[i].size(); j++){
          //bl[i][j].flags = bl[i][j].flags | INGRAVID;
          bl[i][j].flags = bl[i][j].flags | GHOST;
          tosolve.addPointP(&(bl[i][j]));
          sceneTranslator.push_back(bl[i][j].n);
          tosolve.addLink(cnt + lp, cnt + j, springK);
          lp = j;
          pairDistances.push_back(cushion * (float) level);
        }
        tosolve.addLink(cnt + lp, cnt, springK);
        cnt += lp + 1;
        level++;
      }
      level *= 2;
      for (UINT i = 0; i < circles.size(); i++){
        if (circles[i].radius > 0){
          tosolve.addPointP(&(circles[i]));
          sceneTranslator.push_back(circles[i].n);
          pairDistances.push_back(cushion * (float) level);
        }
      }
      tosolve.setCushions(pairDistances);
      if (blSettings.part){
        //setGravityPartitions();
      }
      tosolve.setPseudoGravity(true);
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
      scConstants.B = f;
      attachScene();
    }
    void scG(float G = 0){
      tosolve.setG(G);
      scConstants.G = G;
      attachScene();
    }
    void scGhostGrav(bool ghostGrav = true){
      tosolve.setGhostGravity(ghostGrav);
      attachScene();
    }
    void scD(float D = 0){
      tosolve.setDampingConstant(D);
      scConstants.D = D;
      attachScene();
    }
    void scSpringK(float k = 1e3){
      tosolve.setSpringK(k);
      scConstants.K = k;
      attachScene();
    }

    void doubleThePoints(){
      if (doublings < maxdoublings){
        std::vector<std::vector<point>> newbl;
        for (UINT i = 0; i < bl.size(); i++){
          std::vector<point> tbl;
          for (UINT j = 0; j < bl[i].size(); j++){
            point p = bl[i][j];
            point n = bl[i][0];
            if (j < (bl[i].size() - 1)){
              n = bl[i][j+1];
            }
            point newp(p);
            newp.x = (p.x + n.x) / 2;
            newp.y = (p.y + n.y) / 2;
            tbl.push_back(p);
            tbl.push_back(newp);
          }
          newbl.push_back(tbl);
        }
        bl.clear();
        for (UINT i = 0; i < newbl.size(); i++){
          bl.push_back({});
          for (UINT j = 0; j < newbl[i].size(); j++){
            bl[i].push_back(newbl[i][j].clone());
          }
        }
        attachScene();
        doublings++;
      }
    }

    void scSolve(){
      blSettings.dt = tosolve.solve(blSettings.dt, resetV);
      bool incorrect = checkTopol();
      while (incorrect){
        restorePrevState();
        evaluation.init();
        //tolog(_L_ + "Bad topol\n");
        udt.report();
        if (blSettings.dt < blSettings.mindt){
          if (doublings < maxdoublings){
            doubleThePoints();
          }
          else{
            tolog(_L_ + "Cannot solve topol problems\n");
            writeSVG("error.svg");
            setError("Cannot solve topol problems");
          }
        }
        blSettings.dt = udt.cdt();
        //tolog(_L_ + "Bad topol: " + toString(udt.cdt()) + "\n");
        tosolve.solve(blSettings.dt, resetV);
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
      if (blSettings.optimize && keepDistCounter.isMax()){
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
      float d = (this->*currentMeasure)();
      evaluation.add(d);
      //float mfsq = std::log10(tosolve.getMaxFsq());
      //float mvsq = std::log10(tosolve.getMaxVsq());
      //float mdsq = std::log10(tosolve.getMaxDsq());
      displayFloat("EVALUATE", d);
      displayFloat("LASTEVALUATE", evaluation.viewLastComp());
      if (blSettings.optimize){
        if (evaluation.keepState() && sensible()){
          setBestSoFar();
        }
      }
      displayFloat("DT", blSettings.dt);
      displayFloat("SIMTIME", tosolve.simTime());
      //displayFloat("MAXV", mvsq);
      //displayFloat("MAXF", mfsq);
      displayUINT("COUNTER", evaluation.viewCounter());
      displayUINT("CANFINISH", evaluation.viewCanFinish());
      displayUINT("CWI", evaluation.viewCyclesWithoutImprovement());
      //displayUINT("DECIDER", deciderCounter);
      blCounter++;
      deciderCounter++;
      keepDistCounter++;
      blSettings.ncycles++;
    }
    void scSave(std::string fname = ""){
      tosolve.saveScene();
    }

    std::string scCroack(){
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
    std::vector<UINT> ncircles(){
      std::vector<UINT> result;
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

    void setFixedCircles(bool fixedCircles = true){
      if (fixedCircles){
        for (UINT i = 0; i < circles.size(); i++){
          circles[i].flags = setFlag(circles[i].flags, ANCHORED);
        }
      }
      else{
        for (UINT i = 0; i < circles.size(); i++){
          circles[i].flags = unsetFlag(circles[i].flags, ANCHORED);
        }
      }
      attachScene();
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

    std::vector<std::vector<point> > getPoints(){
      return bl;
    }

    std::vector<std::vector<UINT>> getWeights(std::string code){
      std::stringstream v(code);
      std::string line;
      std::vector<std::vector<UINT>> result;
      UINT n = 0;
      UINT v1;
      UINT v2;
      v >> line;
      v >> n;
      if (n > 0 && n < 20){
        for (UINT i = 0; i < n; i++){
          v >> line;
        }
        while (!v.eof()){
          v >> v1;
          v >> v2;
          std::vector<UINT> vs;
          vs.push_back(v1);
          vs.push_back(v2);
          if (!v.eof()){
            result.push_back(vs);
          }
        }
      }
      return result;
    }

    std::vector<std::string> getNames(std::string code){
      std::stringstream v(code);
      std::string line;
      std::vector<std::string> result;
      UINT n = 0;
      v >> line;
      v >> n;
      if (n > 0 && n < 20){
        for (UINT i = 0; i < n; i++){
          v >> line;

          result.push_back(line);
        }
      }
      return result;
    }

    void restoreFromFile(std::string fileName){
      std::string ft = getFileText(fileName);
      std::string st = "<desc id=\'result\'>";
      std::string nd = "</desc>";
      std::string result = "";
      UINT cstart = ft.find(st);
      if (cstart == std::string::npos){
        setError("Cannot find coordinates");
      }
      else{
        UINT cnd = ft.find(nd, cstart);
        if (cnd > cstart && cnd != std::string::npos){
          UINT a = cstart + st.length();
          UINT b = cnd - a;
          result = ft.substr(a, b);
        }
        else{
          setError("Cannot find coordinates");
        }
      }
      restoreBl(result);
    }

    /** \brief Restore a borderline object from a previous execution
     *
     * \param dataFile std::string File with coordinates from borderLine::saveBl
     * \return void
     *
     */
    void restoreBl(std::string dataFile){
        //loadSignature(signature);
        std::istringstream vFile;
        std::string line;
        vFile.str(dataFile);
        bool sane = true;
        std::string errorstr;
        std::vector<std::vector<point>>newbl;
        newbl.clear();
        std::string all;
        for (std::string line; std::getline(vFile, line);){
            all += exchangeChar(line, '\n', 0x00);
        }
        std::istringstream sline(all);
        std::string el;
        std::getline(sline, el, ';');
        if (el == "F"){
            std::getline(sline, el, ';');
            //std::cout << el;
            currentStep = (UINT) atoi(el.c_str());
        }
        else{
            sane = false;
            errorstr += "Failed first step. ";
        }
        std::getline(sline, el, ';');
        if (el == "L"){
            while (el != "" && el != "C"){
                if (el == "L"){
                    newbl.push_back({});
                    std::getline(sline, el, ';');
                }
                UINT ind = newbl.size() - 1;
                while (el != "" && el != "C" && el != "L"){
                    float cx = std::atof(el.c_str());
                    std::getline(sline, el, ';');
                    float cy = std::atof(el.c_str());
                    point p(cx, cy);
                    newbl[ind].push_back(p);
                    std::getline(sline, el, ';');
                }
            }
        }
        std::vector<point> newcircles;
        if (el == "C"){ // (There should be a fixed number of circles.) Not anymore!
            while (el != "S"){
                point p;
                std::getline(sline, el, ';');
                if (el != "S"){
                  p.x = std::atof(el.c_str());
                  std::getline(sline, el, ';');
                  p.y = std::atof(el.c_str());
                  std::getline(sline, el, ';');
                  p.radius = std::atof(el.c_str());
                  std::getline(sline, el, ';');
                  p.n =  (UINT) std::atoi(el.c_str());
                  std::getline(sline, el, ';');
                  p.orig =  (UINT) std::atoi(el.c_str());
                  newcircles.push_back(p);
                }
            }
        }
        else{
            sane = false;
            errorstr += "Failed circles. ";
        }
        //std::getline(sline, el, ';');
        if (sane){
            nvenn sel = nvenn();
            for (UINT i = 0; i < newbl.size(); i++){
                if (el == "S" || el == ""){
                    std::string sn;
                    std::unordered_set<std::string> els;
                    std::getline(sline, el, ';');
                    sn = el;
                    std::getline(sline, el, ';');
                    while (el != "" && el != "S"){
                        els.insert(el);
                        std::getline(sline, el, ';');
                    }
                    sel.addSet(sn, sel.asVector(els));
                }
                else{
                    sane = false;
                }
            }
            std::string tovenn = sel.getCode();
            std::vector<std::string> setNames = getNames(tovenn);
            std::vector<std::vector<UINT>> tw = getWeights(tovenn);
            init(setNames, tw);
            setElements = sel.clone();
            savedState.bl_secure.clear();
            savedState.bl_old10.clear();
            bl.clear();
            circles.clear();
            for (UINT i = 0; i < newcircles.size(); i++){
                circles.push_back(newcircles[i].clone());
                internalScale.addToScale(newcircles[i]);
            }
            for (UINT i = 0; i < newbl.size(); i++){
                bl.push_back({});
                for (UINT j = 0; j < newbl[i].size(); j++){
                    bl[i].push_back(newbl[i][j].clone());
                }
            }
            attachScene();
            blSettings.doCheckTopol = true;
            blSettings.smoothSVG = true;
            resetScale();
        }
        else{
            error = true;
            errorMessage = "Malformed save string: " + errorstr;
            std::cout << "insane" << std::endl;
        }

    }

    void reset(){
      nvenn keep = setElements.clone();
      bl.clear();
      circles.clear();
      init(groups, origw);
      setElements = keep.clone();
    }


    void showInfo(){
      return;
      std::cout << "Number of groups: " << ngroups << "\n";
      std::cout << "Number of circles: " << circles.size() << "\n";
      for (UINT i = 0; i < circles.size(); i++){
        std::cout << "\t" << "circle" << i << " - (" << circles[i].x << ", " << circles[i].y << ")\n";
        std::cout << "\t" << "radius - " << circles[i].radius << "\n";
      }
      std::cout << "Internal scale - " << "(" << internalScale.minX() << ", " << internalScale.minY() << ") to (" <<
                                            internalScale.maxX() << ", " << internalScale.maxY() << ") \n";
      std::cout << "Spans: " << "(" << internalScale.xSpan() << ", " << internalScale.ySpan() << ")\n";
    }


    std::string saveBl(){
        fileText result(";", 80);
        result.addLine("F");
        std::string nc = UINT2string(currentStep);
        result.addLine(nc);
        UINT i; UINT j;
        for (i = 0; i < bl.size(); i++){
            result.addLine("L");
            for (j = 0; j < bl[i].size(); j++){
                std::string x = float2string(bl[i][j].x);
                std::string y = float2string(bl[i][j].y);
                result.addLine(x);
                result.addLine(y);
            }
        }
        result.addLine("C");
        for (i = 0; i < circles.size(); i++){
            std::string x = float2string(circles[i].x);
            std::string y = float2string(circles[i].y);
            std::string r = float2string(circles[i].radius);
            std::string n = toString(circles[i].n);
            std::string o = toString(circles[i].orig);
            result.addLine(x);
            result.addLine(y);
            result.addLine(r);
            result.addLine(n);
            result.addLine(o);
        }
        std::string rst = result.getText() + "\n" + setElements.getSets();
        return rst;
    }


    std::string croack(){
      std::string result = internalScale.croack();
      for (UINT i = 0; i < circles.size(); i++){
        result += circles[i].croack();
      }
      result += "\nLines\n";
      for (UINT i = 0; i < bl.size(); i++){
        result += "Line " + toString(i) + "\n";
        for (UINT j = 0; j < bl[i].size(); j++){
          result += bl[i][j].croack();
        }
      }
      return result;
    }

    void clearWarnings(){
      warn.clear();
    }

    std::vector<point> getWarnings(){
      std::vector<point> result = warn;
      return result;
    }

    std::string coord(float c){
      std::string result = vformat("%.2f", c);
      return result;
    }

    std::string num(int c){
      std::string result = vformat("%d", c);

      return result;
    }

    point ctrlPoint(point start, point curr, point nxt, float sc = 0.2f){
      point result;
      result.x = curr.x + sc * (nxt.x - start.x);
      result.y = curr.y + sc * (nxt.y - start.y);
      return result;
    }

    template<typename T>
    std::string join(std::string interm, std::vector<T> arr) {
      if (arr.size() < 1){
        return "";
      }
      std::stringstream r;
      for (UINT i = 0; i < (arr.size() - 1); i++){
        r << arr[i] << interm;
      }
      r << arr[arr.size() - 1];
      return r.str();
    }

    /** \brief Calculate the position of each label if elements are shown TODO
     *
     *         Based on heuristics: length = font_size * (7.5 * word_length + 5) / 15 and
                                    height = 1.368 * font_size + 0.2811
     * \return std::vector<std::string> svg code with the code for each
     *                                  element label
     *
     */
    std::vector<std::string> svgLabels(){
        std::vector<std::string> result;
        return result;
    }

    void setSVGColor(UINT setNumber, std::vector<UINT> rgbColor){
        std::string s = vformat("#%02x%02x%02x", rgbColor[0], rgbColor[1], rgbColor[2]);
        //std::cout << setNumber << std::endl;
        //std::cout << s << std::endl;
        if (setNumber > 0){
            setNumber--;
        }
        svgParams.svgColors[setNumber] = s;
    }




    bool doIShowThis(){
        return showThis;
    }

    /** \brief Get isolated SVG code
     *  Similar to `toSVG()`, but adds a unique id that serves to isolate the SVG code.
     *  This way, several svgs can coexist in the same web page.
     *
     * \return std::string
     *
     */
    std::string tosvg(){
      std::string cuid = uid();
      return toSVG(cuid).getText();
    }

    std::string uid(){
      std::random_device rd;
      UINT uuid = rd();
      std::string result = "_" + toString(uuid);
      return result;
    }

    fileText toSVG(std::string cuid = ""){
      //if (blSettings.optimize){
       // getBestSoFar();
      //}
      fileText svg;
      int fsize = svgParams.svgFontSize;
      UINT i, j;
      std::string tst;
      point svgtemp;
      svg.addLine("<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 700 500\">");// width=\"700\" height=\"500\">");
      svg.addLine("<defs>");
      svg.addLine("<style type=\"text/css\"><![CDATA[");
      svg.addLine("  .borderLine" + cuid + " {");
      svg.addLine("	   stroke: none;");
      svg.addLine(vformat("	   fill-opacity: %.4f;", svgParams.svgOpacity));
      svg.addLine("  }");
      svg.addLine("  .outLine" + cuid + " {");
      svg.addLine(vformat("	   stroke-width: %.4f;", svgParams.svgLineWidth));
      svg.addLine("	   fill: none;");
      svg.addLine("  }");
      svg.addLine("  .circle" + cuid + " {");
      svg.addLine("	   stroke: #888888;");
      svg.addLine("	   stroke-width: 0.5;");
      svg.addLine("	   fill: none;");
      svg.addLine("    pointer-events: all;");
      svg.addLine("  }");
      svg.addLine("  .spcircle" + cuid + " {");
      svg.addLine("	   stroke: #FF2222;");
      svg.addLine("	   stroke-width: 1;");
      svg.addLine("	   fill: #000000;");
      svg.addLine("	   opacity: 0.2;");
      svg.addLine("    pointer-events: all;");
      svg.addLine("  }");
      svg.addLine("  .tLabel" + cuid + " {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      svg.addText("	   font-size: ");
      svg.addText(num(fsize+4));
      svg.addLine("px;");
      svg.addLine("	   text-anchor: middle;");
      svg.addLine("	   alignment-baseline: central;");
      svg.addLine("  }");
      svg.addLine("  .nLabel" + cuid + " {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      svg.addText("	   font-size: ");
      svg.addText(num(fsize));
      svg.addLine("px;");
      svg.addLine("	   text-anchor: middle;");
      svg.addLine("	   alignment-baseline: central;");
      svg.addLine("  }");
      svg.addLine("  .belong" + cuid + " {");
      svg.addLine("	   font-family: Arial;");
      svg.addLine("    pointer-events: none;");
      svg.addText("	   font-size: ");
      svg.addText(num((int) fsize / 2));
      svg.addLine("px;");
      svg.addLine("	   text-anchor: middle;");
      svg.addLine("	   alignment-baseline: central;");
      svg.addLine("  }");
      for (i = 0; i < ngroups; i++){
        svg.addLine("  .p" + num(i) + cuid + "{");
        svg.addLine("    stroke: none;");
        svg.addLine("    fill: " + svgParams.svgColors[i] + ";");
        svg.addLine("  }");
        svg.addLine("  .q" + num(i) + cuid + "{");
        svg.addLine("    fill: none;");
        svg.addLine("    stroke: " + svgParams.svgColors[i] + ";");
        svg.addLine("  }");
      }
      svg.addLine("]]>");
      svg.addLine("</style>");
      std::string nc = bool2string(blSettings.signalEnd);
      /* This softens the lines*/
      if (blSettings.doCheckTopol){
        if (blSettings.smoothSVG == true){
          for (i = 0; i < ngroups; i++){
            fileText mypath(" ", 80);
            point nxt = place(svgScale, bl[i][0]);
            std::string cpath = "M " + coord(nxt.x) + " " + coord(nxt.y);
            mypath.addLine(cpath);
            for (j = 0; j < (bl[i].size()); j++){
              point prev = place(svgScale, bl[i][prevPoint(i, j)]);
              point curr = place(svgScale, bl[i][j]);
              point next = place(svgScale, bl[i][nextPoint(i, j)]);
              point next2 = place(svgScale, bl[i][nextPoint(i, nextPoint(i, j))]);
              point ctrlfst = ctrlPoint(prev, curr, next);
              point ctrlsec = ctrlPoint(next2, next, curr);
              cpath = " C " + coord(ctrlfst.x) + " " + coord(ctrlfst.y) + " " +
                               coord(ctrlsec.x) + " " + coord(ctrlsec.y) + " " +
                               coord(next.x) + " " + coord(next.y);
              mypath.addLine(cpath);
            }
            svg.addLine("<symbol id=\"bl" + num(i) + cuid + "\">");
            svg.addLine("<path d=\"" + mypath.getText() + " Z\" />");
            svg.addLine("</symbol>");
          }
        } else{
          /* This does not */
          for (i = 0; i < ngroups; i++){
            point nxt = place(svgScale, bl[i][0]);
            std::string cpath = "M " + coord(nxt.x) + " " + coord(nxt.y);
            for (j = 0; j < bl[i].size(); j++){
              nxt = place(svgScale, bl[i][j]);
              cpath += " L " + coord(nxt.x) + " " + coord(nxt.y);
            }
            svg.addLine("<symbol id=\"bl" + num(i) + cuid + "\">");
            svg.addLine("<path d=\"" + cpath + " Z\" />");
            svg.addLine("</symbol>");
          }
        }
      }
      svg.addLine("</defs>");
      //svg.addLine("<!-- signature: " + signature + " -->");
      svg.addLine("<desc>" + join((std::string)";", dataDisplay) + "</desc>");
      std::string bldesc = saveBl();
      svg.addLine("<desc id='result" + cuid + "'>" + bldesc + "</desc>");
      //restoreBl(bldesc);
      svg.addLine("<rect width=\"700\" height=\"500\" style=\"fill:#fff;stroke-width:0\" />");

      // Add fills
      if (blSettings.doCheckTopol){
        for (i = 0; i < ngroups; i++){
          svg.addLine("<use class=\"p" + num(i) + cuid + " borderLine" + cuid + "\" xlink:href=\"#bl" + num(i) + cuid + "\"/>");
        }
        // Add strokes
        for (i = 0; i < ngroups; i++){
          svg.addLine("<use class=\"q" + num(i) + cuid + " outLine" + cuid + "\" xlink:href=\"#bl" + num(i) + cuid + "\"/>");
        }
      }
      for (i = 0; i < circles.size(); i++){
        //printf("%d\n", i);
        if (circles[i].radius > 0){
          svgtemp = place(svgScale, circles[i]);
          //printf("%.4f, %.4f, %.4f\n", svgtemp.x, sc.minX, sc.maxX);
          if (svgtemp.x > svgScale.minX() && svgtemp.x < svgScale.maxX()){
            std::string clss = "circle" + cuid;
            if ((circles[i].flags & IS_OUTSIDE) > 0){
              clss = "spcircle" + cuid;
            }
            tst = vformat("<circle onclick=\"fromCircle(%u)\" class=\"%s\" cx=\"%.4f\" cy=\"%.4f\" r=\"%.4f\" />", circles[i].n, clss.c_str(), svgtemp.x,
                            svgtemp.y, svgtemp.radius);
            svg.addLine(tst);
            if (svgParams.showNumbers){
                float deltaY = -fsize / 3;
                if (svgParams.showRegionNumbers){
                    deltaY = fsize / 2;
                }
                //tst = vformat("<text class=\"tLabel\" x=\"%.2f\" y=\"%.2f\">%s</text>", svgtemp.x, svgtemp.y - 4*fsize/2, labels[i].c_str());
                //svg.addLine(tst);
                tst = vformat("<text class=\"nLabel%s\" x=\"%.2f\" y=\"%.2f\">%g</text>", cuid.c_str(), svgtemp.x, svgtemp.y - deltaY, circles[i].orig);
                svg.addLine(tst);

            }
            // Belongs to
            if (svgParams.showRegionNumbers){
                std::vector<int> tb = toBin(circles[i].n, bl.size());
                std::vector<std::string> blongs;
                UINT m;
                for (m = 0; m < tb.size(); m++){
                  if (tb[m] > 0){
                    std::string t = vformat("%d", m + 1);
                    blongs.push_back(t);
                  }
                }
                std::string bgs = join(", ", blongs);
                float deltaY = 0;
                if (svgParams.showNumbers){
                    deltaY = fsize / 2;
                }
                std::string t = vformat("<text class=\"belong%s\" x=\"%.2f\" y=\"%.2f\">(%s)</text>", cuid.c_str(), svgtemp.x, svgtemp.y + deltaY, bgs.c_str());
                svg.addLine(t);
            }
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
        std::string g = groups[l];
        std::string myg = vformat("p%d", l) + cuid;
        std::string myq = vformat("q%d", l) + cuid;
        std::string addRect = vformat("<rect class=\"%s borderLine%s\" data-nbit=\"%u\" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />",
                myg.c_str(), cuid.c_str(), l, cx, cy, rw, rh);
        std::string addOut = vformat("<rect class=\"%s\" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />",
                myq.c_str(), cx, cy, rw, rh);
        std::string addLegend = vformat("<text class=\"legend%s\" x=\"%.2f\" y=\"%.2f\">%s</text>", cuid.c_str(), cx + dx, cy + rh, g.c_str());
        svg.addLine(addRect);
        svg.addLine(addOut);
        svg.addLine(addLegend);
        cy += dy;
      }
      if (showThis){
        /*for (UINT i = 0; i < bl.size(); i++){
          for (UINT j = 0; j < bl[i].size(); j++){
            point nxt = place(svgScale, bl[i][j]);
            if ((bl[i][j].flags & DELME) > 0){
              std::string tcuid = "spcircle" + cuid;
              std::string tmp = vformat("<circle class=\"%s\" cx=\"%.4f\" cy=\"%.4f\" r=\"2\" />", tcuid.c_str(), nxt.x, nxt.y);
              svg.addLine(tmp);
            }
          }
        }*/
        for (UINT i = 0; i < debug.size(); i++){
          point t = place(svgScale, debug[i]);
          std::string tcuid = "spcircle" + cuid;
          std::string tmp = vformat("<circle class=\"%s\" cx=\"%.4f\" cy=\"%.4f\" r=\"10\" />", tcuid.c_str(), t.x, t.y);
          svg.addLine(tmp);
        }
        debug.clear();
      }
      svg.addLine("</svg>");
      return svg;
    }

    fileText toPS()
    {
        fileText pstext;
        std::string tst;
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
        std::string st = vformat("/step %u def", ngroups);
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
            tst = (std::string) "]";
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

    std::string tohtml(){
      return toHTML().getText();
    }

    fileText toHTML(){
        fileText result;
        std::stringstream l;
        std::vector<std::string> checkids;
        result.addLine("<!DOCTYPE html>");
        result.addLine("<html>");
        result.addLine("<head>");
        result.addLine("<style>");
        result.addLine("body{");
        result.addLine("\tmargin: 0;");
        result.addLine("\tpadding: 0;");
        result.addLine("}");
        result.addLine(".dbutton{");
        result.addLine("\t\theight: 8vh;");
        result.addLine("\t\tvertical-align: middle;");
        result.addLine("}");
        result.addLine(".menus{");
        result.addLine("\twidth: 100%;");
        result.addLine("\theight: 9vh;");
        result.addLine("\tpadding: 0;");
        result.addLine("\tborder: thin solid black;");
        result.addLine("\tborder-radius: 1vw;");
        result.addLine("}");
        result.addLine("#svg{");
        result.addLine("\tfloat: right;");
        result.addLine("};");
        result.addLine(".panel{");
        result.addLine("\twidth: 49vw;");
        result.addLine("\theight: 90vh;");
        result.addLine("\tfloat: left;");
        result.addLine("\tmargin: 0;");
        result.addLine("\tpadding-left: 0.5vw;");
        result.addLine("\tborder: solid thin black;");
        result.addLine("\tborder-radius: 1vh;");
        result.addLine("}");
        result.addLine(".panel25{");
        result.addLine("\twidth: 24vw;");
        result.addLine("\theight: 90vh;");
        result.addLine("\tfloat: left;");
        result.addLine("\tmargin: 0;");
        result.addLine("\tpadding-left: 0.5vw;");
        result.addLine("\tborder: solid thin black;");
        result.addLine("\tborder-radius: 1vh;");
        result.addLine("}");
        result.addLine(".panel75{");
        result.addLine("\twidth: 74vw;");
        result.addLine("\theight: 90vh;");
        result.addLine("\tfloat: left;");
        result.addLine("\tmargin: 0;");
        result.addLine("\tpadding-left: 0.5vw;");
        result.addLine("\tborder: solid thin black;");
        result.addLine("\tborder-radius: 1%;");
        result.addLine("}");
        result.addLine("svg{");
        result.addLine("\twidth: 95%;");
        result.addLine("\theight: 95%;");
        result.addLine("}");
        result.addLine("</style>");
        result.addLine("<script>");
        result.addLine("\tconst cboxes = [];");
        result.addLine("</script>");
        result.addLine("</head>");
        result.addLine("<body>");
        result.addLine("<div class=\"menus\" id=\"uppermenu\">");
        result.addLine("<button class=\"dbutton\" id=\"getSVG\">Download SVG</button>");
        result.addLine("<button class=\"dbutton\" id=\"getPNG\">Download PNG</button>");
        result.addLine("</div>");
        result.addLine("<div class=\"row\" id=\"noscript\">");
        result.addLine("<h1>If this message does not disappear, Javascript is inactive</h1>");
        result.addLine("<h1>You need to activate Javascript to use this interface</h1>");
        result.addLine("</div>");
        result.addLine("<div class=\"panel25\" id=\"info\">");
        result.addLine("<dialog id=\"save\" closedby=\"any\">");
        result.addLine("\t<label for=\"outfile\">Output file:</label>");
        result.addLine("\t<input autofocus id=\"outfile\" name=\"outfile\" type=\"text\" />");
        result.addLine("\t<button id=\"downloadpng\">Download</button>");
        result.addLine("\t<form method=\"dialog\">");
        result.addLine("\t\t<button>Close</button>");
        result.addLine("\t</form>");
        result.addLine("</dialog>");
        result.addLine("<dialog id=\"safety\" closedby=\"any\">");
        result.addLine("\t<span><a id=\"safelink\">If the download does not start, you can try clicking here</a></span>");
        result.addLine("\t<form method=\"dialog\">");
        result.addLine("\t\t<button autofocus>Close</button>");
        result.addLine("\t</form>");
        result.addLine("</dialog>");
        result.addLine("<div id=\"ticks\">");
        std::vector<std::string> setNames = setElements.names();
        result.addLine("<div id=\"checkboxes\">");
        UINT nbit = 0;
        //for (UINT j = setNames.size(); j >= 1; --j){
        for (UINT j = 0; j < setNames.size(); j++){
            UINT i = j;
            std::string el = setNames[i];
            result.addLine("<p>");
            l.str(std::string());
            l << "groupchk" << i;
            std::string id = l.str();
            checkids.push_back(id);
            l.str(std::string());
            l << "<input type=\"checkbox\" class=\"cbox\" id=\""<< id << "\" name=\"" << id << "\" data-nbit=\"" << nbit << "\">";
            l << "<label for=\"" << id << "\">" << el << "</label>";
            result.addLine(l.str());
            result.addLine("<script>cboxes.push(document.getElementById('" + id + "'));</script>");
            result.addLine("</p>");
            nbit++;
        }
        result.addLine("<textarea id=\"reg\" rows=\"20\" cols=\"30\">");
        result.addLine("</textarea>");
        result.addLine("</div>");
        result.addLine("</div>");
        result.addLine("</div>");
        result.addLine("<div class=\"panel75\" id=\"svg\">");
        fileText svg = toSVG();
        result.addLine(svg.getText());
        result.addLine("</div>");
        result.addLine("<script>");
        result.addLine("let currentUrl = null;");
        result.addLine("const dsvg = document.getElementById('getSVG');");
        result.addLine("const dpng = document.getElementById('getPNG');");
        result.addLine("const dlg = document.getElementById('save');");
        result.addLine("const dlg2 = document.getElementById('safety');");
        result.addLine("const outf = document.getElementById('outfile');");
        result.addLine("const dod = document.getElementById('downloadpng');");
        result.addLine("dsvg.addEventListener('click', preparesvg);");
        result.addLine("dpng.addEventListener('click', preparepng);");
        result.addLine("outf.addEventListener('keyup', checkfilename);");
        result.addLine("dod.addEventListener('click', downloadpng);");
        result.addLine("dlg2.addEventListener('close', function(){window.URL.revokeObjectURL(currentUrl);});");
        result.addLine("const outp = document.getElementById('reg');");
        result.addLine("const elements = " + setElements.asJSON() + ";");
		result.addLine("function setout(nreg){");
		result.addLine("\toutp.value = \"\";");
		result.addLine("\tif (elements[nreg] !== undefined){");
		result.addLine("\t\toutp.value = elements[nreg].join(\"\\n\");");
		result.addLine("\t}");
		result.addLine("}");
		result.addLine("function fromCircle(nreg){");
		result.addLine("\tfor (let i = 0; i < cboxes.length; i++){");
		result.addLine("\tlet nb = 1 << i;");
		result.addLine("\t\tif ((nreg & nb) > 0){");
		result.addLine("\t\t\tcboxes[i].checked = true;");
		result.addLine("\t\t}");
		result.addLine("\t\telse{");
		result.addLine("\t\t\tcboxes[i].checked = false;");
		result.addLine("\t\t}");
		result.addLine("\t}");
		result.addLine("\tsetout(nreg);");
		result.addLine("}");
		result.addLine("function intersection(){");
		result.addLine("let region = 0;");
		result.addLine("\tconst checks = document.getElementsByClassName('cbox');");
		result.addLine("\tfor (const c of checks){");
		result.addLine("\t\tif (c.checked){");
		result.addLine("\t\t\tlet n = c.dataset.nbit;");
		result.addLine("\t\t\tregion += 1 << n;");
		result.addLine("\t\t}");
		result.addLine("\t}");
		result.addLine("\t\tfromCircle(region);");
		result.addLine("}");
		result.addLine("function hideMsg(){");
		result.addLine("const nosc = document.getElementById('noscript');");
		result.addLine("nosc.parentNode.removeChild(nosc);");
		result.addLine("}");
		result.addLine("hideMsg();");
		result.addLine("const checks = document.getElementsByClassName('cbox');");
		result.addLine("for (const c of checks){");
		result.addLine("\tc.addEventListener('click', intersection);");
		result.addLine("intersection();");
		result.addLine("}");
		result.addLine("");
		result.addLine("function makePNG(img, jname, callback){");
		result.addLine("\t\tlet canvas = document.createElement('canvas');");
		result.addLine("\t\tconst newW = img.width;");
		result.addLine("\t\tconst newH = img.height;");
		result.addLine("\t\tcanvas.width = newW;");
		result.addLine("\t\tcanvas.height = newH;");
		result.addLine("");
		result.addLine("\t\tlet ctx = canvas.getContext('2d');");
		result.addLine("\t\tctx.drawImage(img, 0, 0, newW, newH);");
		result.addLine("");
		result.addLine("\t\tlet canvasdata = canvas.toDataURL('image/png');");
		result.addLine("\t\tlet a = document.createElement('a');");
		result.addLine("\t\ta.id=\"safelink\";");
		result.addLine("\t\ta.download = jname + \".png\";");
		result.addLine("\t\ta.href=canvasdata;");
		result.addLine("\t\tif (currentUrl !== null){window.URL.revokeObjectURL(currentUrl);}");
		result.addLine("\t\tcurrentUrl = canvasdata;");
		result.addLine("\t\tcallback(a);");
		result.addLine("}");
		result.addLine("");
		result.addLine("function downloadLink(a){");
		result.addLine("\tconst lnk = document.getElementById('safelink');");
		result.addLine("\tconst p = lnk.parentNode;");
		result.addLine("\tconst cnt = lnk.innerHTML;");
		result.addLine("\ta.innerHTML = cnt;");
		result.addLine("\tp.removeChild(lnk);");
		result.addLine("\tp.appendChild(a);");
		result.addLine("\ta.click();");
		result.addLine("\tdlg.close();");
		result.addLine("\tdlg2.show();");
		result.addLine("}");
		result.addLine("");
		result.addLine("function toPNG(jname){");
		result.addLine("\tconst svg = document.getElementsByTagName('svg')[0];");
		result.addLine("\tconst svg_xml = (new XMLSerializer()).serializeToString(svg);");
		result.addLine("\tconst blob = new Blob([svg_xml], {type:'image/svg+xml;charset=utf-8'});");
		result.addLine("\tconst url = window.URL.createObjectURL(blob);");
		result.addLine("\tvar newW = 2800;");
		result.addLine("\tvar newH = 2000;");
		result.addLine("\tvar img = new Image();");
		result.addLine("\timg.width = newW;");
		result.addLine("\timg.height = newH;");
		result.addLine("\timg.src = url;");
		result.addLine("\tif (currentUrl !== null){window.URL.revokeObjectURL(currentUrl);}");
		result.addLine("\tcurrentUrl = url;");
		result.addLine("\timg.addEventListener('load', function(){makePNG(img, jname, downloadLink)});");
		result.addLine("}");
		result.addLine("");
		result.addLine("function toSVG(jname){");
		result.addLine("\tconst svg = document.getElementsByTagName('svg')[0];");
		result.addLine("\tconst svg_xml = (new XMLSerializer()).serializeToString(svg);");
		result.addLine("\tconst blob = new Blob([svg_xml], {type:'image/svg+xml;charset=utf-8'});");
		result.addLine("\tconst url = window.URL.createObjectURL(blob);");
		result.addLine("\tlet a = document.createElement('a');");
		result.addLine("\ta.id=\"safelink\";");
		result.addLine("\ta.download = jname + \".svg\";");
		result.addLine("\ta.href=url;");
		result.addLine("\tconsole.log(url);");
		result.addLine("\tif (currentUrl !== null){window.URL.revokeObjectURL(currentUrl);}");
		result.addLine("\tcurrentUrl = url;");
		result.addLine("\tdownloadLink(a);");
		result.addLine("}");
		result.addLine("");
		result.addLine("function preparesvg(){");
		result.addLine("\tdlg.showModal();");
		result.addLine("\ttoSVG(outf.value);");
		result.addLine("}");
		result.addLine("");
		result.addLine("function preparepng(){");
		result.addLine("\tdlg.showModal();");
		result.addLine("}");
		result.addLine("");
		result.addLine("function downloadpng(){");
		result.addLine("\ttoPNG(outf.value);");
		result.addLine("}");
		result.addLine("");
		result.addLine("function checkfilename(e){");
		result.addLine("\tif (e.keyCode === 13){");
		result.addLine("\t\tdownloadpng();");
		result.addLine("\t}");
		result.addLine("}");
		result.addLine("</script>");
        result.addLine("</body>");
        result.addLine("</html>");
        return result;
    }


    void keepDist(float minDist){
      if (blSettings.doCheckTopol){
        //float md = minDist;
        //if (md < minCircScreenRadius){
          float md = minCircScreenRadius;
        //}
        for (UINT i = 0; i < bl.size(); i++){
          UINT n = twoPow(i);
          std::vector<point> q = getSetBoundaries(n, 2*maxRadius*AIR, true);
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
          std::vector<point> rb;
          std::vector<point> backup = bl[i];
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

    void interpolateToDist(float pDist){
      if (pDist <= 0){
        pDist = 1;
      }

      for (UINT i = 0; i < bl.size(); i++){
        std::vector<point>tempbl;
        float pointDist = pDist;
        if (bl[i].size() < 5){
          pointDist = pDist / 10;
        }
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
      avgStartDist = pDist;
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
        std::vector<point> tempv;
        std::vector<std::vector<point> > tempbl;
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

    void writeSVG(std::string fname = ""){
        fileText tmp = toSVG();
        writeFileText(&tmp, fname);

    }
    void writeHTML(std::string fname = ""){
        //std::cout << "Writing to " << fname << std::endl;
        fileText tmp = toHTML();
        writeFileText(&tmp, fname);

    }

    /** \brief Writes the coordinates of every circle at the end
     *         of step 2. This allows the reproduction of a simulation.
     *         All coordinates are adjusted to four decimals with floor
     *         to make the string representation exact.
     *
     * \return string
     *
     */
    std::string getSignature(){
      fileText result(";", 80);
      result.addLine(toString(seed));
      result.addLine(toString(ngroups));
      for (UINT i = 0; i < groups.size(); i++){
        result.addLine(groups[i]);
      }
      std::vector<float> origs;
      origs.assign((1 << ngroups), 0);
      for (UINT j = 0; j < circles.size(); j++){
        UINT n = circles[j].n;
        origs[n] = circles[j].orig;
      }
      for (UINT i = 0; i < origs.size(); i++){
        result.addLine(toString(origs[i]));
      }
      return result.getText();
    }

    void setError(std::string msg){
      error = true;
      errorMessage = msg;
    }

    void loadSignature(std::string sig){
      sig = exchangeChar(sig, '\n', 0x00);
      bl.clear();
      circles.clear();
      circRadii.clear();
      fromSignature = true;
      std::vector<std::string> rgroups;
      splitString nums(sig, ';');
      std::string nxt;
      nxt = purgeLetters(nums.next());
      seed = atoi(nxt.c_str());
      nxt = purgeLetters(nums.next());
      UINT ng = atoi(nxt.c_str());
      if (ng > 0){
        for (UINT j = 0; j < ng; j++){
          nxt = nums.next();
          rgroups.push_back(nxt);
          //bl.push_back({});
        }
        if (nums.finished()) setError("Incorrect input at middle");
        std::vector<std::vector<UINT>> tw;
        while (!nums.finished()){
          nxt = purgeLetters(nums.next());
          std::string nxt2 = purgeLetters(nums.next());
          std::vector<UINT> snd = {(UINT) std::atoi(nxt.c_str()), (UINT) std::atoi(nxt2.c_str())};
          tw.push_back(snd);
        }
        init(rgroups, tw);
      }
      else{
        setError("Incorrect input\n");
        return;
      }
    }


    /** \addtogroup API
     *  @{
     *  Functions exposed to run the algorithm in different
     *  settings. Briefly, setStep() will leave the object
     *  ready for a step, setCycle() will perform each cycle
     *  of the step and isStepFinished() will inform whether
     *  the conditions for ending the step have been met.
     *  If an error occurred, err() will be set to true,
     *  and errorMsg() will return a string.
     *  When used with a graphical interface,
     *  refresh() is true a given number of
     *  cycles after the last true value.
     *  It can be used to avoid drawing each step.
     *
     *  Thus, if bl is an exported borderLine object,
     *
     *       <pseudocode>
     *        for step in 1:7
     *         setStep(step)
     *         quit = false
     *         while !quit
     *           setCycle(step)
     *           if refresh()
     *             draw_svg_result
     *           if isStepFinished(step)
     *             quit = true
     *        </pseudocode>
     *
     * The API also contains functions to change the appearance
     * of the SVG result (rotation, colors, opacity, ...).
     */

    std::string errorMsg(){
      return errorMessage;
    }

    bool err(){
      return error;
    }

    /** \brief Inits the conditions for a given step
     *
     * \param 0 UINT stepNumber
     * \return bool
     *
     */
    bool setStep(UINT stepNumber = 0){
      bool result = true;
      if (error){
        std::cout << errorMessage << std::endl;
        return false;
      }
      if (stepNumber == attract){
        // Start by saving signature
        signature = getSignature();
        //
        refreshScreen.setLimits(1,1);
        resetV = false;
        setCheckTopol(false);
        currentStep = stepNumber;
      }
      else if (stepNumber == disperse){
        setCheckTopol(false);
        currentStep = stepNumber;
      }
      else if (stepNumber == minimizeCompactness){
        setFixedCircles(false);
        setCheckTopol(true);
        resetOptimize();
        fixTopology();
        oc.maxOutCount = 1 * nregions();
        oc.outCount = 0;
        oc.optVal = compactness();
        optStep.init(oc.optVal);
        oc.optVal = outCompactness(&optStep, &borderLine::furthestPoint,
                                   &borderLine::compactness, &borderLine::countCrossings);
        currentStep = stepNumber;
      }
      else if (stepNumber == minimizeCrossings){
        resetOptimize();
        fixTopology(1e-4);
        oc.maxOutCount = 1 * nregions();
        oc.outCount = 0;
        oc.optVal = countCrossings();
        optStep.init(oc.optVal);
        oc.optVal = outCompactness(&optStep, &borderLine::crossestPoint,
                                   &borderLine::countCrossings, &borderLine::compactness);
        currentStep = stepNumber;
      }
      else if (stepNumber == contract){
        udt.init(1e-4, 0.01);
        resetOptimize();
        fixTopology();
        setCheckTopol(true);
        evaluation.init();
        evaluation.setConstants(50, 50);
        if (checkTopol() == false){
          interpolateToDist(minCircDist());
          setPrevState();
          setSecureState();
        }
        else{
          listOutsiders();
          std::ofstream result;
          fileText svgfile = toSVG();
          result.open("error.svg");
          result.write(svgfile.getText().c_str(), svgfile.getText().size());
          result.close();
          error = true;
          errorMessage = "Could not fix topology at start\n";
          //std::cout << croack(); exit(0);
        }
        resetTimer();
        attachScene();
        scFriction(100);
        scD(1e2);
        scG(1e-1);
        scSpringK(1e4);
        scGhostGrav(false);
        getBestSoFar();
        currentStep = stepNumber;
      }
      else if (stepNumber == refineCircles){
        resetTimer();
        resetOptimize();
        evaluation.init();
        evaluation.setConstants(10, 50);
        setCheckTopol(true);
        float d = getEmbellishDist(2);
        interpolateToDist(d);
        //setGravityPartitions();
        scSpringK(1e4);
        scFriction(1000);
        scG(1e-1);
        scD(1e2);
        scGhostGrav(true);
        getBestSoFar();
        currentStep = stepNumber;
      }
      else if (stepNumber == embellishLines){
        setCheckTopol(true);
        blSettings.smoothSVG = true;
        if (checkTopol() == true){
          restorePrevState();
        }
        resetTimer();
        resetOptimize();
        evaluation.init();
        evaluation.setConstants(10, 0, 1.01, 30);
        startRefiningSteps();
        resetTimer();
        float d = getEmbellishDist(0.4);
        interpolateToDist(d);
        //setGravityPartitions();
        this->currentMeasure = &borderLine::getArea;
        resetV = true;
        scG(2e-1);
        scD(50);
        scSpringK(5e4);
        scFriction(100);
        oc.maxOutCount = 70;
        oc.outCount = 0;
        oc.optVal = 0;
        setPrevState();
        setSecureState();
        currentStep = stepNumber;
      }
      else{
        result = false;
      }
      return result;
    }
    bool setCycle(UINT stepNumber = 0){
      dataDisplay.clear();
      displayUINT("STEP", stepNumber);
      if (error){
        return false;
      }
      bool result = true;
      if (stepNumber == attract){
        setForcesFirstStep();
        solve();
      }
      else if (stepNumber == disperse){
        setForcesSecondStep();
        setContacts(false, true, 3*maxRad()*AIR);
        solve(true);
      }
      else if (stepNumber == minimizeCompactness){
        float thisOut = outCompactness(&optStep, &borderLine::furthestPoint,
                                       &borderLine::compactness, &borderLine::countCrossings);
        if (optStep.hasEnded()){
          if (thisOut < oc.optVal){
            oc.optVal = thisOut;
            oc.outCount = 0;
            //showCrossings();
          }
          else{
            oc.outCount = oc.outCount + 1;
            tolog("Outcount: " + toString(oc.outCount) + "\n");
            tolog("Cval: " + toString(oc.optVal) + "\n");
          }
          //fixTopology();
        }
      }
      else if (stepNumber == minimizeCrossings){
        float thisCross = outCompactness(&optStep, &borderLine::crossestPoint,
                                         &borderLine::countCrossings, &borderLine::compactness);
        if (optStep.hasEnded()){
          if (thisCross < oc.optVal){
            oc.optVal = thisCross;
            oc.outCount = 0;
            // Check for bad topology, restart if necessary
            fixTopology();
            setCheckTopol(true);
            if (checkTopol()){
                oc.maxOutCount *= 2;
                optStep.init(oc.optVal);
            }
            //showCrossings();
          }
          else{
            oc.outCount = oc.outCount + 1;
            tolog("Outcount: " + toString(oc.outCount) + "\n");
          }
          //fixTopology();
        }
      }
      else if (stepNumber == contract || stepNumber == refineCircles){
        if (checkTopol()){
          setError("Bad starting topology");
        }
        scSolve();
        /*if (tosolve.getDebugSignal()){
          writeSVG();
          exit(0);
        }*/
      }
      else if (stepNumber == embellishLines){
        scSolve();
        oc.outCount++;
      }
      else{
        result = false;
      }
      return result;
    }
    bool isStepFinished(UINT stepNumber = 0){
      bool result = false;
      if (error == true){
        return true;
      }
      if (stepNumber == attract){
        float tc = maxCircleVsq;
        if (tc > 0 && tc < 1e-2){
          result = true;
        }
      }
      else if (stepNumber == disperse){
        if (minCircDist() > (2*maxRad()*AIR)){
          result = true;
          setCheckTopol(true);
          fixTopology();
        }
      }
      else if (stepNumber == minimizeCompactness || stepNumber == minimizeCrossings){
        if (oc.outCount > oc.maxOutCount){
          result = true;
          displayFloat("COMPACTNESS", compactness());
          displayUINT("CROSSINGS", countCrossings());
          setCheckTopol(true);
          fixTopology();
        }
      }
      else if (stepNumber == contract || stepNumber == refineCircles){
        result = isSimulationComplete();
      }
      else if (stepNumber == embellishLines){
        result = isSimulationComplete();
        if (result){
          blSettings.smoothSVG = true;
          //currentStep = 1;
        }
        //if (getMaxVsq() > 1e-7){
        //  result = false;
        //}
      }
      refreshScreen++;
      return result;
    }
    bool refresh(){
      return refreshScreen.isMax();
    }

    void rotateScene(float alpha){
      centerScene();
      float cosa = std::cos(alpha);
      float sina = std::sin(alpha);
      std::vector<std::vector<point>> newbl;
      std::vector<point> newcirc;
      for (UINT i = 0; i < bl.size(); i++){
        newbl.push_back({});
        for (UINT j = 0; j < bl[i].size(); j++){
          point t;
          t.x =  cosa * bl[i][j].x + sina * (bl[i][j].y);
          t.y = -sina * bl[i][j].x + cosa * (bl[i][j].y);
          newbl[i].push_back(t);
        }
      }
      for (UINT i = 0; i < circles.size(); i++){
        point t = circles[i];
        t.x =  cosa * circles[i].x + sina * (circles[i].y);
        t.y = -sina * circles[i].x + cosa * (circles[i].y);
        newcirc.push_back(t);
      }
      bl = newbl;
      circles = newcirc;
      resetScale();
    }

    void loadPalette(UINT n){
        std::vector<UINT> arr = svgPalettes.getPalette(n);
        svgParams.svgColors.clear();
        for (UINT i = 0; i < ngroups; i++){
            std::string c = vformat("#%06x", arr[i]);
            svgParams.svgColors.push_back(c.c_str());
        }
        //init postscript colors
        for (UINT i = 0; i < ngroups; i++)
        {
            colors.push_back(toRGB(arr[i], 1));
        }
    }

    void setRGBColor(UINT setNumer, UINT red, UINT green, UINT blue){
        std::vector<UINT> c = {red, green, blue};
        setSVGColor(setNumer, c);
    }

    void setVennColor(UINT setNumber, std::string svgColor){
      svgParams.svgColors[setNumber] = svgColor;
    }

    void setSVGOpacity(float t){
        svgParams.svgOpacity = t;
    }

    void setSVGLineWidth(float lw){
        svgParams.svgLineWidth = lw;
    }

    void showCircleNumbers(bool s){
        svgParams.showNumbers = s;
    }
    void showRegionNumbers(bool s){
        svgParams.showRegionNumbers = s;
    }
    void setFontSize(UINT fs){
        svgParams.svgFontSize = fs;
    }

    std::string getVennRegion(UINT r){
        std::vector<std::string> s = setElements.getRegion(r);
        std::string result = join("\n", s);
        return result;
    }

    std::string getVennRegion(std::vector<std::string> r){
        std::vector<std::string> s = setElements.getRegion(r);
        std::string result = join("\n", s);
        return result;
    }

    std::vector<std::string> getVennRegionVector(UINT r){
        std::vector<std::string> s = setElements.getRegion(r);
        return s;
    }

    std::vector<std::string> getVennRegionVectorL(std::vector<std::string> r){
        std::vector<std::string> s = setElements.getRegion(r);
        return s;
    }

    /** @} */

    std::vector<std::vector<point>> getBl(){
        return bl;
    }

    std::vector<std::string> getSetNames(){
        return groups;
    }

    std::string regionDescription(UINT nreg){
      std::vector<std::string> r;
      for (UINT i = 0; i < groups.size(); i++){
        UINT mask = 1 << i;
        if ((nreg & mask) > 0){
          r.push_back(groups[i]);
        }
      }
      std::string result = "(" + join(", ", r) + ")";
      return result;
    }


    bool simulate(bool verbose = false){
      restart_log();
      reset();
      UINT cstep = currentStep;
      for (UINT step = currentStep; step < 8; step++){
        bool bQuit = false;
        if (verbose){
            std::cout << "Step " << step << std::endl;
        }
        bool success = setStep(step);
        if (!success) return false;
        while (!bQuit){
          setCycle(step);
          if (err()){
            std::cout << errorMessage << std::endl;
            bQuit = true;
          }
          //if (refreshScreen.isMax()) writeSVG();
          if (isStepFinished(step)){
            bQuit = true;
          }
        }
        cstep = step;
      }
      currentStep = cstep;
      return true;
    }

};

borderLine fromSets(std::string sets, UINT byCol){
    borderLine result(sets, byCol);
    return result;
}

borderLine fromSetFile(std::string filepath, UINT byCol = 0x00){
    std::string nfo = getFileText(filepath);
    borderLine result = fromSets(nfo, byCol);
    return result;
}

borderLine fromPrevRun(std::string filename){
  borderLine result;
  result.restoreFromFile(filename);
  return result;
}

borderLine getInfoFromStream(std::stringstream& vFile, const char lineSep = 0x00, std::string fname = "nvenn.txt", std::string outputFile = "result.svg"){
  std::string header;
  std::vector<std::string> groupNames;
  std::vector<int> temp;
  std::vector<float> weights;
  std::vector<std::string> labels;
  if (lineSep > 0x00){
    std::string s = exchangeChar(vFile.str(), lineSep, '\n');
    vFile.clear();
    vFile << s;
  }
  getline(vFile, header);
  //std::cout << header << std::endl;
  getline(vFile, header);
  std::string ng = purgeLetters(header);
  UINT number = (UINT) atoi(ng.c_str());
  if (number > 0 && number < 100){
    //std::cout << std::endl << number << " groups:" << std::endl;
    for (UINT i = 0; i < number; i++){
        getline(vFile, header);
        std::string cl = cleanString(header);
        groupNames.insert(groupNames.end(), cl);
        //std::cout << header << std::endl;
    }
    UINT n = (UINT) twoPow(number);
    for (UINT i = 0; i < n; i++){
        getline(vFile, header); //  get the whole line
        int l = header.find_first_of(" ");
        std::string w = purgeLetters(header);
        if (l > 0){
          w = header.substr(0,l);
        }
        weights.insert (weights.end(), atoi(w.c_str())); // it takes the first number
        std::string label = "";
        /*try
        {
            label = header.substr(header.find_first_of(" "));
        }
        catch (const std::exception& e)
        {
            //std::cout << i << std::endl;
            label = "";
        }*/
        labels.insert (labels.end(), label);
        //std::cout << "w=" << w << "  label:" << label << std::endl;
        //getline(vFile, header, ' '); /// get the number
        //weights.insert (weights.end(), atoi(header.c_str()));
        //getline(vFile, header) ;  ///  get the rest of the line with the labels
        //labels.insert (labels.end(), header);
        temp = toBin(i, number);
        //printv(temp);
        //std::cout << ".- " << weights[i] << " : " << labels[i] << std::endl;
    }
  }
  borderLine lines(groupNames, weights, labels, fname, outputFile);
  return lines;
  //else{
  //  borderLine l({"one"}, {0, 1}, {"", ""});
  //  l.setError("Malformed input string. No more than 100 groups are allowed");
  //  return l;
  //}
}


borderLine getFileInfo(std::string fname, std::string outputFile){
    std::ifstream vFile;
    vFile.open(fname.c_str());
    std::stringstream content;
    content << vFile.rdbuf();
    borderLine lines = getInfoFromStream(content, 0x00, fname, outputFile);

    vFile.close();
    std::string dataFile = outputFile + ".data";
    vFile.open(dataFile.c_str());
    if (false){ //vFile.good() == true){ // Unfinished
        vFile.close();
        lines.restoreBl(dataFile);
    }


    return lines;
}



#endif // TOPOL_H_INCLUDED
