/**************************
 * Includes
 *
 **************************/

#include "topol.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <signal.h>


//#ifndef GRAPHICS
//#define GRAPHICS
//#endif // GRAPHICS

#ifdef GRAPHICS

  #include <windows.h>
  #include <Windows.h>
  #include "graphics.h"
#endif // GRAPHICS










#ifndef GRAPHICS
borderLine lines;
int c = 0;

volatile sig_atomic_t gSignalStatus;
void handleSignal(int s){
  std::cout << lines.croack() << std::endl;
  tolog(lines.croack());
  c++;
  //if (c > 5){
    exit(0);
  //}
}
int main(int argc, char** argv)
{
    signal(SIGINT, handleSignal);
    std::string fname;
    std::string outputFile;
    std::ofstream result;
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
    lines = getFileInfo(fname, outputFile);
    //lines.showInfo();
    //std::string sigt = "1295570370;6;iba;ic;ida;iea;iss;nas;0;7;10;0;287;1;49;0;20;0;3;0;16;0;2;0;2;0;0;0;2;0;0;0;0;0;1;0;0;0;0;0;39;0;0;0;39;0;4;0;0;0;0;0;1;0;0;1;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;";
    //std::string cv = buildTw(sigt);
    //std::stringstream vv;
    //vv << cv;
    //lines = getInfoFromStream(vv);

    //lines.interpolate(50);
    lines.simulate();
    //mymap.textOut();
    //psfile = lines.toPS();
    //result.open("result.ps");
    //result.write(psfile.getText().c_str(), psfile.getText().size());
    //result.close();
    lines.restoreBl(lines.saveBl());
    lines.writeSVG();
    //lines.centerScene();
    lines.writeHTML("/home/vqf/delmeya.html");
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
    std::string fname;
    std::ofstream result;
    fileText psfile;
    fileText svgfile;
    fname = "venn.txt";
    std::string outputFile = "result.svg";
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
