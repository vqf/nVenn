#include "topol.h"
#include <cstdlib>
#include <sstream>

//
#include <iostream>

//

using namespace std;
borderLine bl;

extern "C" {
 bool init_bl(string description);
 void set_step(UINT step_number);
 void set_cycle(UINT step_number);
 bool finished(UINT step_number);
 bool draw();
 string svg();
}

bool init_bl(string description){
  stringstream info(description);
  bl = getInfoFromStream(info);
  return true;
}

void set_step(UINT step_number){
  bl.setStep(step_number);
}

void set_cycle(UINT step_number){
  bl.setCycle(step_number);
}

bool finished(UINT step_number){
  return bl.isStepFinished(step_number);
}

bool draw(){
  return bl.refresh();
}

string svg(){
  return bl.toSVG().getText();
}

int main(){
  init_bl("nVenn12 jsons/49CE6E11.json\n4\nS_salivarius\nOther\nGAS\nGCS/GGS\n0\n2\n12\n0\n28\n7\n6\n0\n84\n15\n11\n0\n16\n2\n1\n0\n");
  for (UINT step = 1; step < 8; step++){
    bool bQuit = false;
    cout << "Step " << step << endl;
    set_step(step);
    while (!bQuit){
      set_cycle(step);
      if (finished(step)){
        bQuit = true;
      }
    }
  }
  return 0;
}
