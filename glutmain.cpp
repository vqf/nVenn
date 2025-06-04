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
    glutSimulate("/home/vqf/proyectos/nVenn2/stressTest.txt");
    initGlut(argc, argv);
}
