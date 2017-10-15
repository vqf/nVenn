#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED
#include <gl/gl.h>
#include <gl/glu.h>
#include "bmpfont.h"



class glGraphics{
  public:

  vector<point> glCircle(float x, float y, float r)
  {
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
  void toOGL(borderLine bl, HDC hDC)
  {
      int i, j;
      point P;     //coordinates
      vector<point> temp; //stores perimeters
      vector<vector<point> > blp = bl.bl;
      glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
      glClear (GL_COLOR_BUFFER_BIT);
      //define openGL scale

      scale ogl;

      ogl.minX = -1;
      ogl.maxX = 1;
      ogl.minY = -1;
      ogl.maxY = 1;
      //define vectors
      for (i = 0; i < blp.size(); i++)
      {
          //attention(bl[i][0].x, bl[i][0].y, 0.1);
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
      bl.warn.clear();
      for (i = 0; i < bl.dataDisplay.size(); i++){
        free(bl.dataDisplay[i]);
      }
      bl.dataDisplay.clear();
      glFlush();
      /**********/
      SwapBuffers (hDC);
      Sleep(1);
  }
  borderLine gsimulate(borderLine* blp, int ncycles, HDC hDC)
  {
      borderLine bl = *blp;
      int i, j, k;
      int cycle;
      int size;
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
              //setForces1();
              //solve();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.refreshScreen++;
              bl.warn.clear();
              for (i = 0; i < bl.dataDisplay.size(); i++){
                free(bl.dataDisplay[i]);
              }
              bl.dataDisplay.clear();
              // TODO (vic#1#): Attention here\

              //Sleep(100);
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
              bl.solve();
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.refreshScreen++;
              bl.warn.clear();
              for (i = 0; i < bl.dataDisplay.size(); i++){
                free(bl.dataDisplay[i]);
              }
              bl.dataDisplay.clear();
              // TODO (vic#1#): Attention here\

              //Sleep(200);
          }
      }
      bl.udt.clear();
      bQuit = false;
      bl.interpolate(700);

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
              bl.setForces3();
              bl.solve(true);
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.refreshScreen++;
              bl.warn.clear();
              for (i = 0; i < bl.dataDisplay.size(); i++){
                free(bl.dataDisplay[i]);
              }
              bl.dataDisplay.clear();
              //Sleep(1);
          }
      }
      bl.udt.clear();
      bQuit = false;

      //interpolate(700);
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
              bl.setForces3();
              bl.solve(true);
              if (bl.refreshScreen.isMax()) toOGL(bl, hDC);
              bl.refreshScreen++;
              bl.warn.clear();
              for (i = 0; i < bl.dataDisplay.size(); i++){
                free(bl.dataDisplay[i]);
              }
              bl.dataDisplay.clear();
              //Sleep(1);
          }
      }
      return bl;
  }
};

#endif // GRAPHICS_H_INCLUDED
