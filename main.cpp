/**************************
 * Includes
 *
 **************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "topol.h"


//#ifndef GRAPHICS
//#define GRAPHICS
//#endif // GRAPHICS

#ifdef GRAPHICS

  #include <windows.h>
  #include <Windows.h>
  #include "graphics.h"
#endif // GRAPHICS





using namespace std;





#ifndef GRAPHICS
int main(int argc, char** argv)
{
    string fname;
    string outputFile;
    ofstream result;
    fileText psfile;
    fileText svgfile;
    if (argc > 1){
      fname = argv[1];
    }
    else{
      fname = getFile("Name of the Venn data file?", "File not found!");
    }
    if (argc > 2){
      outputFile = argv[2];
    }
    else{
      outputFile = "result.svg";
    }
    borderLine lines = getFileInfo(fname, outputFile);
    lines.showInfo();
    //lines.interpolate(50);
    lines.simulate();
    //mymap.textOut();
    //psfile = lines.toPS();
    //result.open("result.ps");
    //result.write(psfile.getText().c_str(), psfile.getText().size());
    //result.close();
    svgfile = lines.toSVG();
    result.open(outputFile.c_str());
    result.write(svgfile.getText().c_str(), svgfile.getText().size());
    result.close();

    return 0;
}
#else
#include "initogl.h"
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
    string fname;
    ofstream result;
    fileText psfile;
    fileText svgfile;
    fname = "venn.txt";
    string outputFile = "result.svg";
    borderLine lines = getFileInfo(fname, outputFile);

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

    //lines.interpolate(8);

    glGraphics mygl;
    borderLine lres = mygl.gsimulate(&lines, 100, hDC);


    //mymap.textOut();
    psfile = lres.toPS();
    result.open("result.ps");
    result.write(psfile.getText().c_str(), psfile.getText().size());
    result.close();

    svgfile = lres.toSVG();
    result.open("result.svg");
    result.write(svgfile.getText().c_str(), svgfile.getText().size());
    result.close();



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
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}

#endif
