#include <cstdlib>
#include <sstream>

#include "topol.h"

//
#include <iostream>

//

borderLine bl;
std::string svgcode;
std::string errorMessage;

extern "C" {
 bool init_bl(const char* description);
 void set_step(UINT step_number);
 void set_cycle(UINT step_number);
 bool finished(UINT step_number);
 bool error();
 const char* errorMsg();
 bool draw();
 void load_signature(const char* signature);
 void rotate_venn(float angle);
 const char* svg();
}


void rotate_venn(float angle){
  bl.rotateScene(angle);
}

bool init_bl(const char* description){
  std::stringstream info;
  info << description << std::endl;
  bl = getInfoFromStream(info);
  //free(&description);
  return true;
}

void load_signature(const char* signature){
  std::stringstream sig;
  sig << signature;
  std::string desc = buildTw(signature);
  std::stringstream in;
  in << desc;
  bl = getInfoFromStream(in);
  bl.loadSignature(sig.str());

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


bool error(){
  return bl.err();
}

const char* errorMsg(){
  errorMessage = bl.errorMsg();
  return errorMessage.c_str();
}

bool draw(){
  return bl.refresh();
}

const char* svg(){
  svgcode = bl.toSVG().getText();
  const char* result = svgcode.c_str();
  return result;
}

int main(){
  init_bl("nVenn12 jsons/49CE6E11.json\n4\nS_salivarius\nOther\nGAS\nGCS/GGS\n0\n2\n12\n0\n28\n7\n6\n0\n84\n15\n11\n0\n16\n2\n1\n0\n");
  for (UINT step = 1; step < 8; step++){
    bool bQuit = false;
    std::cout << "Step " << step << std::endl;
    set_step(step);
    while (!bQuit){
      set_cycle(step);
      if (finished(step)){
        bQuit = true;
      }
    }
  }
  const char* s = svg();
  std::cout << (std::string) s << std::endl;
  std::string yo;
  getline(std::cin, yo);
  return 0;
}
