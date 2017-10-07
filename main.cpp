/**************************
 * Includes
 *
 **************************/

#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "topol.h"
#define CIRCLE_MASS 200.0f
#define POINT_MASS 20
#define GRAPHICS
#ifdef GRAPHICS
  #include <windows.h>
  #include <Windows.h>
  #include "graphics.h"
#endif // GRAPHICS

typedef unsigned int UINT;


float sk = 1e3f;
float dt = 1e-1f;
float kdt = 0.0f;
float baseV  = 5.0f;
float baseBV = 20.0f;

HWND publich;

using namespace std;

void temp(int n, string a="")
{
    int i, j;
    char t[200];
    string temp;
    j = sprintf(t, "%d", n);
    temp = t;
    MessageBox(publich, temp.c_str(), a.c_str(), MB_ICONINFORMATION | MB_OK);
}

void showText(char* n, string a="")
{
    MessageBox(publich, n, a.c_str(), MB_ICONINFORMATION | MB_OK);
}

class point
{
public:
    UINT n;
    float x;
    float y;
    float vx;
    float vy;
    float fx;
    float fy;
    float radius;
    float mass;
    bool cancelForce;
    point()
    {
        x = 0;
        y = 0;
        radius = 0;
        reset();
    }
    void reset()
    {
        resetv();
        resetf();
    }
    void resetv()
    {
        vx = 0;
        vy = 0;
    }
    void resetf()
    {
        fx = 0;
        fy = 0;
    }
};

class timeMaster
{
    UINT count;
    float unstable;
    float defaultst;
    float stored;
public:

    timeMaster()
    {}

    void init(float unst)
    {
        count = 0;
        unstable = unst;
        defaultst = unst;
        stored = unst;
    }

    void clear()
    {
        count = 0;
        unstable = defaultst;
    }

    void report(float unst)
    {
        if (unst == unstable)
        {
            ++count;
        }
        else
        {
            count = 0;
            unstable = unst;
        }
        if (count >= 5)
        {
            stored = unstable;
        }
    }

    float unstabledt()
    {
        return stored;
    }
};

class lCounter
{

    UINT counter;
    UINT lLimit;
    UINT uLimit;
    UINT span;

public:

    lCounter(UINT lowerLimit, UINT upperLimit)
    {
        counter = lowerLimit;
        lLimit = lowerLimit;
        uLimit = upperLimit;
        span = uLimit - lLimit;
    }

    lCounter()
    {
        counter = 0;
        lLimit = 0;
        uLimit = 0;
        span = 0;
    }

    void setLimits(UINT lowerLimit, UINT upperLimit)
    {
        counter = lowerLimit;
        lLimit = lowerLimit;
        uLimit = upperLimit;
        span = uLimit - lLimit;
    }

    bool isMax()
    {
        if (counter == uLimit)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool isMin()
    {
        if (counter == lLimit)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void operator++(int)
    {
        if (counter < uLimit)
        {
            counter++;
        }
        else
        {
            counter = lLimit;
        }
    }
    void operator++()
    {
        if (counter < uLimit)
        {
            counter++;
        }
        else
        {
            counter = lLimit;
        }
    }
    void operator--(int)
    {
        if (counter > lLimit)
        {
            counter--;
        }
        else
        {
            counter = uLimit;
        }
    }
    void operator--()
    {
        if (counter > lLimit)
        {
            counter--;
        }
        else
        {
            counter = uLimit;
        }
    }
    operator UINT()
    {
        return counter;
    }
    void operator=(UINT a)
    {
        UINT tCount;
        tCount =  a - lLimit;
        tCount = tCount % span;
        counter = lLimit + tCount;
    }
};

class scale
{
public:

    float minX;
    float minY;
    float maxX;
    float maxY;
    bool clear;

    float xSpan()
    {
        float result = maxX - minX;
        return result;
    }
    float ySpan()
    {
        float result = maxY - minY;
        return result;
    }
    float ratio()
    {
        float result;
        if (xSpan() == 0)
            return 0.0f;
        result = ySpan() / xSpan();
        return result;
    }
};

struct rgb
{
    float red;
    float green;
    float blue;
};

template<typename T>
string toString(T input)
{
    ostream result;
    result << input;
    return result;
}

UINT twoPow(UINT n)
{
    if (n > 8*sizeof(UINT)-1)
        return 0;
    UINT result = 1;
    result = result << n;
    return result;
}

rgb toRGB(int color, int max)
{
    rgb result;
    result.red = (0x00FF0000 & color)/0x10000;
    result.red = result.red * max / 0xFF;
    result.green = (0x0000FF00 & color)/0x100;
    result.green = result.green * max / 0xFF;
    result.blue = (0x000000FF & color);
    result.blue = result.blue * max / 0xFF;
    return result;
}

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

float sprod(vector<float> a, vector<float> b)
{
    float result;
    int i;
    result = 0;
    for (i = 0; i < a.size(); i++)
    {
        result += a[i] * b[i];
    }
    return result;
}

vector<float> arr2vec(float arr[], int n)
{
    int i;
    vector<float> result;
    for (i = 0; i < n; i++)
    {
        result.insert(result.end(), arr[i]);
    }
    return result;
}


vector<int> toBin(int number, int nBits = 0)
{
    //  Takes an integer and returns a vector containing
    //  its binary representation

    int i;                          // iterate over each bit
    int bit;                        // temp storage of each bit
    vector<int> bits;               // return vector
    while (number > 1)
    {
        bit = number % 2;
        number = number / 2;
        bits.insert(bits.begin(), bit);
    }
    if (number > 0)
        bits.insert(bits.begin(),1);
    if (nBits > bits.size())
    {
        for (i = bits.size(); i < nBits; i++)
        {
            bits.insert(bits.begin(), 0);
        }
    }
    return bits;
}

//--------------------------------------------
void printv(vector<int> v)
{
    int i;
    for (i = 0; i < v.size(); i++)
    {
        cout << v[i];
    }
    cout << " ";
}
//--------------------------------------------

int toInt(vector<int> v)
{
    // Takes a vector containing the binary representation
    // of an integer and returns that integer

    int i;                          // decreasing iterator
    int counter = 0;                // increasing iterator
    int result = 0;
    for (i = v.size()-1; i >= 0; --i)
    {
        result = result + v[i] * twoPow(counter);
        counter++;
    }
    return result;
}

class fileText
{
    string text;
public:
    void addLine(string t)
    {
        text = text + t +"\n";
    }
    void clearText()
    {
        text = "";
    }
    string getText()
    {
        return text;
    }
};

float distance(float x0, float y0, float x1, float y1)
{
    float result = 0;
    float rx, ry;
    rx = x1 - x0;
    ry = y1 - y0;
    rx *= rx;
    ry *= ry;
    result = sqrt(rx + ry);
    return result;
}

float perimeter(vector<point> v, bool close = false)
{
    int i;
    float result = 0;
    float rx, ry;
    for (i = 0; i < v.size() - 1; i++)
    {
        rx = v[i+1].x - v[i].x;
        ry = v[i+1].y - v[i].y;
        result += sqrt(rx * rx + ry * ry);
    }
    if (close)
    {
        rx = v[v.size()-1].x - v[0].x;
        ry = v[v.size()-1].y - v[0].y;
        result += sqrt(rx * rx + ry * ry);
    }
    return result;
}


#ifndef GRAPHICS
int main()
{
    int i;
    int number,n;
    string fname;
    string header;
    ifstream vFile;
    ofstream result;
    vector<int> temp;
    vector<string> groupNames;
    vector<float> weights;
    fileText psfile;
    fname = getFile("Name of the Venn data file?", "File not found!");
    vFile.open(fname.c_str());
        getline(vFile, header);
        cout << header;
        getline(vFile, header);
        number = atoi(header.c_str());
        cout << endl << number << " groups:" << endl;
        for (i = 0; i < number; i++){
            getline(vFile, header);
            cout << header << endl;
        }
        n = (int)pow(2, number);
        for (i = 0; i < n; i++){
            getline(vFile, header);
            weights.insert (weights.end(), atoi(header.c_str()));
            temp = toBin(i, number);
            printv(temp);
            cout << ".- " << weights[i] << endl;
        }
    vFile.close();
    binMap mymap(number);
    borderLine lines(mymap, weights);
    lines.interpolate(50);
    lines.simulate(1000);
    mymap.textOut();
    psfile = lines.toPS();
    result.open("result.ps");
    result.write(psfile.getText().c_str(), psfile.getText().size());
    result.close();
    return 0;
}
#else

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
    float theta = 0.0f;
    int i;
    int number,n;
    string fname;
    string header;
    ifstream vFile;
    ofstream result;
    vector<int> temp;
    vector<string> groupNames;
    vector<float> weights;
    fileText psfile;
    fname = "venn.txt";
    vFile.open(fname.c_str());
    getline(vFile, header);
    getline(vFile, header);
    number = atoi(header.c_str());
    for (i = 0; i < number; i++)
    {
        getline(vFile, header);
        cout << header << endl;
    }
    n = twoPow(number);
    for (i = 0; i < n; i++)
    {
        getline(vFile, header);
        weights.insert (weights.end(), atoi(header.c_str()));
        temp = toBin(i, number);
        printv(temp);
        cout << ".- " << weights[i] << endl;
    }
    vFile.close();
    binMap mymap(number);
    borderLine lines(mymap, weights);

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
               "GLSample", "OpenGL Sample",
               WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
               0, 0, 800, 600,
               NULL, NULL, hInstance, NULL);
    publich = hWnd;
    //MessageBox(hWnd, "hi", "yo", MB_ICONINFORMATION | MB_OK);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    init(); // Init bitmap font

    lines.interpolate(50);
    lines.simulate(1000, hDC);


    //mymap.textOut();
    psfile = lines.toPS();
    result.open("result.ps");
    result.write(psfile.getText().c_str(), psfile.getText().size());
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
