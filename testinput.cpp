#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include "../gh/elements.h"
#include "../gh/topol.h"

std::string inp = "a b c\nd e g\n e \n";


std::vector<std::string> setn = {"set1", "set2", "set3"};
std::vector<std::vector<std::string>> setinp = {{"a", "b", "c", "x"}, {"c", "b", "c"}, {"a", "b", "d", "x"}};

int main(){

    nvenn yo;
    std::ifstream ifile;
    ifile.open("/home/vqf/proyectos/nVenn2/gh/sets.txt");
    std::stringstream txt;
    std::string l;
    while (std::getline(ifile, l)){
      txt << l << std::endl;
    }
    ifile.close();
    yo.addInfo(txt.str());
    std::stringstream info;
    info << yo.getCode();
    borderLine bl = getInfoFromStream(info, 0x00, "");
    bl.simulate();
    bl.writeSVG("/home/vqf/proyectos/nVenn2/gh/sets.svg");
}
