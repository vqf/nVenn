#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED
#include <gl/gl.h>
#include <gl/glu.h>
#include "bmpfont.h"
//#include <windows.h>



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

  void addRectangle(borderLine bl, scale ogl, vector<point> p){
    glBegin (GL_LINE_LOOP);
    glColor3f(.5, .5, .5);
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

  void toOGL(borderLine bl, HDC hDC)
  {
    borderLine dummy;
      UINT i, j;
      point P;     //coordinates
      vector<point> temp; //stores perimeters
      vector<vector<point> > blp = bl.bl;
      glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
      glClear (GL_COLOR_BUFFER_BIT);
      //define openGL scale

      scale ogl;
      //addRectangle(bl, ogl, bl.getBoundaries(2 * bl.maxRad()));
      ogl.initScale();
      //define vectors
      if (bl.blSettings.doCheckTopol == true){
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
      }
      else{

        for (i = 0; i < bl.circles.size() -1; i++)
        {
          if (bl.circles[i].radius > 0){
              //attention(blp[i][0].x, blp[i][0].y);
              //attention(bl[i][bl[i].size()-1].x, bl[i][bl[i].size()-1].y, 0.1);

               //Show forces
               //-----------
/*              glBegin (GL_LINES);

              point t = showForce(bl.circles[i], 0.01);

              P = bl.place(ogl, bl.circles[i]);
              glVertex2f(P.x, P.y);
              P = bl.place(ogl, t);
              glVertex2f(P.x, P.y);
              glEnd ();
*/
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
        }

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
      /**********/
      for (i = 0; i < bl.warn.size();  i++)
      {
          P = bl.place(ogl, bl.warn[i]);
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
      bl.warn.clear();
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
      //Sleep(1);
  }




  borderLine gsimulate(borderLine* blp, int ncycles, HDC hDC)
  {
    std::ofstream dlme;
dlme.open("log.txt", std::ios_base::out);
dlme.write("", 0);
dlme.close();

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

              //Sleep(200);
          }
      }
      bl.udt.clear();
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
              bl.setContacts(false, true, 2*bl.maxRad());
              bl.solve(true);
              bl.refreshScreen++;


          }
      }
      bl.udt.clear();
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
        //      bl.setForces3();
              //bl.chooseCombination();
              bl.addLines();
              bl.polishLines();
              //bl.setCheckTopol(true);
              UINT b1 = bl.countOutsiders();
              UINT bo = bl.chooseCombination();
              do{
                  //tolog("One more\n");
                b1 = bo;
                bo = bl.chooseCombination();
              } while (bo < b1);
              bl.chooseCrossings();
              bl.setCheckTopol(true);
              bl.addLines();
              bl.fixTopology(true);

              bl.setCheckTopol(false);
              toOGL(bl, hDC);
              bQuit = true;
              //bQuit = true;

          }
      }
      bl.setCheckTopol(true);
      if (bl.checkTopol() == false){
        bl.udt.clear();
        //vector<point> s = bl.getBoundaries();
        //scale sc = scale(s[0], s[1]);
        //point pt; pt.x = 0; pt.y = 0; pt.radius = bl.minCircRadius;
        //point P = bl.place(sc, pt);
        //bl.interpolateToDist(P.radius);
        bl.interpolateToDist(bl.minCircRadius);
        bl.setPrevState();
        bl.setSecureState();
      }
      else{
        bl.listOutsiders();
        ofstream result;
        fileText svgfile = bl.toSVG();
        result.open("result.svg");
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
              bl.setCheckTopol(true);
              bl.setForces1();
              bl.setContacts();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.solve();
              bl.refreshScreen++;

          }
      }
          // Debug topol
          bQuit = false;
      bl.resetTimer();
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
              bl.setCheckTopol(true);
              bl.setForces1();
              bl.setContacts();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.solve(false, true);
              bl.refreshScreen++;

          }
      }


      return bl;
  }
};

#endif // GRAPHICS_H_INCLUDED
