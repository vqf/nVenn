#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include "elements.h"
#include "topol.h"

std::string inp = "a b c\nd e g\ne n \nn i e";


std::vector<std::string> setn = {"set1", "set2", "set3"};
std::vector<std::vector<std::string>> setinp = {{"a", "b", "c", "x"}, {"c", "b", "c"}, {"a", "b", "d", "x"}};


int main(int argc, char *argv[]){
    std::string infile = "";
    if (argc > 1){
        infile = argv[1];
    }
    else{
        infile = getFile("Input?", "Cannot find file. Please, write the location of the text file with the Venn table");
    }
    std::ifstream vFile;
    vFile.open(infile.c_str());
    std::stringstream content;
    content << vFile.rdbuf();
    vFile.close();
    borderLine b(content.str());
    b.simulate();
    b.loadPalette(2);
    b.setSVGOpacity(0.2);
    b.setSVGLineWidth(0.5);
    b.writeHTML(infile + ".html");
    return 0;
}

/*int main(){

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
    std::cout << yo.asJSON() << std::endl;
}
*/
