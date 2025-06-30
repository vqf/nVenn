
#include <Rcpp.h>
#include "topol.h"
using namespace Rcpp;

borderLine bl;


// [[Rcpp::export]]
void nVenn(String desc){
  bl = borderLine(desc);
}

// [[Rcpp::export]]
void simulate(bool verbose){
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
String svg() {
  
    return bl.toSVG().getText() ;
}
