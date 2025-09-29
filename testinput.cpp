#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include "elements.h"
#include "topol.h"

std::string inp = "a b c\nd e g\ne n \nn i e";


std::vector<std::string> setn = {"set1", "set2", "set3"};
std::vector<std::vector<std::string>> setinp = {{"a", "b", "c", "x"}, {"c", "b", "c"}, {"a", "b", "d", "x"}};

std::string getFile(std::string prompt, std::string errorPrompt)
{
    std::string fname;
    std::cout << prompt << std::endl;
    std::cin >> fname;
    if (fname != ""){
      std::ifstream isfile;
      isfile.open(fname.c_str());
      if (!isfile.is_open())
      {
          isfile.close();
          std::cout << errorPrompt << std::endl;
          fname = getFile(prompt, errorPrompt);
      }
      isfile.close();
    }
    return fname;
}


int main(int argc, char *argv[]){
    std::string infile = "/home/vqf/proyectos/nVenn2/example.txt";
    std::string ufile = getFile("Filename: ", "Cannot find file");
    if (ufile != ""){
      infile = ufile;
    }
    //infile = "/home/vqf/proyectos/nVenn2/rror.txt";
    /*if (argc > 1){
        infile = argv[1];
    }
    else{
        infile = getFile("Input?", "Cannot find file. Please, write the location of the text file with the Venn table");
    }*/
    std::ifstream vFile;
    vFile.open(infile.c_str());
    std::stringstream content;
    content << vFile.rdbuf();
    vFile.close();
    borderLine b(content.str());
    restart_log();
      b.reset();
      UINT cstep = 1;
      for (UINT step = cstep; step < 8; step++){
          std::cout << step << std::endl;
        bool bQuit = false;
        bool success = b.setStep(step);
        if (!success) return 1;
        while (!bQuit){
          b.setCycle(step);
          if (b.err()){
            std::cout << b.errorMsg() << std::endl;
            bQuit = true;
          }
          //if (refreshScreen.isMax()) writeSVG();
          if (b.isStepFinished(step)){
            bQuit = true;
            std::string ofname = infile + ".svg";
            b.writeSVG(ofname);
          }
        }
        cstep = step;
      }

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
