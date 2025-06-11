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
    glutSimulate("/home/vqf/web/nvenn2/lighttest.txt");
    initGlut(argc, argv);
}
