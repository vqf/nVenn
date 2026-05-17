#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include "topol.h"
#include "elements.h"


template<typename T>
void printv(std::vector<T> v){
  for (UINT i = 0; i < v.size() - 1; i++){
    std::cout << v[i] << ", ";
  }
  std::cout << v[v.size()-1] << std::endl;

}


int main(){
  std::string path1 = "/home/vqf/proyectos/nVenn2/example.txt";
  std::string path2 = "/home/vqf/proyectos/nVenn2/extremeTest.txt";
  std::string path3 = "/home/vqf/proyectos/nVenn2/stressTest.txt";
  std::string path4 = "/home/vqf/proyectos/nVenn2/polish.txt";
  std::string path5 = "/home/vqf/Downloads/tmp.txt";
  std::string path6 = "/home/vqf/proyectos/nVenn2/extremeTest2.txt";
  borderLine bl = fromSetFile(path1);
  UINT lvl = 2;
  UINT n = bl.countCombs(lvl);
  std::cout << n << std::endl;
  float t = bl.estimateExhaustiveRunTime(lvl);
  std::cout << t << std::endl;
  bl.simulate(true, lvl);
  bl.writeSVG("exhaustive.svg");
  return 0;
}
