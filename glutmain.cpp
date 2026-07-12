#include "topol.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "toGlut.h"



int main(int argc, char *argv[]){
    restart_log();
    std::string path1 = "/home/vqf/proyectos/nVenn2/example.txt";
    std::string path2 = "/home/vqf/proyectos/nVenn2/extremeTest.txt";
    std::string path3 = "/home/vqf/proyectos/nVenn2/stressTest.txt";
    std::string path4 = "/home/vqf/proyectos/nVenn2/polish.txt";
    std::string path5 = "/home/vqf/Downloads/tmp.txt";
    std::string path6 = "/home/vqf/proyectos/nVenn2/extremeTest2.txt";
    //bl.restoreFromFile("/home/vqf/Downloads/tmp.html");
    glutSimulate(path3);

    //std::string t = path3 + ".html";
    //borderLine b;
    //b.restoreFromFile(t);
    //b.croack();
    initGlut(argc, argv);
}
