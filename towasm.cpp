#include <cstdlib>
#include <sstream>

#include "topol.h"

//
#include <iostream>

//

borderLine bl;
std::string svgcode;
std::string errorMessage;
std::string saved;
std::string rvenn;

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
 const char* html();
 const char* get_region(UINT n);
 void set_color(UINT setNumber, UINT red, UINT green, UINT blue);
 void set_opacity(float t);
 void set_line_width(float t);
 void set_font_size(UINT fs);
 void load_sets(const char* sets, UINT bycol);
 void set_palette(UINT p);
 void show_region_size(bool s);
 void show_region_description(bool s);
 void restore_prev(const char* savedState);
 void reset();
 void run();
}

void reset(){
  bl.reset();
}

void restore_prev(const char* savedState){
    std::stringstream conv;
    conv << savedState;
    saved = conv.str();
    bl.restoreBl(saved);
}

void show_region_description(bool s){
    bl.showRegionNumbers(s);
}

void show_region_size(bool s){
    bl.showCircleNumbers(s);
}

const char* get_region(UINT n){
    rvenn = bl.getVennRegion(n);
    const char* result_region = rvenn.c_str();
    return result_region;
}

void rotate_venn(float angle){
  bl.rotateScene(angle);
}

void set_color(UINT setNumber, UINT red, UINT green, UINT blue){
    bl.setRGBColor(setNumber, red, green, blue);
}

void set_palette(UINT p){
    bl.loadPalette(p);
}

void set_opacity(float t){
    bl.setSVGOpacity(t);
}

void set_line_width(float t){
    bl.setSVGLineWidth(t);
}

void showSizes(bool s){
    bl.showCircleNumbers(s);
}

void showRegionDesc(bool s){
    bl.showRegionNumbers(s);
}

void set_font_size(UINT fs){
    bl.setFontSize(fs);
}

void run(){
    bl.simulate();
}

bool init_bl(const char* description){
  std::stringstream info;
  info << description << std::endl;
  bl = getInfoFromStream(info);
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

void load_sets(const char* sets, UINT bycol){
    bl = fromSets(sets, bycol);
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

const char* html(){
  svgcode = bl.toHTML().getText();
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
  set_color(4, 255, 0, 0);
  const char* s = svg();
  std::cout << (std::string) s << std::endl;
  std::string yo;
  getline(std::cin, yo);
  return 0;
}
