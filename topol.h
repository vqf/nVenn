#ifndef TOPOL_H_INCLUDED
#define TOPOL_H_INCLUDED

#include <vector>
#include <cmath>
#include <time.h>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <algorithm>
#include <math.h>
//#include <windows.h>

#define CIRCLE_MASS 200.0f
#define POINT_MASS 20

// Flags for points
#define IS_OUTSIDE        0x01  // The circle is in an incorrect space. Set to 0x01 to highlight
#define DO_NOT_EMBELLISH  0X02  // The point has already been corrected
#define TAKEN_OUT         0x04  // The point has been moved outside
#define USED              0x08  // The point has been used in a procedure. Remember to reset at the end

typedef unsigned int UINT;


/**< Code enclosed into DEBUGONLY() will only appear if DEBUG is defined.
Only in that case the function tolog(toString(__LINE__) + "\n" + ) will be available and executed.
 */

#define DEBUG
//#undef DEBUG

#ifdef DEBUG
#define DEBUGONLY(a) a
#else
#define DEBUGONLY(a)
#define tolog(toString(__LINE__) + "\n" + a)
#endif // DEBUG


using namespace std;


DEBUGONLY(


void tolog(string t){
  ofstream f;
  f.open("log.txt", std::ios_base::app);
  f.write(t.c_str(), t.size());
  f.close();
}
)

UINT setFlag(UINT flag, UINT mask){
  UINT result = flag | mask;
  return result;
}

UINT unsetFlag(UINT flag, UINT mask){
  UINT result = flag & (~mask);
  return result;
}

template<typename T>
string toString(T input)
{
    ostringstream result;
    result << input;
    return result.str();
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

    int i;                          // decreasing iterator
    int counter = 0;                // increasing iterator
    int result = 0;
    for (i = 0; i < v.size(); i++)
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

/** \brief Computes the cartesian distance between
 * points (\p x0, \p y0) and (\p x1, \p y1)
 *
 * \param x0 float
 * \param y0 float
 * \param x1 float
 * \param y1 float
 * \return float
 *
 */
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
  void initPoint(){
      cancelForce = false;
      fx = 0.0f;
      fy = 0.0f;
      mass = POINT_MASS;
      n = 0;
      orig = 0.0f;
      radius = 0.0f;
      vx = 0.0f;
      vy = 0.0f;
      x = 0.0f;
      y = 0.0f;
      flags = 0;
      inContact = false;
      softenVel = false;
    }
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
    uint8_t flags;
    point()
    {
        x = 0;
        y = 0;
        radius = 0;
        initPoint();
    }
    point(float px, float py){
      x = px;
      y = py;
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
    string croack(){
      ostringstream r;
      r << "X: " << x << ", Y: " << y << "\tN: " << n << "\tradius: " << radius << "\n";
      return r.str();
    }
};
bool operator==(point p1, point p2){
  bool result = false;
  if (p1.x == p2.x && p1.y == p2.y){
    result = true;
  }
  return result;
}
bool operator!=(point p1, point p2){
  return !(p1 == p2);
}

class borderLine;

class groupIterator {
  UINT current;
  UINT cgroup;
  UINT first;
  UINT bits;
  UINT sze;
  UINT mask;
  vector<point> circles;

  /** \brief Next number counting from @n that has bit @group set to 1
   *
   * \param n UINT Starting number
   * \param group UINT Set number
   * \param size UINT Total number of elements
   * \return UINT If the next number is higher than @size, the function returns
   *         the first number in @group
   *
   */
  UINT nextInGroup(UINT n) {
    UINT result = n;
    if (mask == (mask + 1)) {
      return result;
    }
    result++;
    if ((result & mask) == 0){
      UINT anti = mask - 1;
      anti = ~anti;
      result = result & anti;
      result += mask;
    }
    if (bits > 0 && result > sze) {
      result = mask;
    }
    return result;
  }

public:
  groupIterator(vector<point> circs, UINT group, UINT nBits, UINT starting = 0) {
    first = 0;
    circles = circs;
    cgroup = group;
    UINT s = 1 << cgroup;
    mask = s;
    bits = nBits;
    sze = 1 << bits;
    setval(starting);
    first = current;
  }
  /*~groupIterator(){
    tolog(toString(__LINE__) + "\n" + "Called destructor\n");
  }*/
  UINT val() { return current; }

  void setval(UINT v){
    UINT sanity = v;
    if ((v & mask) > 0 && circles[v].radius > 0){
      current = v;
    }
    else{
      current = nextInGroup(v);
      while (circles[v].radius == 0){
        v = nextInGroup(v);
        if (v == sanity){
          exit(1);
        }
        current = v;
        if (current == first && first > 0){
          current = 0;
          return;
        }
      }
    }
  }

  void reset(UINT from = 0){
    setval(from);
  }

  UINT nxt(UINT from = 0) {
    if (from > 0){
      setval(from);
    }
    if (current == 0){
      return 0;
    }
    current = nextInGroup(current);
    setval(current);
    if (current == first) {
      current = 0;
      return 0;
    }
    return current;
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


class circleIterator {
  UINT current;
  UINT first;
  UINT sze;
  UINT mask;
  vector<point> circles;

public:
  circleIterator(vector<point> circs, UINT npoints, UINT starting = 0) {
    first = 0;
    circles = circs;
    sze = npoints;
    setval(starting);
    first = current;
  }
  /*~groupIterator(){
    tolog(toString(__LINE__) + "\n" + "Called destructor\n");
  }*/
  UINT val() { return current; }

  void setval(UINT v){
    UINT sanity = v;
    if (circles[v].radius > 0){
      current = v;
    }
    else{
      current = v + 1;
      while (circles[v].radius == 0){
        v++;
        if (v >= sze){
          v = 0;
        }
        if (v == sanity){
          exit(1);
        }
        current = v;
        if (current == first && first > 0){
          current = 0;
          return;
        }
      }
    }
  }

  void reset(UINT from = 0){
    setval(from);
  }

  UINT nxt(UINT from = 0) {
    if (from > 0){
      setval(from);
    }
    if (current == 0){
      return 0;
    }
    current++;
    setval(current);
    if (current == first) {
      current = 0;
      return 0;
    }
    return current;
  }
};



class tangent {
  float slope;
  UINT quadrant;

public:
  tangent(float dx, float dy) {
    slope = 0;
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
  tangent(point p1, point p2){
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    slope = 0;
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
      tolog(toString(__LINE__) + "\n" + "Incorrect tangent: \n" + p1.croack() + p2.croack());
      exit(0);
    }
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
  void operator=(tangent t) {
    quadrant = t.quadrant;
    slope = t.slope;
  }
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

  tangent *t;

public:
  ccwangle(point p0, point p1, point p2) {
    t = NULL;
    float dx0 = p0.x - p1.x;
    float dy0 = (p0.y - p1.y);
    float dx1 = p2.x - p1.x;
    float dy1 = (p2.y - p1.y);
    //tangent g(dx1, dy1);
    t = new tangent(dx1, dy1);
    tangent u(dx0, dy0);
    t->rotate(u);
    //t = &g;
    //tolog(toString(__LINE__) + "\n" + t->croack() + "-\n");
  }

  bool operator<(ccwangle c) {
    //tolog(toString(__LINE__) + "\n" + t->croack() + "\n" + c.t->croack() + "\n\n");
    if (*t < *(c.t)){
      return true;
    }
    else{
      return false;
    }
  }
  string croack(){
    return t->croack();
  }
};

class timeMaster
{
    UINT count;
    UINT backcount;
    UINT topcount;
    float unstable;
    float currentdt;
    float defaultst;
    float stepdt;
public:

    timeMaster()
    {}

    void init(float unst, float mystepdt)
    {
        count = 0;
        topcount = 100;
        backcount = topcount;
        unstable = unst;
        defaultst = unst;
        currentdt = unst;
        stepdt = mystepdt;
    }

    void reset(){
      count = 0;
        backcount = topcount;
        unstable = defaultst;
        currentdt = defaultst;
    }

    void clear()
    {
        init(unstable, stepdt);
    }

    float cdt(){
      return currentdt;
    }

    UINT counter(){
      return count;
    }

    UINT backcounter(){
      if (backcount > topcount){
        backcount = topcount;
      }
      return backcount;
    }

    void report()
    {
      if (currentdt >= unstable){
        ++count;
      }
      else
      {
          count = 0;
          unstable = currentdt;
          ++backcount;
      }
      if (count >= 5)
      {
          backcount = topcount;
          count = 0;
          unstable = currentdt * stepdt;
      }
      currentdt *= stepdt;
    }

    void poke(){
      if (currentdt < unstable)
      {
          currentdt /= stepdt;
      }
      else{
        --backcount;
        if (backcount <= 0 && unstable < defaultst){ // Ok, try again
          unstable /= stepdt;
          currentdt = unstable;
          backcount = topcount;
        }
      }
    }

    float unstabledt()
    {
        return unstable;
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


class borderLine
{
    friend class glGraphics;
    friend class groupIterator;

    binMap* bm;
    vector<string> groups;
    vector<point> p;
    vector<vector<point> > bl; /**< Vector of lines. Each line is a vector of points */
    vector<vector<point> > bl_secure; /**< For changes in the number of points */
    vector<vector<point> > bl_old10;
    vector<point> circles;
    vector<point> scircles; /**< Circles sorted according to @n */
    vector<point> circles_secure;
    vector<point> circles_old10;
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
    int totalExpectedSurface;
    vector<string> dataDisplay;
    timeMaster udt;
    blData blSettings;
    float minRat;
    bool showThis;

    void initBlData(blData* b){
      b->sk = 1e3f;
      b->dt = 2.5e-2f;
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
      b->maxRunningTime = 200; // 300 seconds to finish the first part
    }

    UINT leftmostCircle(UINT group){
      groupIterator git(scircles, group, bl.size(), 1);
      UINT result = git.val();
      float lx = scircles[result].x;
      while(git.nxt() > 0){
        UINT n = git.val();
        if (scircles[n].radius > 0 && scircles[n].x < lx){
          result = n;
          lx = scircles[n].x;
        }
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
        bl_old10 = bl;
        ncyles_old10 = blSettings.ncycles;
        circles_old10 = circles;
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




    /** \brief Get the circles situated most counterclockwise in @group
     *         starting from circle @p.
     * \param p UINT Number of the circle
     * \param group UINT Set number
     * \return UINT
     *
     */
    UINT ccw(UINT p, UINT group){

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
            //if (trad > maxRadius){
            //  maxRadius = trad;
            //}
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
      UINT ncircles = circles.size();
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


    void setScale(point p)
    {
      UINT r = 0;
      if (p.radius > 0){
        r = p.radius;
      }
        if (internalScale.isClear() == true)
        {
            internalScale.setMinX(p.x - 2 * r);
            internalScale.setMinY(p.y - 2 * r);
            internalScale.setMaxX(p.x + 2 * r);
            internalScale.setMaxY(p.y + 2 * r);
            internalScale.setClear(false);
        }
        else
        {
            if (p.x < internalScale.minX())
                internalScale.setMinX(p.x - r);
            if (p.y < internalScale.minY())
                internalScale.setMinY(p.y - r);
            if (p.x > internalScale.maxX())
                internalScale.setMaxX(p.x + r);
            if (p.y > internalScale.maxY())
                internalScale.setMaxY(p.y + r);
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
        setScale(fpoint);
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
            setScale(cpoint);
            p.push_back(cpoint);
            cpoint.x = 6 * i + 2;
            setScale(cpoint);
            p.push_back(cpoint);
            // add circles
        }
        //last point
        cpoint.x = 6 * (width + 1 + cstart);
        cpoint.y = 6 * (height + 1 + cstart);
        cpoint.mass = POINT_MASS;
        setScale(cpoint);
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
        for (i = 0; i < bl_secure.size(); i++)
        {
            for(j = 0; j < bl_secure[i].size(); j++)
            {
                bl_secure[i][j].reset();
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
        float repulsion = blSettings.sk * kattr * 4;
        point result;
        float factor = (float) getRelationships(p0.n, p1.n);
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
        float repulsion = 0; //-blSettings.sk * kattr;
        point result;
        //float factor = (float) getRelationships(p0.n, p1.n);
        float radius = p0.radius + p1.radius;
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
              tolog(toString(__LINE__) + "\n" + "Line " + toString(i) + ", " + circles[j].croack());
            }
          }
        }
      }
    }

    void embellishTopology(bool logit = false){
      tangent lft(0, -1);
      tangent rgh(0, 1);
      for (UINT i = 0; i < bl.size(); i++){
        bool again = true;
        while (again){
          again = false;
          vector<point> newbl;
          for (UINT j = 0; j < bl[i].size(); j++){
            if (!again){
              point nxt = bl[i][0];
              if (j < (bl[i].size() - 1)){
                nxt = bl[i][j+1];
              }
              point current = bl[i][j];
              newbl.push_back(current);
              float dsq3 = sqDistance(current, nxt);
              if (dsq3 > 0){
                float sq3 = sqrt(dsq3);
                UINT fcirc = 0;
                for (UINT k = fcirc; k < circles.size(); k++){
                  point c = circles[k];
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
                        c.flags = setFlag(c.flags, USED);
                        fcirc = k + 1;
                        tangent tp(nxt.x - current.x, nxt.y - current.y);
                        float t = sqrt(c.radius * c.radius - h * h);
                        float d1 = sqrt(dsq1 - h * h);
                        float d2 = sqrt(dsq2 - h * h);
                        float x1 = d1 - t;
                        float x2 = d2 - t;
                        //tolog(toString(__LINE__) + "\n" + "---\n");
                        //tolog(toString(__LINE__) + "\n" + "I: " + toString(i) + "\t J: " + toString(j) + "\n");
                        //tolog(toString(__LINE__) + "\n" + current.croack() + nxt.croack() + c.croack());
                        //tolog(toString(__LINE__) + "\n" + "D1: " + toString(dsq1) + ", x1: " + toString(x1) + "\n");
                        //tolog(toString(__LINE__) + "\n" + "h: " + toString(h) + "\n");
                        //tolog(toString(__LINE__) + "\n" + "Mask: " + toString(twoPow(i)) + "\t" + "Inside: " + toString(inside) + "\n");
                        //tolog(toString(__LINE__) + "\n" + "---\n");
                        point pst1 = tp.transformPoint(current, x1);
                        point pst2 = tp.transformPoint(current, d1 + t) ;
                        newbl.push_back(pst1);
                        tangent tr = tp + rgh;
                        //tolog(toString(__LINE__) + "\n" + "Outside: \n" + tp.croack() + tr.croack());
                        if (inside){
                          tr = tp + lft;
                          //tolog(toString(__LINE__) + "\n" + "Inside: \n" + tp.croack() + tr.croack());
                        }
                        point newp = tr.transformPoint(c, c.radius);
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
      //writeSVG("addlines.svg");
      polishLines();
      //writeSVG("polishlines.svg");
      embellishTopology();
      //writeSVG("embellish.svg");
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
              point safety;
              if (incorrect){
                if (logit){
                  tolog(toString(__LINE__) + "\n" + "Fixing circle " + toString(circles[j].n) +
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
                    float mindsena = 0;
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
                        tolog(toString(__LINE__) + "\nLine: " + toString(k) + ", segment" + toString(i) + ", circle " + toString(circles[j].n) + "\n");
                        tolog("minV " + toString(minV) + ", ddsq " + toString(ddsq) + "\n");
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
                    tolog(t.croack() + toc1.croack() + toc2.croack() + toc4.croack());
                  }
                  for (UINT i = oi.nextVertex; i < bl[k].size(); i++){
                    newpoints.push_back(bl[k][i]);
                  }
                  bl[k] = newpoints;
                  tolog(toString(__LINE__) + " - " + toString(circleTopol(p3, belong, k)) + "\n");
                  //writeSVG("delme.svg");
                  //exit(0);
                  //tolog(toString(circleTopol(p3, belong, k)) + "\n");
                }
                else{
                  tolog("Somehow, this one did not make it.\n");
                  tolog("Line " + toString(k) + ", segment " + toString(oi.vertex) + "\n");
                  tolog(oi.outsider.croack());
                  writeSVG("delme.svg");
                  exit(1);
                }
                incorrect = circleTopol(circles[j], belong, k); //Was it fixed?
                if (incorrect){
                  writeSVG("error.svg");
                  tolog("Could not fix circle " + toString(circles[j].n) + " with line " + toString(k) + ".\n");
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


    /** \brief Fix topology by excluding circles from improper borderlines
     *
     * \param false bool logit= Whether the function should log operations.
     *        Ignored if DEBUG is undef.
     * \return void
     *
     */
    void fTopology(bool logit = false){
      for (UINT i = 0; i < bl.size(); i++){
        vector<point> outsiders;
        for (UINT j = 0; j < circles.size(); j++){
          vector<int> belong = toBin(circles[j].n, bl.size());
          if (circles[j].radius > 0 && belong[i] == 0){
            bool incorrect = circleTopol(circles[j], belong, i);
            if (incorrect){
              outsiders.push_back(circles[j]);
            }
          }
        }
        vector<outsiderInfo> oInfo;


        for (UINT j = 0; j < outsiders.size(); j++){
          float minV = -1;
          UINT tvertx = 0;
          UINT tvertnx = 0;
          float mindsena = 0;
          bool countThisOne = true;
          for (UINT k = 0; k < bl[i].size(); k++){
            UINT vnx = 0;
            if (k < (bl[i].size() - 1)){
              vnx = k + 1;
            }
            point nxt = bl[i][vnx];
            float v = 0;
            float d1sq = (outsiders[j].x - bl[i][k].x) * (outsiders[j].x - bl[i][k].x) +
                         (outsiders[j].y - bl[i][k].y) * (outsiders[j].y - bl[i][k].y);
            float d2sq = (nxt.x - outsiders[j].x) * (nxt.x - outsiders[j].x) +
                         (nxt.y - outsiders[j].y) * (nxt.y - outsiders[j].y);
            float dsq = (nxt.x - bl[i][k].x) * (nxt.x - bl[i][k].x) +
                        (nxt.y - bl[i][k].y) * (nxt.y - bl[i][k].y);
            float d = sqrt(dsq);
            float dsena = (dsq + d1sq - d2sq) / (2*d);
            float ddsq = d1sq - dsena * dsena;
            if (dsena < outsiders[j].radius){
              countThisOne = false;
            }
            if (dsq < d1sq && dsq < d2sq && dsena > 0 && (minV < 0 || (ddsq < minV))){
              minV = ddsq;
              tvertx = k;
              tvertnx = vnx;
              mindsena = dsena;
            }
          }
          if (countThisOne){
            outsiderInfo oi;
            oi.outsider = outsiders[j];
            oi.dsena = mindsena;
            oi.vertex = tvertx;
            oi.nextVertex = tvertnx;
            oInfo.push_back(oi);
          }
        }
        if (logit){
          tolog(toString(__LINE__) + "\n" + "Line " + toString(i) + " has " + toString(outsiders.size()) + " outsiders.\n");
        }
        sort(oInfo.begin(), oInfo.end(), outsorter);
        vector<point> newpoints;
        UINT currentVertex = 0;
        for (UINT j = 0; j < oInfo.size(); j++){
          for (UINT v = currentVertex; v <= oInfo[j].vertex; v++){
            newpoints.push_back(bl[i][v]);
          }
          UINT prevVertex = oInfo[j].vertex;
          currentVertex = oInfo[j].nextVertex;
          float dsena = oInfo[j].dsena;
          point pt = oInfo[j].outsider;
          if (dsena > pt.radius){
            point v = bl[i][prevVertex];
            point vn = bl[i][currentVertex];
            tangent t(vn.x - v.x, vn.y - v.y);
            point q = t.transformPoint(v, dsena);
            newpoints.push_back(q);
            //pt.flags = pt.flags | RIGHT_SIDE;
            newpoints.push_back(pt);
            newpoints.push_back(q);
          }
        }
        for (UINT k = currentVertex; k < bl[i].size(); k++){
          newpoints.push_back(bl[i][k]);
        }
        bl[i] = newpoints;
      }
      embellishTopology(logit);
    }

    /** \brief Adds group lines
     *
     * \return void
     *
     */
    void addLines(bool logit = false){
      uint8_t mask = (uint8_t) 0x1;
      for (UINT i = 0; i < circles.size(); i++){
        scircles[circles[i].n] = circles[i];
      }
      for (UINT i = 0; i < bl.size(); i++){
        bl[i].clear();
        UINT lm = leftmostCircle(i);
        bl[i].push_back(scircles[lm]);
        point prev = point(scircles[lm].x - 1, scircles[lm].y);
        UINT l = lm;
        groupIterator git(scircles, i, bl.size(), lm);
        UINT tmp = git.nxt();
        if (tmp > 0){
          ccwangle fst = ccwangle(prev, scircles[l], scircles[tmp]);
          UINT counter = 0;
          do{
            groupIterator secgit(scircles, i, bl.size(), 1);
            UINT j = secgit.val();
            //tolog(toString(__LINE__) + "\n" + "Group " + toString(i) + " Starting at " + toString(j) + "\n");
            if (j > 0){
              UINT maxl = tmp;
              while (j > 0){
                if (scircles[j].radius > 0 && j != l){
                  ccwangle scnd = ccwangle(prev, scircles[l], scircles[j]);
                  if (fst < scnd){
                    if (logit){
                      tolog(toString(__LINE__) + "\n" + "---\n");
                      tolog(toString(__LINE__) + "\n" + "Circle: " + toString(j) + "\t" +  "From: " + toString(l) + "\t" +  "Best: " + toString(maxl) + "\n");
                      tolog(toString(__LINE__) + "\n" + "Points1: \n\t" + prev.croack() + "\t" + scircles[l].croack() + "\t" + scircles[tmp].croack() + "\n");
                      tolog(toString(__LINE__) + "\n" + "Points2: \n\t" + prev.croack() + "\t" + scircles[l].croack() + "\t" + scircles[j].croack() + "\n");
                      tolog(toString(__LINE__) + "\n" + fst.croack() + "\t\t");
                      tolog(toString(__LINE__) + "\n" + scnd.croack() + "\n");
                      tolog(toString(__LINE__) + "\n" + "First lower? " + toString(fst < scnd) + ";\t" + "Second lower?: " + toString(scnd < fst) + "\n");
                      tolog(toString(__LINE__) + "\n" + "---\n");
                    }
                    fst = ccwangle(prev, scircles[l], scircles[j]);
                    maxl = j;
                  }
                }
                j = secgit.nxt();
              }
              prev = scircles[l];
              l = maxl;
            }
            else{
              l = lm;
            }
            //if (l == 0){
            //  tolog(toString(__LINE__) + "\n" + "Error\nFirst: " + fst.croack() + "\n"); exit(0);
            //}
            bl[i].push_back(scircles[l]);
            git = groupIterator(scircles, i, bl.size(), l);
            tmp = git.nxt();
            //git.setval(l);
            fst = ccwangle(prev, scircles[l], scircles[tmp]);
            counter++;
          } while (counter < 100 && l != lm && git.val() > 0);
        }
        else{
          bl[i].push_back(scircles[lm]);
        }
        bl[i].pop_back();
        //tolog(toString(__LINE__) + "\n" + "---Group " + toString(i) + ": ");
        //for (UINT k = 0; k < bl[i].size(); k++){
        //  tolog(toString(__LINE__) + "\n" + toString((UINT)bl[i][k].n) + "\t");
        //}
        //tolog(toString(__LINE__) + "\n" + "\n");

      }
      //exit(0);
    }

    /** \brief Take the lines generated with @addLines and set the points to the
     *         outside of each circle.
     *
     * \return void
     *
     */
    void polishLines(){
      for (UINT i = 0; i < bl.size(); i++){
        UINT sz = bl[i].size();
        point prev = bl[i][sz - 1];
        vector<point> newpoints;
        for (UINT j = 0; j < sz; j++){
          point current = bl[i][j];
          point next = bl[i][0];
          if (j < (sz-1)){
            next = bl[i][j+1];
          }
          if (current.radius > 0){
            // Point 1
            float r = current.radius;
            float d = distance(current.x, current.y, prev.x, prev.y);
            if (d > 0){
              float ndx = (current.x - prev.x) * (1 - r/d);
              float ndy = (current.y - prev.y) * (1 - r/d);
              point np;
              np.x = prev.x + ndx;
              np.y = prev.y + ndy;
              newpoints.push_back(np);
              tangent rev(1, 0);
              tangent t1(current.x - prev.x, current.y - prev.y);
              tangent t2(next.x - current.x, next.y - current.y);
              tangent t3 = t1.leftIntermediate(t2);
              tangent rt3 = t3 + rev;
              tangent t4 = t1.leftIntermediate(t3);
              tangent t5 = rt3.leftIntermediate(t2);
              // Point 2
              point tt2 = t4.transformPoint(current, r);
              tt2.flags = setFlag(tt2.flags, DO_NOT_EMBELLISH);

              // Point 3
              point tt3 = t3.transformPoint(current, r);
              tt3.flags = setFlag(tt3.flags, DO_NOT_EMBELLISH);

              // Point 4
              point tt4 = t5.transformPoint(current, r);
              tt4.flags = setFlag(tt4.flags, DO_NOT_EMBELLISH);

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
              newpoints.push_back(np);
            }
          }
          else{
            newpoints.push_back(current);
          }
          prev = current;
        }
        bl[i] = newpoints;
      }
    }

    void swapCoords(UINT i, UINT j){
      point interm = circles[i];
      circles[i].x = circles[j].x;
      circles[i].y = circles[j].y;
      circles[j].x = interm.x;
      circles[j].y = interm.y;
    }

    vector<point> getBoundaries(float air = 0){
      point ul;
      point lr;
      circleIterator ci(circles, circles.size());
      UINT i = ci.val();
      ul.x = circles[i].x; ul.y = circles[i].y;
      lr.x = circles[i].x; lr.y = circles[i].y;
      vector<point> result;
      result.push_back(ul); result.push_back(lr);
      i = ci.nxt();
      while (i > 0){
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
        i= ci.nxt();
      }
      result[0].x -= air;
      result[0].y -= air;
      result[1].x += air;
      result[1].y += air;
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
      if (logit){
        tolog(toString(__LINE__) + "\n" + "Starting with " + toString(bestout) + " outsiders.\n");
      }
      for (UINT i = 0; i < circles.size() - 1; i++){
        if (circles[i].radius > 0){
          for (UINT j = i + 1; j < circles.size(); j++){
            if (circles[j].radius > 0){
              swapCoords(i, j);
              addLines();
              polishLines();
              UINT out = countOutsiders();
              if (out < bestout){
                best = circles;
                bestout = out;
                if (logit){
                  tolog(toString(__LINE__) + "\n" + "i: " + toString(i) + ", " +
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
          }
        }
      }
      // See if getting a circle out is better
      bool goon = true;
      UINT deb = 0;
      do{
        goon = false;
        vector<point> q = getOutsidePoints();
        for (UINT i = 0; i < circles.size(); i++){
          if (circles[i].radius > 0){
            point t; t.x = circles[i].x; t.y = circles[i].y;
            UINT j = 0;
            circles[i].x = q[j].x;
            circles[i].y = q[j].y;
            addLines();
            polishLines();
            UINT out = countOutsiders();
            if (logit){
              tolog(toString(__LINE__) + "\n" + toString(out) + "-" + toString(bestout) + "\n");
            }
            if (out < bestout){
                bestout = out;
                goon = true;
                circles[i].flags = circles[i].flags | TAKEN_OUT;
                setScale(circles[i]);
                if (logit){
                  tolog(toString(__LINE__) + "\n" + "i: " + toString(i) + ", " +
                        "j: " + toString(j) + ", " + "outer: " + toString(out) + "\n");
                  tolog(toString(__LINE__) + "\n" + "Change : \n" + t.croack() + circles[i].croack());
                }
            }
            else{
              //tolog(toString(__LINE__) + "\n" + toString(i) + " with " + toString(j) + " was no better\n");
              circles[i].flags = circles[i].flags & (~TAKEN_OUT);
              circles[i].x = t.x;
              circles[i].y = t.y;
            }
          }
        }
      } while (goon);
      return bestout;
    }

    void chooseCrossings(bool logit = false){
      addLines();
      polishLines();
      UINT bestcross = countCrossings();
      UINT bestout = countOutsiders();
      for (UINT i = 0; i < circles.size(); i++){
        UINT n = circles[i].n;
        while ((n > 0) && ((n & 0x1) == 0)){
          n = n >> 1;
        }
        if (n == 1){
          vector<point> q = getOutsidePoints();
          if (logit){
            tolog(toString(__LINE__) + "\n" + "Checking on circle " + circles[i].croack());
          }
          for (UINT j = 0; j < q.size(); j++){
            point u; u.x = circles[i].x; u.y = circles[i].y;
            circles[i].x = q[j].x;
            circles[i].y = q[j].y;
            addLines();
            polishLines();
            UINT newcross = countCrossings();
            UINT newout = countOutsiders();
            if ((newout <= bestout) && (newcross < bestcross)){
              if (logit){
                tolog(toString(__LINE__) + "\n" + "Better newcross with circle " + circles[i].croack());
                tolog(toString(__LINE__) + "\n" + toString(newout) + " outs from " + toString(bestout) + "\n");
                tolog(toString(__LINE__) + "\n" + toString(newcross) + " crosses\n");
              }
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
        bl_secure = bl;
        ncycles_secure = blSettings.ncycles;
        resetOld();
        circles_secure = circles;
      }
    }

    void restoreSecureState(){
      bl = bl_secure;
      circles = circles_secure;
      deciderCounter = 0;
      keepDistCounter = 0;
      blSettings.ncycles = ncycles_secure;
    }

    void setPrevState(){
      bl_old10 = bl;
      ncyles_old10 = blSettings.ncycles;
      resetOld();
      circles_old10 = circles;
    }

    void restorePrevState(){
      bl = bl_old10;
      circles = circles_old10;
      deciderCounter = 0;
      keepDistCounter = 0;
      blSettings.ncycles = ncyles_old10;
    }

    point contact(point &p0, point &p1, float hardness = 5e1f, float radius = 0 )
    {
      if (hardness == 0){
        hardness = 5e1f;
      }
        point result;
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
        float trueRadius = p0.radius + p1.radius;
        if (radius == 0){
          radius = (trueRadius) * 1.2;
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

    void setForcesFirstStep(){
      UINT i, j;
      UINT size;
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
      UINT size;
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
                  f = contact(circles[i], circles[j], 0, radius);
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


    void solve(bool resetVelocity = false, bool breakOnTopol = false)
    {
        UINT i;
        float kb = blSettings.baseBV;
        dataDisplay.clear();

        //Init the scale for the new frame
        internalScale.setClear(true);
        updPos(kb, resetVelocity);
        clearForces();
//Show dt
        displayFloat("DT", blSettings.dt);
        displayUINT("CYCLES", blSettings.ncycles);
        displayFloat("NPOINTSMIN", nPointsMin);
        displayUINT("NPOINTS", bl[0].size());
        displayFloat("UNST", udt.unstabledt());
        displayUINT("COUNT", udt.counter());
        displayUINT("BACKCOUNT", udt.backcounter());
        displayFloat("POTENTIAL", potential);

        potential = 0;

        for (UINT j = 0; j < circles.size(); j++){
          circles[j].flags = unsetFlag(circles[j].flags, IS_OUTSIDE);
        }
        if (blSettings.doCheckTopol == true && (checkTopol()))
        {
          for (UINT j = 0; j < circles.size(); j++){
            //circles[j].resetv();
          }
            if (breakOnTopol){
              //writeSVG("error.svg");
              tolog("Break on topol\n");
              for (UINT j = 0; j < circles.size(); j++){
                if (circles[j].radius > 0){
                  point P = circles[j];
                  vector<int> b = toBin(circles[j].n, bl.size());
                  for (UINT i = 0; i < bl.size(); i++){
                    bool incorrect = circleTopol(P, b, i);
                    if (incorrect){
                      tolog("Circle " + toString(circles[j].n) + " is incorrect with line " + toString(i) + "\n");
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
          }
          if (deciderCounter.isMax())
          {
              udt.poke();
          }
          if (keepDistCounter.isMax()){
              setSecureState();
              keepDist(avgStartDist);
              if (checkTopol()){
                restorePrevState();
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
                  if (bl[i][j].softenVel == true){
                    limitVel(bl[i][j], blSettings.maxv);
                    bl[i][j].softenVel = false;
                  }
                  //

                  bl[i][j].vx += bl[i][j].fx * blSettings.dt / bl[i][j].mass;
                  bl[i][j].vy += bl[i][j].fy * blSettings.dt / bl[i][j].mass;
                  blSettings.totalLineV += bl[i][j].vx * bl[i][j].vx + bl[i][j].vy * bl[i][j].vy;

                  bl[i][j].x += bl[i][j].vx * blSettings.dt;
                  bl[i][j].y += bl[i][j].vy * blSettings.dt;
                  /*******/
                  //attention(bl[i][j].x, bl[i][j].y);
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
              //dataDisplay.push_back(t);
          }
        }
        for (i = 0; i < circles.size(); i++)
        {
          if (circles[i].radius > 0){
              if (isNAN(circles[i].fx)){
                writeSVG();
                exit(0);
              }
              //limitForce(circles[i], blSettings.maxf);

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
              if (circles[i].softenVel == true){
                //limitVel(circle[i], blSettings.maxv);
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
              setScale(circles[i]);
          }
        }
        displayFloat("LINEV", blSettings.totalLineV);
        displayFloat("MINRAT", minRat);
        displayFloat("CIRCLEV", blSettings.totalCircleV);
        displayFloat("SURFRATIO", estSurf());
        blSettings.totalCircleV = 0;
        blSettings.totalLineV = 0;
    }

    float estSurf(int nPoints = 100){
      float tsurf = (float) (internalScale.xSpan() * internalScale.ySpan());
      tsurf /= (float) totalExpectedSurface;
      return tsurf;
    }

    bool circleTopol(point P, vector<int> belong, UINT j){
      int lastPoint;
      bool mustBeIn = (belong[j] == 1);  //Must the circle be inside the curve?
      bool isIn = false;      //Is the circle inside the curve?
      vector<point> bnd = getBoundaries();
      float xmax = max(bnd[0].x, bnd[1].x) + 2 * maxRad() +  1;
      point p3 = P;
      point p4;
      p4.x = xmax;
      p4.y = P.y;
      for (UINT i = 0; i < bl[j].size(); i++){
        point p1 = bl[j][i];
        point p2 = bl[j][0];
        if (i < (bl[j].size() - 1)){
          p2 = bl[j][i+1];
        }
        if (cross(p1, p2, p3, p4)){
          isIn = !isIn;
        }
      }
      if (isIn ^ mustBeIn)
      {
          //attention(circles[j].x, circles[j].y);
          //Sleep(1000);
          //tolog(toString(__LINE__) + "\n" + "Line " + toString(j) + ", " + P.croack() + "isIn: " + toString(isIn) + " mustBeIn: " + toString(mustBeIn) + "\n\n");
          return true;
      }
      return false;
    }


    bool isTopolCorrect(point P, vector<int> belong){
      if (!(P.radius > 0)){
        tolog(toString(__LINE__) + "\n" + "Called isTopoloCorrect with radius 0\n"); exit(1);
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
      UINT j = circles.size();
      for (i = 0; i < circles.size(); i++)
      {
          if (circles[i].radius > 0)
          { /* Circle has radius */
              vector<int> belong = toBin(circles[i].n, bl.size());
              bool result = isTopolCorrect(circles[i], belong);
              if (result){
                return result;
              }
          } /* Circle has radius */
      }
      return false;
    }

    /** \brief Do segments p1-p2 and p3-p4 cross?
     *
     * \param p1 point
     * \param p2 point
     * \param p3 point
     * \param p4 point
     * \return bool true if segments cross.
     *
     */
    bool cross(point p1, point p2, point p3, point p4){
      bool result = false;
      if (p1 == p3 || p2 == p3 || p2 == p4 || p1 == p4){
        return true;
      }
      tangent t1 = tangent(p1, p3);
      tangent t2 = tangent(p3, p2);
      tangent t3 = tangent(p2, p4);
      tangent t4 = tangent(p4, p1);
      t2.rotate(t1);
      t3.rotate(t1);
      t4.rotate(t1);
      tangent tz = tangent(-1, 0);
      tangent tr = tangent(1, 0);
      if (t2 != tz && t4 != tr &&
          t4 != tz && t2 != tr){
        if (((t2 <= t3) && (t3 <= t4)) ||
            ((t3 <= t2) && (t4 <= t3))){
          result = true;
        }
      }
      return result;
    }

    UINT countCrossings(){
      UINT result = 0;
      for (UINT i = 0; i < bl.size()-1; i++){
        for (UINT ii = 0; ii < bl[i].size(); ii++){
          point p1 = bl[i][ii];
          point p2 = bl[i][0];
          if (ii < (bl[i].size() - 1)){
            p2 = bl[i][ii+1];
          }
          for (UINT j = i + 1; j < bl.size(); j++){
            for (UINT jj = 0; jj < bl[j].size(); jj++){
              point p3 = bl[j][jj];
              point p4 = bl[j][0];
              if (jj < (bl[j].size() - 1)){
                p4 = bl[j][jj+1];
              }
              if (cross(p1, p2, p3, p4)){
                result++;
              }
            }
          }
        }
      }

      return result;
    }

    UINT countOutsiders(){
      UINT result = 0;
      UINT j = circles.size();
      //tolog(toString(__LINE__) + "\n" + "---\n");
      for (UINT i = 0; i < circles.size(); i++)
      {
          if (circles[i].radius > 0)
          { /* Circle has radius */
              vector<int> belong = toBin(circles[i].n, bl.size());
              bool incorrect = isTopolCorrect(circles[i], belong);
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
  UINT k;
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
        initBlData(&blSettings);
        minRat = 0;
        showThis = false;
        blSettings.signalEnd = false;
        blSettings.contacts = 0;
        blSettings.fixCircles = false;
        blSettings.minratio = 0.1f * (ngroups * ngroups * ngroups)/ (4 * 4 * 4);
        blSettings.marginScale = 0.02f;
        blSettings.totalCircleV = 0;
        blSettings.totalLineV   = 0;
        blSettings.minSurfRatio = 0;
        blSettings.maxf = 5e-2f;
        blSettings.maxv = 1e-0f;
        blSettings.margin = 1.2 * ngroups * blSettings.marginScale;
        blSettings.startdt = blSettings.dt;
        blSettings.stepdt = 0.6f;
        blSettings.inputFile = inputFile;
        blSettings.fname = outputFile;
        blSettings.ncycles = 0;
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
        blCounter.setLimits(0, 5u);
        deciderCounter.setLimits(0, 5u);
        keepDistCounter.setLimits(0, 100u);
        refreshScreen.setLimits(1, 50);

        //init internal scale
        internalScale.setClear(true);

        //init time parameters
        udt.init(blSettings.startdt, blSettings.stepdt);

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

        setPrevState();
        setSecureState();

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
            bl_secure.clear();
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
                    point p;
                    p.x = x; p.y = y;
                    setScale(p);
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
      for (i = 1; i < circles.size(); i++){
        //printf("%d\n", i);
        if (circles[i].radius > 0){
          svgtemp = place(sc, circles[i]);
          //printf("%.4f, %.4f, %.4f\n", svgtemp.x, sc.minX, sc.maxX);
          if (svgtemp.x > sc.minX() && svgtemp.x < sc.maxX()){
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
      int l;
      float cx = 500.0f;
      float cy = 50.0f;
      float rw = 30.0f;
      float rh = 15.0f;
      float dy = 40.0f;
      float dx = 40.0f;
      for (l = 0; l < ngroups; l++){
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
      UINT i, j;
      float perim;
      float segment;
      float minDist10 = minDist / 100;
      point startPoint;
      point endPoint;
      point tempPoint;
      vector<point> tempv;
      vector<vector<point> > tempbl;
      for (i = 0; i < bl.size(); i++)
      {
        UINT bls = (UINT) bl[i].size();
          perim = perimeter(bl[i], true);
          startPoint = bl[i][bl[i].size()-1];
          for (j = 0; j < bls; j++)
          {
              endPoint = bl[i][j];
              segment = distance(startPoint.x, startPoint.y,
                                 endPoint.x, endPoint.y);
              if ((bls <= nPointsMin && segment > minDist10) || segment > minDist){
                tempPoint.x = endPoint.x;
                tempPoint.y = endPoint.y;
                tempPoint.mass = POINT_MASS;
                tempv.push_back(tempPoint);
                startPoint = endPoint;
              }

          }
          tempbl.push_back(tempv);
          tempv.clear();
      }
      bl.clear();
      bl = tempbl;
    }

    void interpolateToDist(float pointDist){
      if (pointDist <= 0){
        pointDist = 1;
      }

      for (UINT i = 0; i < bl.size(); i++){
        vector<point>tempbl;
        for(UINT j = 0; j < bl[i].size(); j++){
          point current = bl[i][j];
          point nxt = bl[i][0];
          if (j < (bl[i].size() - 1)){
            nxt = bl[i][j+1];
          }
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
    }

    void writeSVG(string fname = ""){
        fileText tmp = toSVG();
        ofstream result;
        if (fname == ""){
          fname = blSettings.fname.c_str();
        }
        result.open(fname);
        result.write(tmp.getText().c_str(), tmp.getText().size());
        result.close();

    }

    void simulate(int maxRel = 0)
    {
        UINT i;
        UINT it1 = (UINT) 7e3;
        UINT it2 = (UINT) 2e2;
        point minP;
        point maxP;
        printf("Starting...\n");

        // This loop is limited to maxRunningTime
        time_t start = time(NULL);
        if (blSettings.ncyclesInterrupted >= it1) blSettings.ncyclesInterrupted = 0;
        for (i = blSettings.ncyclesInterrupted; i < it1; i++){
          //setForces1();
          setForcesFirstStep();
          blSettings.doCheckTopol = false;
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
          setContacts(false);
          solve();
          //cout << "x - " << circles[2].x << "\n";
        }
        /*
        setAsStable();
        for (i = 0; i < 50; i++){
          setForces1();

          if (refreshScreen.isMax() == true){
            writeSVG();
          }
          refreshScreen++;
          setContacts();
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
          setContacts();
          solve(true);
        }
        */
        //setForces3();
        UINT counter;
        dataDisplay.clear();
    }

};


#endif // TOPOL_H_INCLUDED
