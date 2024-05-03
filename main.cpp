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


string getFile(string prompt, string errorPrompt)
{
    string fname;
    cout << prompt << endl;
    cin >> fname;
    ifstream isfile;
    isfile.open(fname.c_str());
    if (!isfile.is_open())
    {
        isfile.close();
        cout << errorPrompt << endl;
        fname = getFile(prompt, errorPrompt);
    }
    isfile.close();
    return fname;
}

//--------------------------------------------
void printv(vector<int> v)
{
    UINT i;
    for (i = 0; i < v.size(); i++)
    {
        cout << v[i];
    }
    cout << " ";
}
//--------------------------------------------
borderLine getFileInfo(string fname, string outputFile){
    ifstream vFile;
    string header;
    vector<string> groupNames;
    vector<int> temp;
    vector<float> weights;
    vector<string> labels;
    int i;
    vFile.open(fname.c_str());
    getline(vFile, header);
    cout << header;
    getline(vFile, header);
    int number = atoi(header.c_str());
    cout << endl << number << " groups:" << endl;
    for (i = 0; i < number; i++){
        getline(vFile, header);
        groupNames.insert(groupNames.end(), header);
        cout << header << endl;
    }
    int n = twoPow(number);
    for (i = 0; i < n; i++){
        getline(vFile, header); //  get the whole line
        string w = header.substr(0,header.find_first_of(" "));
        weights.insert (weights.end(), atoi(w.c_str())); // it take the first number
        string label;
        try
        {
            label = header.substr(header.find_first_of(" "));
        }
        catch (const std::exception& e)
        {
            cout << i << endl;
            label = "";
        }
        labels.insert (labels.end(), label);
        cout << "w=" << w << "  label:" << label << endl;
        //getline(vFile, header, ' '); /// get the number
        //weights.insert (weights.end(), atoi(header.c_str()));
        //getline(vFile, header) ;  ///  get the rest of the line with the labels
        //labels.insert (labels.end(), header);
        temp = toBin(i, number);
        printv(temp);
        cout << ".- " << weights[i] << " : " << labels[i] << endl;
    }
    vFile.close();
    binMap mymap(number);
    string dataFile = outputFile + ".data";
    borderLine lines(&mymap, groupNames, weights, labels, fname, outputFile);

    vFile.open(dataFile.c_str());
    if (vFile.good() == true){ // Unfinished
        vFile.close();
        lines.setCoords(dataFile);
    }


    return lines;
}



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
    lines.simulate(7);
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
HWND publich;

void temp(int n, string a="")
{
    char t[200];
    string temp;
    sprintf(t, "%d", n);
    temp = t;
    MessageBox(publich, temp.c_str(), a.c_str(), MB_ICONINFORMATION | MB_OK);
}

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

#endif
