#include <windows.h>
#include <Windows.h>
#include "topol.h"
#include "initogl.h"
#include "graphics.h"

void addCircle(point P){
  vector<point> temp = glCircle(P.x, P.y, P.radius);
  glBegin (GL_LINE_LOOP);
  glColor3f (0.0f, 1.0f, 0.0f);
  for (UINT j = 0; j < temp.size(); j++)
  {
      glVertex2f (temp[j].x, temp[j].y);
  }
  glEnd ();
}

void addLine(point p0, point p1){
  glBegin (GL_LINE_LOOP);
  glColor3f (0.0f, 0.0f, 1.0f);
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
  vector<point> circles = s.getPoints();
  for (UINT i = 0; i < circles.size(); i++){
    point tp = sc.place(ogl, circles[i]);
    addCircle(tp);
  }
  vector<springLink> springs = s.getLinks();
  for (UINT i = 0; i < springs.size(); i++){
    point p0 = circles[springs[i].from];
    point p1 = circles[springs[i].to];
    //tolog(p0.croack()); exit(0);
    point s0 = sc.place(ogl, p0);
    point s1 = sc.place(ogl, p1);
    addLine(s0, s1);
  }
  SwapBuffers (hDC);
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
               0, 0, 1200, 800,
               NULL, NULL, hInstance, NULL);
    publich = hWnd;
    //MessageBox(hWnd, "hi", "yo", MB_ICONINFORMATION | MB_OK);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    init(); // Init bitmap font

    //restart logger
    std::ofstream dlme;
    dlme.open("log.txt", std::ios_base::out);
    dlme.write("", 0);
    dlme.close();

    scene univ;
    point fp;
    fp.x = 5;
    fp.y = 5;
    fp.radius = 1;
    univ.addPoint(fp);
    fp.x = 10;
    fp.y = 7;
    univ.addPoint(fp);
    fp.x = 5;
    fp.y = 9;
    fp.mass = 2000;
    univ.addPoint(fp);
    float d = distance(10, 7, 5, 9);
    univ.addLink(0, 1, 2e5, 5);
    univ.addLink(0, 2, 200, 7);
    univ.addLink(1, 2, 1000, d);
    scale scscale(point(0, 0), point(20, 20));
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
          univ.solve(1e-2);
          OGLShow(univ, scscale, hDC, 1e-2);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}




