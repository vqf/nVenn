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
    std::string path1 = "/home/vqf/proyectos/nVenn2/errorTest.txt";
    std::string path2 = "/home/vqf/proyectos/nVenn2/extremeTest.txt";
    //bl.restoreFromFile("/home/vqf/Downloads/tmp.html");
    glutSimulate(path1, 0);
    initGlut(argc, argv);
}
