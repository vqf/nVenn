
#include <Rcpp.h>
#include "topol.h"
using namespace Rcpp;

borderLine bl;



void nVennCpp(String desc){
  bl = borderLine(desc);
}

// [[Rcpp::export]]
void nvSimulate(bool verbose){
  UINT step = 1;
  UINT maxStep = 8;
  bl.setStep(step);
  while (step < maxStep){
    bl.setCycle(step);
    if (bl.isStepFinished(step)){
      if (verbose){
        Rcout << "Step " << step << " finished." << std::endl;
      }
      step++;
      bl.setStep(step);
    }
  }
}

// [[Rcpp::export]]
List nvGetRegion(unsigned int n) {
  List result = bl.getVennRegion(n);
  return result;
}

// [[Rcpp::export]]
String nvSvg() {
  
    return bl.toSVG().getText() ;
}
