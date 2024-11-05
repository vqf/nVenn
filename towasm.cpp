#include "topol.h"
#include <cstdlib>
#include <sstream>

using namespace std;
borderLine bl;

bool init_bl(string description){
  istringstream info(description);
  string line;
  getline(info, line);
  getline(info, line);
  UINT n = atoi(line.c_str());
  for (UINT i = 0; i < n; i++){

  }
  return true;
}


int main(){
  init_bl("nVenn12 jsons/49CE6E11.json\n4\nS_salivarius\nOther\nGAS\nGCS/GGS\n0\n2\n12\n0\n28\n7\n6\n0\n84\n15\n11\n0\n16\n2\n1\n0\n");
  return 0;
}
