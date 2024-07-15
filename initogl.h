#ifndef INITOGL_H_INCLUDED
#define INITOGL_H_INCLUDED

HWND publich;
float DUMP = false; // Debugging;


void showText(char* n, string a="")
{
    MessageBox(publich, n, a.c_str(), MB_ICONINFORMATION | MB_OK);
}



/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

vector<point> glCircle(float x, float y, float r){
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


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK
WndProc (HWND hWnd, UINT message,
         WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
        case WM_CREATE:
        return 0;
        case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

        case WM_DESTROY:
        return 0;

        case WM_KEYDOWN:
        switch (wParam)
        {
            case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;

        default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/



void
EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );

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

void Keyboard_Input()
{

	if((GetKeyState(VK_LEFT) & 0x80))
	{
		wait();
	}

      if(GetKeyState('D')& 0x80)
	{
	  DUMP = true;
	}
}

/******************
 * Disable OpenGL
 *
 ******************/

void
DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}


#endif // INITOGL_H_INCLUDED
