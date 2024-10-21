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
      dummy.initPoint(&temp);
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
  void toOGL(borderLine bl, HDC hDC)
  {
    borderLine dummy;
      UINT i, j;
      point P;     //coordinates
      dummy.initPoint(&P);
      vector<point> temp; //stores perimeters
      vector<vector<point> > blp = bl.bl;
      glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
      glClear (GL_COLOR_BUFFER_BIT);
      //define openGL scale

      scale ogl;
      ogl.initScale();

      //define vectors
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

      for (i = 0; i < bl.circles.size();  i++)
      {
          P = bl.place(ogl, bl.circles[i]);
          temp = glCircle(P.x, P.y, P.radius);
          glColor3f (1.0f, 0.0f, 0.0f);
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
        char* mymsg = bl.dataDisplay[i];
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
      borderLine bl = *blp;
      bl.refreshScreen.setLimits(1,1);
      MSG msg;
      point minP;
      bl.initPoint(&minP);
      point maxP;
      bl.initPoint(&maxP);
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
              bl.setForces1();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.solve();
              bl.refreshScreen++;

              //Sleep(200);
          }
      }
      bl.udt.clear();
      bQuit = false;
      bl.interpolate(700);
      bl.blSettings.margin /= 10;
      bl.setRadii();
      bl.setAsStable();
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
              bl.setForces2();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
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
              toOGL(bl, hDC);
              bQuit = true;

          }
      }


      return bl;
  }
};

#endif // GRAPHICS_H_INCLUDED
