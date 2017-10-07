#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED
#include <gl/gl.h>
#include <gl/glu.h>
#include "bmpfont.h"

class glGraphics: private borderLine{
  void toOGL(HDC hDC)
  {
      int i, j;
      point P;     //coordinates
      vector<point> temp; //stores perimeters
      glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
      glClear (GL_COLOR_BUFFER_BIT);
      //define openGL scale

      scale ogl;

      ogl.minX = -1;
      ogl.maxX = 1;
      ogl.minY = -1;
      ogl.maxY = 1;
      //define vectors
      for (i = 0; i < bl.size(); i++)
      {
          //attention(bl[i][0].x, bl[i][0].y, 0.1);
          //attention(bl[i][bl[i].size()-1].x, bl[i][bl[i].size()-1].y, 0.1);
          glBegin (GL_LINE_LOOP);
          glColor3f (colors[i].red, colors[i].green, colors[i].blue);
          for (j = 0; j < bl[i].size(); j++)
          {
              P = place(ogl, bl[i][j]);
              glVertex2f (P.x, P.y);
          }
          glEnd ();
      }

      for (i = 0; i < circles.size();  i++)
      {
          P = place(ogl, circles[i]);
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
      for (i = 0; i < warn.size();  i++)
      {
          P = place(ogl, warn[i]);
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
      for (i = 0; i < dataDisplay.size(); i++){
        char* mymsg = dataDisplay[i];
        //showText(mymsg); exit(0);
        glRasterPos2f(-0.9f, yd);
        printString(mymsg);
        yd -= 0.1;
      }
      warn.clear();
      for (i = 0; i < dataDisplay.size(); i++){
        free(dataDisplay[i]);
      }
      dataDisplay.clear();
      glFlush();
      /**********/
      SwapBuffers (hDC);
      Sleep(1);
  }
  void simulate(int ncycles, HDC hDC)
  {
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
              if (refreshScreen.isMax()) toOGL(hDC);
              refreshScreen++;
              warn.clear();
              for (i = 0; i < dataDisplay.size(); i++){
                free(dataDisplay[i]);
              }
              dataDisplay.clear();
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
              setForces1();
              solve();
              if (refreshScreen.isMax()) toOGL(hDC);
              refreshScreen++;
              warn.clear();
              for (i = 0; i < dataDisplay.size(); i++){
                free(dataDisplay[i]);
              }
              dataDisplay.clear();
              // TODO (vic#1#): Attention here\

              //Sleep(200);
          }
      }

      udt.clear();
      bQuit = false;
      interpolate(700);
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
              setForces3();
              solve();
              if (refreshScreen.isMax()) toOGL(hDC);
              refreshScreen++;
              warn.clear();
              for (i = 0; i < dataDisplay.size(); i++){
                free(dataDisplay[i]);
              }
              dataDisplay.clear();
              //Sleep(1);
          }
      }
  }

};

#endif // GRAPHICS_H_INCLUDED
