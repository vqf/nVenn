#include <windows.h>
#include <Windows.h>
#include "topol.h"
#include "initogl.h"
#include "graphics.h"
#include "scene.h"
#include <math.h>
//#include <random>

bool showForces = true;


void addCircle(point P, vector<float> color = {0, 1, 0}){
  vector<point> temp = glCircle(P.x, P.y, P.radius);
  glBegin (GL_LINE_LOOP);
  glColor3f (color[0], color[1], color[2]);
  for (UINT j = 0; j < temp.size(); j++)
  {
      glVertex2f (temp[j].x, temp[j].y);
  }
  glEnd ();
}

void addLine(point p0, point p1, vector<float> color = {0, 0, 1}){
  glBegin (GL_LINE_LOOP);
  glColor3f (color[0], color[1], color[2]);
  glVertex2f(p0.x, p0.y);
  glVertex2f(p1.x, p1.y);
  glEnd ();
}

void OGLShow(scene s, scale sc, HDC hDC, float dt){
  time_t start_time = time(nullptr);
  glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
  glClear (GL_COLOR_BUFFER_BIT);
  scale ogl;
  ogl.initScale();
  vector<point*> circles = s.getPoints();
  sc.setClear();
  for (UINT i = 0; i < circles.size(); i++){
    sc.addToScale(*(circles[i]));
  }
  s.addInfo("HX: ", sc.xSpan());
  vector<point> v = s.getVirtual();
  vector<string> w = s.getInfo();
  for (UINT i = 0; i < circles.size(); i++){
    point tp = sc.place(ogl, *(circles[i]));
    addCircle(tp);
  }
  vector<springLink> springs = s.getLinks();
  vector<springLink> rods = s.getRods();
  for (UINT i = 0; i < springs.size(); i++){
    point *p0 = circles[springs[i].from];
    point *p1 = circles[springs[i].to];
    //tolog(p0.croack()); exit(0);
    point s0 = sc.place(ogl, *p0);
    point s1 = sc.place(ogl, *p1);
    addLine(s0, s1);
  }
  for (UINT i = 0; i < rods.size(); i++){
    point *p0 = circles[rods[i].from];
    point *p1 = circles[rods[i].to];
    //tolog(p0.croack()); exit(0);
    point s0 = sc.place(ogl, *p0);
    point s1 = sc.place(ogl, *p1);
    addLine(s0, s1, {1, 0, 0});
  }
  if (showForces){
    for (UINT i = 0; i < circles.size(); i++){
      point *a = circles[i];
      point att = sc.place(ogl, *a);
      float dx = a->x + a->fx * 1e-2;
      float dy = a->y + a->fy * 1e-2;
      point t; t.x = dx; t.y = dy;
      point a2 = sc.place(ogl, t);
      addLine(att, a2, {1, 0, 0});
    }
  }
  glColor3f(0.0f, 0.0f, 1.0f);
  float yd = 0.8f;
  for (UINT i = 0; i < w.size(); i++){
    string mymsg = w[i];
    //showText(mymsg); exit(0);
    glRasterPos2f(-0.9f, yd);
    printString(mymsg);
    yd -= 0.1;
  }
  /**
  if (v.size() > 0){
    for (UINT i = 0; i < v.size(); i++){
      point a = v[i];
      a.radius = 2;
      point f;
      f.x = a.fx;
      f.y = a.fy;
      point att = sc.place(ogl, a);
      addCircle(att, {1, 0, 0});
      point a2 = sc.place(ogl, f);
      a2.x += att.x;
      a2.y += att.y;
      addLine(att, a2);
    }
  }
  /***/
  SwapBuffers (hDC);
  if (v.size() > 0){
    s.clearVirtual();
    wait();
  }
  time_t end_time = time(nullptr);
  time_t diff = end_time - start_time;
  time_t tdt = time_t(dt * 1000);
  if (diff < tdt){
    Sleep(tdt - diff);
  }
}

/**************************
 * WinMain
 *
 **************************/
int WINAPI
WinMain (HINSTANCE hInstance,
         HINSTANCE hPrevInstance,
         LPSTR lpCmdLine,
         int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass (&wc);

    /* create main window */
    hWnd = CreateWindow (
               "GLSample", "nVenn",
               WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
               0, 0, 800.0f, 800.0f,
               NULL, NULL, hInstance, NULL);
    publich = hWnd;
    //MessageBox(hWnd, "hi", "yo", MB_ICONINFORMATION | MB_OK);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    init(); // Init bitmap font

    //restart logger
    restart_log();
    /*std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(-1.0, 1.0);*/

    scene univ;
    univ.setFriction(0);
    univ.setG(2e-3);
    UINT np = 15;
    UINT nr = 5;
    float r = 0.3;
    float d = 1;
    for (UINT j = 0; j < nr; j++){
      for (UINT i = 0; i < np; i++){
        point t;
        t.x = i + 0.1*std::sin(i); t.y = j + 0.1*std::cos(j); t.radius = r;
        univ.addPoint(t);
      }
    }
    for (UINT j = 0; j < nr; j++){
      for (UINT i = 1; i < np; i++){
        UINT f = j * np + i - 1;
        UINT t = j * np + i;
        univ.addLink(f, t, 1000, d);
      }
    }
    for (UINT j = 1; j < nr; j++){
      for (UINT i = 1; i < np; i++){
        UINT f = (j-1) * np + i;
        UINT t = j * np + i;
        univ.addLink(f, t, 1000, d);
      }
    }

    //univ.addRod(0, ncirc >> 1, 2);
    //univ.addRod(ncirc >> 2, 3 * ncirc >> 2, 2);
    scale scscale(point(0, 0), point(30, 30));

    //tolog(univ.croack());

    /* program main loop */
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
          Keyboard_Input();
          if (DUMP || univ.dumpme()){
            tolog(univ.croack()); exit(0);
          }
          float dt = univ.solve();
          OGLShow(univ, scscale, hDC, dt);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}




