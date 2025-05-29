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

//  #include <windows.h>
//  #include <Windows.h>
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

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(900,900);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("nVenn2");
    bool bQuit = FALSE;
    std::string fname;
    std::ofstream result;
    fileText psfile;
    fileText svgfile;
    fname = "venn.txt";
    std::string outputFile = "result.svg";

    init(); // Init bitmap font

    //lines.interpolate(8);

    glGraphics mygl;
    borderLine lres = mygl.gsimulate(fname, outputFile);


    //mymap.textOut();
    psfile = lres.toPS();
    result.open("result.ps");
    result.write(psfile.getText().c_str(), psfile.getText().size());
    result.close();

    svgfile = lres.toSVG();
    result.open("result.svg");
    result.write(svgfile.getText().c_str(), svgfile.getText().size());
    result.close();



}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
    }

    glutPostRedisplay();
}



#endif
