#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <vector>
#include <fstream>
#include <sstream>
#include "debug.h"
using namespace std;

#define CIRCLE_MASS 200.0f
#define POINT_MASS 20

#define INGRAVID 0x20

typedef unsigned int UINT;

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
    point clone(){
      point result;
      result.x = x;
      result.y = y;
      result.cancelForce = cancelForce;
      result.flags = flags;
      result.fx = fx;
      result.fy = fy;
      result.inContact = inContact;
      result.mass = mass;
      result.n = n;
      result.orig = orig;
      return result;
    }
    string croack(){
      ostringstream r;
      r << "X: " << x << ", Y: " << y << "\tN: " << n << "\tradius: " << radius;
      r << "\tvx: " << vx << "\tvy: " << vy << "\tFx: " << fx << "\tFy: " << fy << "\n";
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

typedef struct springLink{
  UINT from;
  UINT to;
  float k;
  float d;
} springLink;



class scene{

  vector<point*> points;
  vector<point> shadowPoints;
  vector<vector<point>> gcopies;
  vector<point> virtualPoints;
  vector<springLink> springs;
  vector<springLink> rods;
  vector<string> info;
  float dt;
  float simtime;
  float defaultK;
  float maxK;
  float maxAllowedForce;
  float maxAllowedVel;
  float rodStiffness;
  float friction;
  float G; // Gravity constant
  bool dump;

  void clearForces(){
    for (UINT i = 0; i < points.size(); i++){
      point *p = points[i];
      p->fx = 0;
      p->fy = 0;
    }
  }

  void effectGravity(float dt){
    if (G != 0){
      for (UINT i = 0; i < (points.size() - 1); i++){
        point *p0 = points[i];
        if ((p0->flags & INGRAVID) == 0){
          for (UINT j = i + 1; j < points.size(); j++){
            point *p1 = points[j];
            if ((p1->flags * INGRAVID == 0)){
              float dx = p1->x - p0->x;
              float dy = p1->y - p0->y;
              float d = sqrt(dx * dx + dy * dy);
              if (d > 0){
                point result;
                result.fx = G * p0->mass * p1->mass * dx / (d * d *d);
                result.fy = G * p0->mass * p1->mass * dy / (d * d *d);
                p0->fx += result.fx;
                p1->fx -= result.fx;
                p0->fy += result.fy;
                p1->fy -= result.fy;
              }
            }
          }
        }
      }
    }
  }


  void effectSprings(float dt){
    for (UINT i = 0; i < springs.size(); i++){
      point result;
      float springK = springs[i].k;
      if (springK == 0){
        springK = defaultK;
      }
      float eqd = springs[i].d;
      point *p0 = points[springs[i].from];
      point *p1 = points[springs[i].to];
      float dx = p1->x - p0->x;
      float dy = p1->y - p0->y;
      float d = sqrt(dx * dx + dy * dy);
      if (d == 0){
        tolog("Distance is zero: " + p0->croack() + p1->croack());
        d = 1e-4;
      }
      else{
        float eqx = eqd * dx / d;
        float eqy = eqd * dy / d;
        result.fx = springK * (dx - eqx);
        result.fy = springK * (dy - eqy);
        float fx1 = result.fx;
        float fy1 = result.fy;
        float fx2 = result.fx;
        float fy2 = result.fy;
        if (p0->fx != p0->fx){
            tolog("Distance is still zero: " + p0->croack() + p1->croack());
            exit(0);
        }
        p0->fx += fx1;
        p1->fx -= fx2;
        p0->fy += fy1;
        p1->fy -= fy2;
      }
    }
  }
  void rod(point *p0, point *p1, float eqd, int neg = 1){
    point result;
    float springK = rodStiffness;
    float dx = p1->x - p0->x;
    float dy = p1->y - p0->y;
    float dsq = dx * dx + dy * dy;
    float d = sqrt(dsq);
    if (d != 0){
      float f01 = (p0->fx * dx + p0->fy * dy) / dsq;
      float f10 = (p1->fx * dx + p1->fy * dy) / dsq;
      p0->fx -= f01 * dx;
      p1->fx -= f10 * dx;
      p0->fy -= f01 * dy;
      p1->fy -= f10 * dy;
      // Adjust with a spring
      float eqx = eqd * dx / d;
      float eqy = eqd * dy / d;
      result.fx = springK * (dx - eqx);
      result.fy = springK * (dy - eqy);
      p0->fx += neg * result.fx;
      p1->fx -= neg * result.fx;
      p0->fy += neg * result.fy;
      p1->fy -= neg * result.fy;
    }
  }

  void effectRods(float dt){
    for (UINT i = 0; i < rods.size(); i++){
      point *p0 = points[rods[i].from];
      point *p1 = points[rods[i].to];
      rod(p0, p1, rods[i].d);
    }
  }
  bool isInside(point *p0, point *p1, point *target)
  {
      /**
      Returns true if target is inside the box limited
      by p0 and p1. False otherwise
      **/

      bool result = true;
      float xdiscr, ydiscr;
      xdiscr = (target->x - p0->x) * (target->x - p1->x);
      ydiscr = (target->y - p0->y) * (target->y - p1->y);
      if (xdiscr > 0 || ydiscr > 0)
      {
          result = false;
      }
      return result;
  }
  void rope(point *circ, UINT nspring, point *p0, point *p1, int neg = 1)
    {
      if (circ->radius == 0){
        return;
      }
      float rght = circ->x + circ->radius;
      float lft  = circ->x - circ->radius;
      float top  = circ->y - circ->radius;
      float bttm = circ->y + circ->radius;
      if (((p0->x <= lft && p1->x >= lft)  ||
           (p1->x <= lft && p0->x >= lft)  ||
          (p0->x <= rght && p1->x >= rght) ||
          (p1->x <= rght && p0->x >= rght)) ||
          ((p0->y <= top && p1->y >= top)  ||
           (p1->y <= top && p0->y >= top)  ||
          (p0->y <= bttm && p1->y >= bttm) ||
          (p1->y <= bttm && p0->y >= bttm))){
        point virt;
        if (p1->x == p0->x){
          virt.x = p0->x;
          virt.y = circ->y;
        }
        else{
          float a = (p1->y - p0->y)/(p1->x - p0->x);
          float b = p0->y - a * p0->x;
          virt.x = ((circ->x + a*circ->y - a * b)/(a*a + 1));
          virt.y = (a * circ->x + a*a*circ->y + b)/(a*a + 1);
        }
        if (isInside(p0, p1, &virt)){
          float d0 = distance(p0->x, p0->y, virt.x, virt.y) /
                   distance(p0->x, p0->y, p1->x, p1->y);
          float d1 = distance(p1->x, p1->y, virt.x, virt.y) /
                     distance(p0->x, p0->y, p1->x, p1->y);
          float td = d0 + d1;
          float rd = d0 / td;
          virt.vx = p0->vx + (p1->vx-p0->vx) * rd;
          virt.fx = p0->fx + (p1->fx-p0->fx) * rd;
          virt.vy = p0->vy + (p1->vy-p0->vy) * rd;
          virt.fy = p0->fy + (p1->fy-p0->fy) * rd;
          virt.mass = (p0->mass + p1->mass) / 2;
          virt.n = nspring;
          float dx = virt.x - circ->x;
          float dy = virt.y - circ->y;
          float dsq = dx * dx + dy * dy;
          float rsq = circ->radius * circ->radius;
          if (dsq <= rsq){
            //dumpthis();
            contact(circ, &virt);
            virtualPoints.push_back(virt);
          }
        }
    }
  }
  UINT convertLinkCoordinates(UINT i, UINT j) {
    UINT a = i < j ? i : j;
    UINT b = i < j ? j : i;
    UINT start = 0;
    UINT k = points.size();
    UINT t = k - 1;
    UINT q = k - a - 1;
    if (a > 0) {
      start = ((t * (t + 1)) - (q * (q + 1))) / 2;
    }
    UINT result = start + b - a - 1;
    return result;
  }
  point contact(point *p0, point *p1){
    float r = p0->radius + p1->radius;
    float rsq = r * r;
    float dx = p1->x - p0->x;
    float dy = p1->y - p0->y;
    float dsq = dx * dx + dy * dy;
    if (dsq <= rsq){
      rod(p0, p1, r);
    }
  }

  void isContact(point *p0, point *p1){
    float r = p0->radius + p1->radius;
    if (r == 0){
      return;
    }
    float rsq = r * r;
    float dx = p1->x - p0->x;
    float dy = p1->y - p0->y;
    float dsq = dx * dx + dy * dy;
    if (dsq <= rsq){
      rod(p0, p1, r, -1);
      float fx = p0->fx - p1->fx;
      float fy = p0->fy - p1->fy;
      float scprod = fx * dx + fy * dy;
      if (scprod > 0){
        rod(p0, p1, r);
        float vx = (p0->mass * p0->vx + p1->mass * p1->vx) / (p0->mass + p1->mass);
        float vy = (p0->mass * p0->vy + p1->mass * p1->vy) / (p0->mass + p1->mass);
        p0->vx = vx; p0->vy = vy;
        p1->vx = vx; p1->vy = vy;
      }
    }
  }


  void icontacts(){
    // First pass
    for (UINT i = 0; i < (points.size()-1); i++){
      point *p0 = points[i];
      for (UINT j = i + 1; j < points.size(); j++){
        point *p1 = points[j];
        if (p0->radius != 0 || p1->radius != 0){
          contact(p0, p1);
        }
      }
    }
    for (UINT i = 0; i < springs.size(); i++){
      UINT f = springs[i].from;
      UINT t = springs[i].to;
      point *a = points[f];
      point *b = points[t];
      for (UINT j = 0; j < points.size(); j++){
        if (j != f && j != t){
          point *c = points[j];
          if (c->radius > 0){
            rope(c, i, a, b);
          }
        }
      }
    }
    // Second pass
    for (UINT i = 0; i < (points.size()-1); i++){
      point *p0 = points[i];
      for (UINT j = i + 1; j < points.size(); j++){
        point *p1 = points[j];
        isContact(p0, p1);
      }
    }
    for (UINT i = 0; i < virtualPoints.size(); i++){
      point *virt = &(virtualPoints[i]);
      UINT f = springs[virt->n].from;
      UINT t = springs[virt->n].to;
      point *p0 = points[f];
      point *p1 = points[t];
      for (UINT j = 0; j < points.size(); j++){
        if (j != f && j != t){
          point *circ = points[j];
          float d = distance(circ->x, circ->y, virt->x, virt->y);
          float r = circ->radius + virt->radius;
          if (d <= r){
            isContact(circ, virt);
            float d0 = distance(p0->x, p0->y, virt->x, virt->y) /
                       distance(p0->x, p0->y, p1->x, p1->y);
            float d1 = distance(p1->x, p1->y, virt->x, virt->y) /
                       distance(p0->x, p0->y, p1->x, p1->y);
            float td = d0 + d1;
            float rd = d0 / td;
            p0->fx += d1 * virt->fx * rd;
            p0->fy += d1 * virt->fy * rd;
            p1->fx += d0 * virt->fx * rd;
            p1->fy += d0 * virt->fy * rd;
            //dumpthis();
          }
        }
      }
    }
    if (virtualPoints.size() > 0 && !dump){
      virtualPoints.clear();
    }
  }
  void update(float cdt){
    float b = friction;
    info.clear();
    float fsq = 0;
    float netvx = 0;
    float netvy = 0;
    for (UINT i = 0; i < points.size(); i++){
      point *p = points[i];
      fsq += p->fx * p->fx + p->fy + p->fy;
      netvx += p->vx;
      netvy += p->vy;
      float fx = p->fx;
      float fy = p->fy;
      fx -= b * p->vx;
      fy -= b * p->vy;
      float ax = fx / p->mass;
      float ay = fy / p->mass;
      p->vx += ax * cdt;
      p->vy += ay * cdt;
      float deltax = p->vx * cdt;
      float deltay = p->vy * cdt;
      p->x += deltax;
      p->y += deltay;
    }
    addInfo("FSQ: ", fsq);
    addInfo("NETVX: ", netvx);
    addInfo("NETVY: ", netvy);
    addInfo("K: ", defaultK);
  }
public:
  scene(){
    clearScene();
    dump = false;
    simtime = 0;
    defaultK = 100;
    G = 0;
    friction = 0.0f;
    maxAllowedForce = 1e5;
    maxAllowedVel = 5e2;
    rodStiffness = 1e5;
    maxK = defaultK;
    dt = 1e-2;
  }
  void clearScene(){
    points.clear();
    shadowPoints.clear();
    springs.clear();
    virtualPoints.clear();
    rods.clear();
    info.clear();
  }
  template<typename T, typename T2>
  void addInfo(T input, T2 ninput){
      ostringstream result;
      result << input;
      result << ninput;
      string td = result.str();
      info.push_back(td);
  }
  void setFriction(float coefficient = 50){
    friction = coefficient;
  }
  void setG(float gravity = 0){
    G = gravity;
  }
  bool dumpme(){
    return dump;
  }
  void dumpthis(){
    dump = true;
  }
  vector<string> getInfo(){
    vector<string> result = info;
    return result;
  }
  vector<point> getVirtual(){
    vector<point> result = virtualPoints;
    return result;
  }
  void clearVirtual(){
    virtualPoints.clear();
  }
  void addPoint(point p){
    shadowPoints.push_back(p);
    points.clear();
    for (UINT i = 0; i < shadowPoints.size(); i++){
      point *pt = &(shadowPoints[i]);
      points.push_back(pt);
    }
  }
  void addPointP(point *p){
    shadowPoints.push_back(*p);
    points.push_back(p);
  }
  void doSomething(){
    points[0]->x += 10;
  }

  /** \brief Saves the coordinates of all points.
   * Recover with \ref grestore()
   * \return void
   *
   */
  void gsave(){
    vector<point> cp;
    for (UINT i = 0; i < points.size(); i++){
      point t = *(points[i]);
      cp.push_back(t);
    }
    gcopies.push_back(cp);
  }

  /** \brief Recovers coordinates of all points.
   * Coordinates must have been saved with \ref gsave().
   * Velocities and forces are reset to zero.
   * \return void
   *
   */
  void grestore(){
    vector<point> cp = gcopies[gcopies.size() - 1];
    gcopies.pop_back();
    for (UINT i = 0; i < cp.size(); i++){
      points[i]->x = cp[i].x;
      points[i]->y = cp[i].y;
      points[i]->vx = 0;
      points[i]->vy = 0;
      points[i]->fx = 0;
      points[i]->fy = 0;
    }
  }

  void saveScene(string fname="scene.unv"){
    ofstream result;
    result.open(fname);
    result << "Points" << endl;
    result << points.size() << endl;
    for (UINT i = 0; i < points.size(); i++){
      result << points[i]->x << "\t" << points[i]->y << "\t" << points[i]->radius << endl;
    }
    result << "Springs" << endl;
    result << springs.size() << endl;
    for (UINT i = 0; i < springs.size(); i++){
      result << springs[i].from << "\t";
      result << springs[i].to << "\t";
      result << springs[i].k << "\t";
      result << springs[i].d << endl;
    }
    result.close();
  }
  void loadScene(string fname = "scene.unv"){
    clearScene();
    ifstream fl;
    fl.open(fname);
    string hd;
    getline(fl, hd);
    getline(fl, hd);
    UINT npts = atoi(hd.c_str());
    char delimiter = '\t';
    for (UINT i = 0; i < npts; i++){
      getline(fl, hd);
      stringstream c(hd);
      string token;
      vector<string> tokens;
      while (getline(c, token, delimiter)) {
        tokens.push_back(token);
      }
      point p;
      p.x = atof(tokens[0].c_str());
      p.y = atof(tokens[1].c_str());
      p.radius = atof(tokens[2].c_str());
      addPoint(p);
    }
    getline(fl, hd);
    getline(fl, hd);
    UINT nspr = atoi(hd.c_str());
    for (UINT i = 0; i < nspr; i++){
      getline(fl, hd);
      stringstream c(hd);
      string token;
      vector<string> tokens;
      while (getline(c, token, delimiter)) {
        tokens.push_back(token);
      }
      springLink sl;
      sl.from = atoi(tokens[0].c_str());
      sl.to = atoi(tokens[1].c_str());
      sl.k = atof(tokens[2].c_str());
      sl.d = atof(tokens[3].c_str());
      addLink(sl.from, sl.to, sl.k, sl.d);
    }
    fl.close();
  }
  bool addLink(UINT from, UINT to, float k = 0, float d = 0){
    bool result = false;
    if (from != to &&
        from >= 0 && from < points.size() &&
        to >= 0 && to < points.size()){
      result = true;
      springLink sl;
      sl.from = from;
      sl.to = to;
      sl.k = k;
      sl.d = d;
      springs.push_back(sl);
    }
    return result;
  }
  bool addRod(UINT from, UINT to, float d = 0){
    bool result = false;
    if (from != to &&
        from >= 0 && from < points.size() &&
        to >= 0 && to < points.size()){
      result = true;
      springLink sl;
      sl.from = from;
      sl.to = to;
      sl.k = 0;
      float dx = points[to]->x - points[from]->x;
      float dy = points[to]->y - points[from]->y;
      sl.d = d * d;
      if (d == 0){
        sl.d = sqrt(dx * dx + dy * dy);
      }
      rods.push_back(sl);
    }
    return result;
  }
  vector<point*> getPoints(){
    return points;
  }
  vector<springLink> getLinks(){
    return springs;
  }
  vector<springLink> getRods(){
    return rods;
  }
  float solve(float cdt = 0){
    if (cdt == 0){
      cdt = dt;
    }
    clearForces();
    effectSprings(cdt);
    effectRods(cdt);
    effectGravity(cdt);
    icontacts();
    float maxfsq = 0;
    float maxvsq = 0;
    for (UINT i = 0; i < points.size(); i++){
      float fx = points[i]->fx;
      float fy = points[i]->fy;
      float vx = points[i]->vx;
      float vy = points[i]->vy;
      float fsq = fx * fx + fy * fy;
      float vsq = vx * vx + vy * vy;
      if (fsq != fsq){

      }
      if (fsq > maxfsq){
        maxfsq = fsq;
      }
      if (vsq > maxvsq){
        maxvsq = vsq;
      }
    }
    //tolog(toString(maxfsq) + "\t" + toString(maxvsq) + "\n");
    //if ((maxfsq > maxAllowedForce * (points.size())) || (maxvsq > maxAllowedVel)){
    //  defaultK *= maxAllowedForce * points.size() / maxfsq;
    //}
//    else{
//      defaultK = maxK * points.size();
//    }
    update(cdt);
    for (UINT i = 0; i < 10; i++){
      clearForces();
      icontacts();
      update(cdt);
    }
    addInfo("DT: ", cdt);
    simtime += cdt;
    addInfo("ST: ", simtime);
    return cdt;
  }
  float simTime(){
    return simtime;
  }
  string croack(){
    ostringstream result;
    result << "Points: " << endl;
    for (UINT i = 0; i < points.size(); i++){
        result << points[i]->croack();
    }
    result << "Virtual points: " << endl;
    for (UINT i = 0; i < virtualPoints.size(); i++){
        point vp = virtualPoints[i];
        result << vp.croack();
    }
    result << "Links: " << endl;
    for (UINT i = 0; i < springs.size(); i++){
      result << i + 1 << ": From " << springs[i].from << " to " << springs[i].to << endl;
      result << "\tk: " << springs[i].k << ", d0: " << springs[i].d << endl;
    }
    result << "Rods: " << endl;
    for (UINT i = 0; i < rods.size(); i++){
      result << i + 1 << ": From " << rods[i].from << " to " << rods[i].to << endl;
      result << "\tk: " << rods[i].k << ", d0: " << rods[i].d << endl;
    }
    return result.str();
  }
};


#endif // SCENE_H_INCLUDED
