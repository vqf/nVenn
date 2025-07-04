
#include <Rcpp.h>
#include "topol.h"
using namespace Rcpp;

borderLine bl;

void nvSimulate(bool verbose){
  UINT step = 1;
  UINT maxStep = 8;
  bl.setStep(step);
  while (step < maxStep){
    bl.setCycle(step);
    if (bl.err()){
      Rcout << bl.errorMsg() << std::endl;
      step = maxStep;
    }
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
void nVennR(SEXP desc, unsigned int byCol = 0){
  List sv = desc;
  std::string dsc;
  if (sv.size() > 0){
    StringVector s1 = as<StringVector>(sv[0]);
    if (sv.size() == 1 && s1.size() == 1){ //Text
      dsc = as<std::string>(s1[0]);
    }
    else{
      Function asNamespace("asNamespace");
      Environment nv_env = asNamespace("nVennR");
      Function f = nv_env[".lol2string"];
      //Function f("lol2string");
      dsc = as<std::string>(f(desc));
    }
  }
  bl = borderLine(dsc, byCol);
  if (bl.err()){
    Rcout << "Input error\n";
  }
  else{
    nvSimulate(true);
  }
  
}

// [[Rcpp::export]]
List getVennSetNames(){
  std::vector<std::string> r = bl.getSetNames();
  List result;
  for (unsigned int i = 0; i < r.size(); i++){
    result.push_back(r[i]);
  }
  return result;
}

// [[Rcpp::export]]
List getVennRegion(SEXP n) {
  List result;
  if (TYPEOF(n) == STRSXP){
    StringVector sv = as<StringVector>(n);
    std::vector<std::string> v(sv.size()); 
    for (unsigned int i = 0; i < sv.size(); i++){
      v[i] = as<std::string>(sv(i));
    }
    result = bl.getVennRegionVectorL(v);
  }
  else if ((TYPEOF(n) == INTSXP) || (TYPEOF(n) == REALSXP)){
    IntegerVector iv = as<IntegerVector>(n);
    unsigned int nreg = bl.nregions();
    unsigned int v = iv(0);
    if (v <= nreg){
      std::vector<std::string> rv = bl.getVennRegionVector(v);
      if (rv.size() > 0){
        for (unsigned int i = 0; i < rv.size(); i++){
          result.push_back(rv[i]);
        }
      }
    }
  }
  return result;
}


// [[Rcpp::export]]
String getVennSvg() {
    return bl.toSVG().getText() ;
}
