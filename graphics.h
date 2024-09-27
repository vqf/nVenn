#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED
#include <gl/gl.h>
#include <gl/glu.h>
#include "bmpfont.h"
//#include <windows.h>

bool showContacts = false;

class glGraphics{
  public:

  vector<point> glCircle(float x, float y, float r)
  {
    borderLine dummy;
      point temp;
      vector<point> result;
      float sen = 0.7071067811f;
      temp.x = x;
      temp.y = y + r;
      result.insert(result.end(), temp);
      temp.x = x + sen * r;
      temp.y = y + sen * r;
      result.insert(result.end(), temp);
      temp.x = x + r;
      temp.y = y;
      result.insert(result.end(), temp);
      temp.x = x + sen * r;
      temp.y = y - sen * r;
      result.insert(result.end(), temp);
      temp.x = x ;
      temp.y = y - r;
      result.insert(result.end(), temp);
      temp.x = x - sen * r;
      temp.y = y - sen * r;
      result.insert(result.end(), temp);
      temp.x = x - r;
      temp.y = y;
      result.insert(result.end(), temp);
      temp.x = x - sen * r;
      temp.y = y + sen * r;
      result.insert(result.end(), temp);
      return result;
  }

  point showForce(point p, float sc = 1){
    borderLine dummy;
    point r;
    r.x = p.x + p.fx * sc;
    r.y = p.y + p.fy * sc;
    return r;
  }

  void addRectangle(borderLine bl, scale ogl, vector<point> p, rgb color = {0.5, 0.5, 0.5}){
    glBegin (GL_LINE_LOOP);
    glColor3f(color.red, color.green, color.blue);
    point p0;
    p0.x = p[0].x;
    p0.y = p[0].y;
    point mp = bl.place(ogl, p0);
    glVertex2d(mp.x, mp.y);
    p0.x = p[1].x;
    p0.y = p[0].y;
    mp = bl.place(ogl, p0);
    glVertex2d(mp.x, mp.y);
    p0.x = p[1].x;
    p0.y = p[1].y;
    mp = bl.place(ogl, p0);
    glVertex2d(mp.x, mp.y);
    p0.x = p[0].x;
    p0.y = p[1].y;
    mp = bl.place(ogl, p0);
    glVertex2d(mp.x, mp.y);
    p0.x = p[0].x;
    p0.y = p[0].y;
    mp = bl.place(ogl, p0);
    glVertex2d(mp.x, mp.y);
    glEnd();
    return;
  }

  void wait(){
    bool bQuit = false;
    MSG msg;

    while (!bQuit){
      if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
      {

          if (msg.message == WM_QUIT)
          {
              bQuit = TRUE;
          }
          {
              TranslateMessage (&msg);
              DispatchMessage (&msg);
          }
      }
    }
  }

  void toOGL(borderLine bl, HDC hDC)
  {
      UINT i, j;
      point P;     //coordinates
      vector<point> temp; //stores perimeters
      vector<vector<point> > blp = bl.bl;
      bool showPoints = bl.showThis;
      glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
      glClear (GL_COLOR_BUFFER_BIT);
      //define openGL scale

      scale ogl;
      //addRectangle(bl, ogl, bl.getBoundaries(2 * bl.maxRad()));
      ogl.initScale();
      //define vectors
      vector<point> attn = bl.getWarnings();
      //if (bl.blSettings.doCheckTopol == true){
        for (i = 0; i < blp.size(); i++)
        {
            //attention(blp[i][0].x, blp[i][0].y);
            //attention(bl[i][bl[i].size()-1].x, bl[i][bl[i].size()-1].y, 0.1);
            glBegin (GL_LINE_LOOP);
            glColor3f (bl.colors[i].red, bl.colors[i].green, bl.colors[i].blue);
            for (j = 0; j < blp[i].size(); j++)
            {
                P = bl.place(ogl, blp[i][j]);
                glVertex2f (P.x, P.y);
            }
            glEnd ();
        }
        if (showPoints){
          for (i = 0; i < blp.size(); i++){
            for (j = 0; j < blp[i].size(); j++){
              P = bl.place(ogl, blp[i][j]);
              glBegin (GL_LINE_LOOP);
              vector<point> cr = glCircle(P.x, P.y, 0.01);
              for (UINT k = 0; k < cr.size(); k++)
              {
                  glVertex2f (cr[k].x, cr[k].y);
              }
              glEnd ();
            }
          }

        }
        /**********
      for (UINT i = 0; i < bl.ngroups; i++){
        UINT n = twoPow(i);
        vector<point> q = bl.getSetBoundaries(n, 2*bl.maxRadius*AIR, true);
        addRectangle(bl, ogl, q, bl.colors[i]);
      }
      /**********/
      //vector<point> bnd = bl.getBoundaries(bl.maxRadius, true);
      //addRectangle(bl, ogl, bnd);
      if (showContacts){
        for (i = 0; i < attn.size();  i++)
        {
            P = bl.place(ogl, attn[i]);
            //point tp;
            /*tp.x = warn[i].fx;
            tp.y = warn[i].fy;
            point P2;
            P2 = place(ogl, tp);
            glBegin (GL_LINES);
                glColor3f (0.0f, 1.0f, 0.0f);
                glVertex2f (P.x, P.y);
                glVertex2f (P2.x,P2.y);
            glEnd ();
            */
            temp = glCircle(P.x, P.y, P.radius);
            glBegin (GL_LINE_LOOP);
            glColor3f (0.0f, 1.0f, 0.0f);
            for (j = 0; j < temp.size(); j++)
            {
                glVertex2f (temp[j].x, temp[j].y);
            }
            glEnd ();
          }
      }

        bl.clearWarnings();
      //}
      //else{

        for (UINT i = 0; i < bl.circles.size() -1; i++)
        {
          if (bl.circles[i].radius > 0){
              //attention(blp[i][0].x, blp[i][0].y);
              //attention(bl[i][bl[i].size()-1].x, bl[i][bl[i].size()-1].y, 0.1);

               //Show forces
               //-----------
               /**********
              glBegin (GL_LINES);

              point t = showForce(bl.circles[i], 0.01);

              P = bl.place(ogl, bl.circles[i]);
              glVertex2f(P.x, P.y);
              P = bl.place(ogl, t);
              glVertex2f(P.x, P.y);
              glEnd ();
              /**********/
              //-----------
              /*
              // Show relationships
              for (j = 0; j < bl.circles.size(); j++)
              {
                if (bl.circles[j].radius > 0){
                  for (UINT group = 0; group < bl.ngroups; group++){
                    UINT mask = 1 << group;
                    glColor3f (bl.colors[group].red, bl.colors[group].green, bl.colors[group].blue);
                    if ((bl.circles[i].n & mask) > 0 && (bl.circles[j].n & mask) > 0){
                      glLineWidth(bl.getRelationships(i, j));
                      glBegin (GL_LINES);
                      P = bl.place(ogl, bl.circles[i]);
                      glVertex2f(P.x, P.y);
                      P = bl.place(ogl, bl.circles[j]);
                      glVertex2f(P.x, P.y);
                      glEnd ();
                    }
                  }
                }
              }*/

              glLineWidth(1);
          }
        //}

      }


      for (i = 0; i < bl.circles.size();  i++)
      {
          P = bl.place(ogl, bl.circles[i]);
          temp = glCircle(P.x, P.y, P.radius);
          if ((bl.circles[i].flags & IS_OUTSIDE) > 0){
            glColor3f (0.0f, 1.0f, 0.0f);
          }
          else{
            glColor3f (1.0f, 0.0f, 0.0f);
          }
          glBegin (GL_LINE_LOOP);
          for (j = 0; j < temp.size(); j++)
          {
              glVertex2f (temp[j].x, temp[j].y);
          }
          glEnd ();
      }
      // Text
      glColor3f(0.0f, 0.0f, 1.0f);
      float yd = 0.8f;
      for (i = 0; i < bl.dataDisplay.size(); i++){
        string mymsg = bl.dataDisplay[i];
        //showText(mymsg); exit(0);
        glRasterPos2f(-0.9f, yd);
        printString(mymsg);
        yd -= 0.1;
      }
      /*for (i = 0; i < bl.dataDisplay.size(); i++){
        free(bl.dataDisplay[i]);
      }
      bl.dataDisplay.clear();
      //glFlush();
      **********/
      SwapBuffers (hDC);
      /*********DEBUG**/
      if (attn.size() > 0){
        //wait();
      }
      /*********DEBUG***/
      //Sleep(1);
  }




  borderLine gsimulate(borderLine* blp, int ncycles, HDC hDC)
  {
    restart_log();

      borderLine bl = *blp;
      bl.refreshScreen.setLimits(1,1);
      MSG msg;
      point minP;
      point maxP;
      bool bQuit = false;
      while (!bQuit)
      {
          /* check for messages */
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
              /* handle or dispatch messages */
              if (msg.message == WM_QUIT)
              {
                  bQuit = TRUE;
              }
              {
                  TranslateMessage (&msg);
                  DispatchMessage (&msg);
              }
          }
          else
          {
              if (bl.refreshScreen.isMax() == true) toOGL(bl, hDC);
              bl.refreshScreen++;
          }
      }
      bQuit = false;
      while (!bQuit)
      {
          /* check for messages */
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
              /* handle or dispatch messages */
              if (msg.message == WM_QUIT)
              {
                  bQuit = TRUE;
              }
              {
                  TranslateMessage (&msg);
                  DispatchMessage (&msg);
              }
          }
          else
          {
              bl.setForcesFirstStep();
              //bl.setForces1();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.setCheckTopol(false);
              //bl.setContacts(false, true);
              bl.solve();
              bl.refreshScreen++;
              float tc = bl.getTotalCircleV();
              if (tc > 0 && tc < (1e-3*bl.ngroups / 5)){
                bQuit = true;
              }
              //Sleep(200);
          }
      }
      bQuit = false;
      while (!bQuit)
      {
          /* check for messages */
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
              /* handle or dispatch messages */
              if (msg.message == WM_QUIT)
              {
                  bQuit = TRUE;
              }
              else
              {
                  TranslateMessage (&msg);
                  DispatchMessage (&msg);
              }
          }
          else
          {
              bl.setForcesSecondStep();
              //bl.setForces1();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.setCheckTopol(false);
              bl.setContacts(false, true, 2*bl.maxRad()*AIR);
              bl.solve(true);
              //wait();
              //exit(0);
              bl.refreshScreen++;
              if (bl.blSettings.totalCircleV > 0 && bl.blSettings.totalCircleV < (5e-3*bl.ngroups / 5)){
                bQuit = true;
              }

          }
      }
      bl.setCheckTopol(true);
      bl.addLines();
      toOGL(bl, hDC);
      bl.setCheckTopol(false);
      //UINT b1 = bl.countOutsiders();
      //UINT bo = bl.chooseCombination();
      bQuit = false;
      while (!bQuit)
      {
          /* check for messages */
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
              /* handle or dispatch messages */
              if (msg.message == WM_QUIT)
              {
                  bQuit = TRUE;
              }
              else
              {
                  TranslateMessage (&msg);
                  DispatchMessage (&msg);
              }
          }
          else
          {
              bl.chooseCompact(true);
              bl.chooseCrossings(true);
              bl.setCheckTopol(true);
              bl.fixTopology();

              bl.setCheckTopol(false);
              toOGL(bl, hDC);
              bQuit = true;
              //bQuit = true;

          }
      }
      bl.setCheckTopol(true);
      if (bl.checkTopol() == false){

        //vector<point> s = bl.getBoundaries();
        //scale sc = scale(s[0], s[1]);
        //point pt; pt.x = 0; pt.y = 0; pt.radius = bl.minCircRadius;
        //point P = bl.place(sc, pt);
        //bl.interpolateToDist(P.radius);

        bl.interpolateToDist(5 * bl.minCircRadius * AIR);
        bl.setPrevState();
        bl.setSecureState();
      }
      else{
        bl.listOutsiders();
        ofstream result;
        fileText svgfile = bl.toSVG();
        result.open("error.svg");
        result.write(svgfile.getText().c_str(), svgfile.getText().size());
        result.close();
        exit(1);
      }
      bQuit = false;
      /*bl.setFixedCircles(true);
      while (!bQuit)
      {

          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {

              if (msg.message == WM_QUIT)
              {
                  bQuit = TRUE;
              }
              else
              {
                  TranslateMessage (&msg);
                  DispatchMessage (&msg);
              }
          }
          else
          {
              bl.setCheckTopol(true);
              bl.setForces1();
              bl.setContacts();
              bl.displayUINT("STATE: ", 1);
              toOGL(bl, hDC);
              bl.solve();
              bl.refreshScreen++;

          }
      }
      bl.setFixedCircles(false);*/
      bQuit = false;
      bl.resetTimer();
      bl.setCheckTopol(true);
      bl.attachScene();
      bl.scFriction(15);
      bl.scD(1e2);
      bl.scG(1e-2);
      //bl.scSave();
      while (!bQuit)
      {
          /* check for messages */
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
              /* handle or dispatch messages */
              if (msg.message == WM_QUIT)
              {
                  bQuit = TRUE;
              }
              else
              {
                  TranslateMessage (&msg);
                  DispatchMessage (&msg);
              }
          }
          else
          {
            if (bl.refreshScreen.isMax()) {
                toOGL(bl, hDC);
            }
            bl.refreshScreen++;
            //bl.writeSVG("result.svg");
            //tolog(bl.scCroack());
            bl.scSolve();
            bool bq = bl.isSimulationComplete();
            if (bq){
              bQuit = true;
            }
          }
      }
          // Debug topol
          bQuit = false;
      bl.resetTimer();
      bl.interpolateToDist(bl.minCircRadius);
      bl.scSpringK(1e2);
      bl.scFriction(70);


      while (!bQuit)
      {
          /* check for messages */
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
              /* handle or dispatch messages */
              if (msg.message == WM_QUIT)
              {
                  bQuit = TRUE;
              }
              else
              {
                  TranslateMessage (&msg);
                  DispatchMessage (&msg);
              }
          }
          else
          {
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.scSolve();
              bool bq = bl.isSimulationComplete();
              if (bq){
                bQuit = true;
              }
              bl.refreshScreen++;

          }
      }


      return bl;
  }
};

#endif // GRAPHICS_H_INCLUDED
